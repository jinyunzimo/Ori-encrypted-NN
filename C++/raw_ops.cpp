#include "pch.h"
#include "raw_ops.h"
void square(vector<vector<int64_t>>& in_images)
{
	int num_channel = in_images.size();
	int num_rows = in_images[0].size();
#pragma omp parallel for shared(in_images)
	for (int i = 0; i < num_channel; i++)
	{
		for (int j = 0; j < num_rows; j++)
		{
			in_images[i][j] = in_images[i][j] * in_images[i][j];
		}
	}
}

void square(vector<int64_t>& in_images)
{
	int num_rows = in_images.size();
#pragma omp parallel for shared(in_images)
	for (int j = 0; j < num_rows; j++)
	{
		in_images[j] = in_images[j] * in_images[j];
	}
}

void weight_sum(vector<int64_t>& in_images, vector<int64_t>& out_images, vector<vector<int64_t>> weight, vector<int64_t> bias)
{
	int rows_in_images = in_images.size();
	int rows_weight = weight[0].size();
	int col_weight = weight.size();
	try
	{
		if (rows_in_images != rows_weight)
		{
			throw "There is a shape error between the in images' shape and weight's shape!";
		}
		if (col_weight != bias.size())
		{
			throw "There is a shape error between the in bias's shape and weight's shape!";
		}
	}
	catch (int i)
	{
		cout << " Error occuring--" << i << "%0" << endl;
	}

	// Predefined output matrix, each element is 0
	for (int i = 0; i < col_weight; i++)
	{
		out_images.push_back(0);
	}
	// Do the same for each row (ie, CRT-encoded ciphertext does the same thing) to get the output matrix
	for (int i = 0; i < col_weight; i++)
	{
		for (int j = 0; j < rows_weight; j++)
		{
			out_images[i] += in_images[j] * weight[i][j];
		}
		out_images[i] += bias[i];
	}
}

void conv_valid(vector<vector<int64_t>>& in_images, vector<vector<int64_t>>& out_images, vector<vector<vector<vector<int64_t>>>>& conv_kernel, int conv_stride[], vector<int64_t> bias)
{
	// the number of output channels
	int num_outchannel = conv_kernel.size();
	// the number of input channels
	int num_inchannel = conv_kernel[0].size();
	// the height of the convolution kernel
	int height_core = conv_kernel[0][0].size();
	// the width of the convolution kernel
	int width_core = conv_kernel[0][0][0].size();
	// the step size in height
	int stride_height = conv_stride[0];
	// the step size in width
	int stride_width = conv_stride[1];
	try {
		if (stride_height != stride_width)
		{
			// if the step size in height and the step size in width are not equal, throw an exception
			throw "Stride_height and stride width must be equal";
		}
	}
	catch (int i)
	{
		cout << " Error occuring--" << i << "%0" << endl;
	}

	try {
		if (num_inchannel != in_images.size())
		{
			// the number of the input channels of the convolution and this of input images are not equal, throw an exception
			throw "There is a shape error between the core's shape and in images' shape";
		}
		if (height_core != width_core || height_core % 2 != 1)
		{
			// if the height and width of the convolution kernel are not equal, or they are not odds, throw an exception
			throw "There is a shape error in height and width of the core!";
		}
	}
	catch (int i)
	{
		cout << " Error occuring--" << i << "%0" << endl;
	}
	// in_images[0].size() represents the images' height * width
	int length_old_image = in_images[0].size();
	int height = sqrt(length_old_image);
	// the size of output images
	int new_images_width = ceil((height - height_core + 1) / stride_height);

	// Predefined the output matrix size
	for (int i = 0; i < num_outchannel; i++)
	{
		vector<int64_t> tmp_image;
		for (int j = 0; j < pow(new_images_width, 2); j++)
		{
			tmp_image.push_back(0);
		}
		out_images.push_back(tmp_image);
	}

	// The calculation of each channel can be parallel
#pragma omp parallel for shared(in_images, out_images)
	// Convolution operation, for each output channel, that is, for each convolution kernel, multiply the corresponding bits, then add
	for (int index_out_channel = 0; index_out_channel < num_outchannel; index_out_channel++)
	{
		//printf("channel: %d\n", index_out_channel);
		//cout << "channel: " << index_out_channel << endl;
		// This layer of results needs to be added. Equivalent to one row vector multiplied by one column vector.
		for (int index_in_channel = 0; index_in_channel < num_inchannel; index_in_channel++)
		{
			// Predefine the output of a single-layer convolution result
			vector<int64_t> tmp_out(new_images_width * new_images_width);
			// Take out the convolution kernel
			vector<int64_t> one_kernel(height_core * height_core);
			for (int tmp_i = 0; tmp_i < height_core; tmp_i++)
			{
				for (int tmp_j = 0; tmp_j < width_core; tmp_j++)
				{
					one_kernel[tmp_i * height_core + tmp_j] = conv_kernel[index_out_channel][index_in_channel][tmp_i][tmp_j];
				}
			}
			for (int i = 0; i < new_images_width; i++)
			{
				for (int j = 0; j < new_images_width; j++)
				{
					// Take the area where the current point is the left-up of the convolution kernel size, convolution, 
					// which is equivalent to the corresponding point multiplication and finally add, so use the line vector storage
					vector<int64_t> tmp_line;
					for (int tmp_i = 0; tmp_i < height_core; tmp_i++)
					{
						for (int tmp_j = 0; tmp_j < width_core; tmp_j++)
						{
							tmp_line.push_back(in_images[index_in_channel][(i * stride_height + tmp_i) * height + j * stride_width + tmp_j]);
						}
					}
					int64_t tmp_sum_kernel_weight = 0;
					for (int index = 0; index < tmp_line.size(); index++)
					{
						tmp_sum_kernel_weight += tmp_line[index] * one_kernel[index];
					}
					tmp_out[i * new_images_width + j] = tmp_sum_kernel_weight;
				}
			}
			// Each individual output is added to the final output.
			for (int index = 0; index < tmp_out.size(); index++)
			{
				out_images[index_out_channel][index] += tmp_out[index];
			}
		}

		// add the bias
		for (int index = 0; index < new_images_width * new_images_width; index++)
		{
			out_images[index_out_channel][index] += bias[index_out_channel];
		}
	}
}

void sum_pool_valid(vector<int>& ksize, vector<int>& stride, vector<vector<int64_t>>& in_images, vector<vector<int64_t>>& out_images)
{
	int num_channel = in_images.size();
	int length_old_image = in_images[0].size();
	int height = sqrt(length_old_image);

	int h_pooling = ksize[1];
	int w_pooling = ksize[2];
	int pool_channel = ksize[3];

	int stride_h = stride[1];
	int stride_w = stride[2];
	int stride_channel = stride[3];
	try {
		if (pool_channel != 1 && stride_channel != 1)
		{
			throw ("Wrong shape of input channel! (%d), (%d) Please rewrite the code to accomplish this function\n",
				pool_channel, stride_channel);
		}
	}
	catch (int i)
	{
		cout << " Error occuring--" << i << "%0" << endl;
	}

	int new_height = height / stride_h;
	int pre_size = h_pooling / 2;
	// Predefined output matrix size
	vector<int64_t> tmp(new_height*new_height);
	for (int index_channel = 0; index_channel < num_channel; index_channel++)
	{
		out_images.push_back(tmp);
	}
	for (int index_channel = 0; index_channel < num_channel; index_channel++)
	{
		int index_out = 0;

		for (int i = 0; i < new_height; i += stride_h)
		{
			for (int j = 0; j < new_height; j += stride_w)
			{
				// Calculate the pool sum centered on the current point
				int64_t tmp_add = 0;
				/*for (int tmp_i = 0; tmp_i < h_pooling; tmp_i++)
				{
					if (i + tmp_i >= height)
						continue;
					for (int tmp_j = 0; tmp_j < h_pooling; tmp_j++)
					{
						if (j + tmp_j >= height)
							continue;
						tmp_add.push_back(in_images[index_channel][(i + tmp_i)*height + j + tmp_j]);
					}
				}*/
				for (int tmp_i = i - pre_size; tmp_i <= i + pre_size; tmp_i++)
				{
					if (tmp_i < 0 || tmp_i >= new_height)
					{
						continue;
					}
					for (int tmp_j = j - pre_size; tmp_j <= j + pre_size; tmp_j++)
					{
						if (tmp_i < 0 || tmp_i >= new_height || tmp_j < 0 || tmp_j >= height)
							continue;
						tmp_add += in_images[index_channel][tmp_i * new_height + tmp_j];
					}
				}
				out_images[index_channel][index_out++] = tmp_add;
			}
		}
	}
}
