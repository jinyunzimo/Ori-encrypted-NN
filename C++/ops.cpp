#include "pch.h"
#include "ops.h"


void generate_Cipher(PolyCRTBuilder& crtbuilder,
	Encryptor& encryptor,
	int slot_size,
	Ciphertext& tmp_cipher,
	int fill_number)
{
	vector<int64_t> tmp;
	for (int j = 0; j < slot_size; j++)
	{
		tmp.push_back(fill_number);
	}
	Plaintext tmp_plain;
	crtbuilder.compose(tmp, tmp_plain);
	encryptor.encrypt(tmp_plain, tmp_cipher);
}

void generate_Plain(PolyCRTBuilder& crtbuilder,
	int slot_size,
	Plaintext& tmp_plain,
	int fill_number)
{
	vector<int64_t> tmp;
	for (int j = 0; j < slot_size; j++)
	{
		tmp.push_back(fill_number);
	}
	crtbuilder.compose(tmp, tmp_plain);;
}
void square(Evaluator& evaluator, 
			EvaluationKeys& ev_keys, 
			vector<vector<Ciphertext>>& in_images)
{
	int num_channel = in_images.size();
	int num_rows = in_images[0].size();
#pragma omp parallel for shared(in_images)
	for (int i = 0; i < num_channel; i++)
	{
		for (int j = 0; j < num_rows; j++)
		{
			evaluator.square(in_images[i][j]);
			evaluator.relinearize(in_images[i][j], ev_keys);
		}
	}
}
void square(Evaluator& evaluator,
	EvaluationKeys& ev_keys,
	vector<Ciphertext>& in_images)
{
	int num_rows = in_images.size();
#pragma omp parallel for shared(in_images)
	for (int j = 0; j < num_rows; j++)
	{
		evaluator.square(in_images[j]);
		evaluator.relinearize(in_images[j], ev_keys);
	}
}
void weight_sum(PolyCRTBuilder& crtbuilder, Encryptor& encryptor, Evaluator& evaluator, EvaluationKeys& ev_keys, vector<Ciphertext>& in_images, vector<Ciphertext>& out_images, vector<vector<int64_t>> weight, vector<int64_t> bias)
{
	// Determine if there is a format error, which makes it impossible to do weights and
	int rows_in_images = in_images.size();
	int rows_weight = weight[0].size();
	int col_weight = weight.size();
	int slot_size = crtbuilder.slot_count();
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
		vector<int64_t> tmp;
		for (int j = 0; j < slot_size; j++)
		{
			tmp.push_back(0);
		}
		Plaintext tmp_plain;
		crtbuilder.compose(tmp, tmp_plain);
		Ciphertext tmp_cipher;
		encryptor.encrypt(tmp_plain, tmp_cipher);
		out_images.push_back(tmp_cipher);
	}


	Ciphertext tmp_0;
	generate_Cipher(crtbuilder, encryptor, crtbuilder.slot_count(), tmp_0, 0);
	// Do the same for each row (ie, CRT-encoded ciphertext does the same thing) to get the output matrix
#pragma omp parallel for shared(in_images, out_images)
	for (int i = 0; i < col_weight; i++)
	{
		for (int j = 0; j < rows_weight; j++)
		{
			// Extend the to-be-multiplied elements to slot_size
			vector<int64_t> tmp_mul;
			for (int k = 0; k < slot_size; k++)
			{
				tmp_mul.push_back(weight[i][j]);
			}
			Plaintext tmp_plain_mul;
			crtbuilder.compose(tmp_mul, tmp_plain_mul);
			// Multiply
			Ciphertext tmp_image = Ciphertext(in_images[j]);
			if (tmp_plain_mul.is_zero())
				tmp_image = tmp_0;
			else
				evaluator.multiply_plain(tmp_image, tmp_plain_mul);
			// evaluator.multiply_plain(tmp_image, tmp_plain_mul);
			evaluator.relinearize(tmp_image, ev_keys);
			// add
			evaluator.add(out_images[i], tmp_image);
		}
		Plaintext tmp_bias;
		vector<int64_t> tmp_add;
		for (int k = 0; k < slot_size; k++)
		{
			tmp_add.push_back(bias[i]);
		}
		crtbuilder.compose(tmp_add, tmp_bias);
		evaluator.add_plain(out_images[i], tmp_bias);
	}
}

void conv_same(PolyCRTBuilder & crtbuilder, 
	Encryptor & encryptor, 
	Evaluator & evaluator, 
	EvaluationKeys & ev_keys, 
	vector<vector<Ciphertext>>& in_images, 
	vector<vector<Ciphertext>>& out_images, 
	vector<vector<vector<vector<int64_t>>>> conv_kernel, 
	vector<int64_t> bias, 
	Decryptor & decryptor)
{
	int num_outchannel = conv_kernel.size();
	int num_inchannel = conv_kernel[0].size();
	int height_core = conv_kernel[0][0].size();
	int width_core = conv_kernel[0][0][0].size();

	if (num_inchannel != in_images.size())
	{
		cout << "There is a shape error between the core's shape and in images' shape" << endl;
		return;
	}
	if (height_core != width_core || height_core % 2 != 1)
	{
		cout << "There is a shape error in height and width of the core!" << endl;
		return;
	}

	int num_new_fill = height_core / 2;
	int length_old_image = in_images[0].size();
	int height = sqrt(length_old_image);
	// padding the new matrix
	// Fill the matrix of each channel of the input
	vector<vector<Ciphertext>> new_in_images;
	Ciphertext tmp_0;
	generate_Cipher(crtbuilder, encryptor, crtbuilder.slot_count(), tmp_0, 0);
	for (int i = 0; i < num_inchannel; i++)
	{
		vector<Ciphertext> each_image;
		for (int j = 0; j < num_new_fill * (height + num_new_fill * 2); j++)
		{
			each_image.push_back(tmp_0);
		}
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < num_new_fill; k++)
			{
				each_image.push_back(tmp_0);
			}
			for (int k = 0; k < height; k++)
			{
				each_image.push_back(in_images[i][j*height + k]);
			}
			for (int k = 0; k < num_new_fill; k++)
			{
				each_image.push_back(tmp_0);
			}
		}
		for (int j = 0; j < num_new_fill * (height + num_new_fill * 2); j++)
		{
			each_image.push_back(tmp_0);
		}
		new_in_images.push_back(each_image);
	}

	// The matrix of predefined results, initially set to 0
	for (int i = 0; i < num_outchannel; i++)
	{
		vector<Ciphertext> tmp_image;
		for (int j = 0; j < length_old_image; j++)
		{
			tmp_image.push_back(tmp_0);
		}
		out_images.push_back(tmp_image);
	}

#pragma omp parallel for shared(new_in_images, out_images)
	// Convolution operation, for each output channel, that is, for each convolution kernel, multiply the corresponding bits, then add
	for (int index_out_channel = 0; index_out_channel < num_outchannel; index_out_channel++)
	{
		printf("channel: %d\n", index_out_channel);
		//cout << "channel: " << index_out_channel << endl;
		// This layer of results needs to be added. Equivalent to one row vector multiplied by one column vector.
		for (int index_in_channel = 0; index_in_channel < num_inchannel; index_in_channel++)
		{
			// Predefine the output of a single-layer convolution result
			vector<Ciphertext> tmp_out(height * height);
			// Take out the convolution kernel
			vector<Plaintext> one_kernel(height_core * height_core);
			for (int tmp_i = 0; tmp_i < height_core; tmp_i++)
			{
				for (int tmp_j = 0; tmp_j < width_core; tmp_j++)
				{
					generate_Plain(crtbuilder, crtbuilder.slot_count(),
						one_kernel[tmp_i * height_core + tmp_j], conv_kernel[index_out_channel][index_in_channel][tmp_i][tmp_j]);
				}
			}
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < height; j++)
				{
					// Take the area where the current point is the center of the convolution kernel size, convolution, 
					// which is equivalent to the corresponding point multiplication and finally add, so use the line vector storage
					vector<Ciphertext> tmp_line;
					for (int tmp_i = 0; tmp_i < height_core; tmp_i++)
					{
						for (int tmp_j = 0; tmp_j < width_core; tmp_j++)
						{
							tmp_line.push_back(new_in_images[index_in_channel][(i + tmp_i)*(height + 2 * num_new_fill) + j + tmp_j]);
						}
					}
					for (int index = 0; index < tmp_line.size(); index++)
					{
						// cout << conv_kernel[index_out_channel][index_in_channel][4][4] << " ";
						if (one_kernel[index].is_zero())
							tmp_line[index] = tmp_0;
						else
							evaluator.multiply_plain(tmp_line[index], one_kernel[index]);
						
						evaluator.relinearize(tmp_line[index], ev_keys);
					}
					evaluator.add_many(tmp_line, tmp_out[i * height + j]);

					Plaintext test;
					decryptor.decrypt(tmp_out[i * height + j], test);
					vector<int64_t> testt;
					crtbuilder.decompose(test, testt);
				}
			}
			// Each individual output is added to the final output.
			for (int index = 0; index < tmp_out.size(); index++)
			{
				evaluator.add(out_images[index_out_channel][index], tmp_out[index]);
				Plaintext test;
				decryptor.decrypt(out_images[index_out_channel][index], test);
				vector<int64_t> testt;
				crtbuilder.decompose(test, testt);
			}

		}

		// add the bias
		Plaintext tmp_bias;
		generate_Plain(crtbuilder, crtbuilder.slot_count(), tmp_bias, bias[index_out_channel]);
		for (int index = 0; index < height * height; index++)
		{
			evaluator.add_plain(out_images[index_out_channel][index], tmp_bias);
		}
	}
}

void conv_valid(PolyCRTBuilder & crtbuilder, Encryptor & encryptor, Evaluator & evaluator, EvaluationKeys & ev_keys, vector<vector<Ciphertext>>& in_images, vector<vector<Ciphertext>>& out_images, vector<vector<vector<vector<int64_t>>>> conv_kernel, vector<int64_t> bias, Decryptor & decryptor)
{
	int num_outchannel = conv_kernel.size();
	int num_inchannel = conv_kernel[0].size();
	int height_core = conv_kernel[0][0].size();
	int width_core = conv_kernel[0][0][0].size();

	if (num_inchannel != in_images.size())
	{
		cout << "There is a shape error between the core's shape and in images' shape" << endl;
		return;
	}
	if (height_core != width_core || height_core % 2 != 1)
	{
		cout << "There is a shape error in height and width of the core!" << endl;
		return;
	}

	int num_new_fill = height_core / 2;
	int length_old_image = in_images[0].size();
	int height = sqrt(length_old_image);
	// padding the new matrix
	// Fill the matrix of each channel of the input
	vector<vector<Ciphertext>> new_in_images;
	Ciphertext tmp_0;
	generate_Cipher(crtbuilder, encryptor, crtbuilder.slot_count(), tmp_0, 0);
	for (int i = 0; i < num_inchannel; i++)
	{
		vector<Ciphertext> each_image;
		for (int j = 0; j < num_new_fill * (height + num_new_fill * 2); j++)
		{
			each_image.push_back(tmp_0);
		}
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < num_new_fill; k++)
			{
				each_image.push_back(tmp_0);
			}
			for (int k = 0; k < height; k++)
			{
				each_image.push_back(in_images[i][j*height + k]);
			}
			for (int k = 0; k < num_new_fill; k++)
			{
				each_image.push_back(tmp_0);
			}
		}
		for (int j = 0; j < num_new_fill * (height + num_new_fill * 2); j++)
		{
			each_image.push_back(tmp_0);
		}
		new_in_images.push_back(each_image);
	}

	// The matrix of predefined results, initially set to 0
	for (int i = 0; i < num_outchannel; i++)
	{
		vector<Ciphertext> tmp_image;
		for (int j = 0; j < length_old_image; j++)
		{
			tmp_image.push_back(tmp_0);
		}
		out_images.push_back(tmp_image);
	}

#pragma omp parallel for shared(new_in_images, out_images)
	// Convolution operation, for each output channel, that is, for each convolution kernel, multiply the corresponding bits, then add
	for (int index_out_channel = 0; index_out_channel < num_outchannel; index_out_channel++)
	{
		printf("channel: %d\n", index_out_channel);
		//cout << "channel: " << index_out_channel << endl;
		// This layer of results needs to be added. Equivalent to one row vector multiplied by one column vector.
		for (int index_in_channel = 0; index_in_channel < num_inchannel; index_in_channel++)
		{
			// Predefine the output of a single-layer convolution result
			vector<Ciphertext> tmp_out(height * height);
			// Take out the convolution kernel
			vector<Plaintext> one_kernel(height_core * height_core);
			for (int tmp_i = 0; tmp_i < height_core; tmp_i++)
			{
				for (int tmp_j = 0; tmp_j < width_core; tmp_j++)
				{
					generate_Plain(crtbuilder, crtbuilder.slot_count(),
						one_kernel[tmp_i * height_core + tmp_j], conv_kernel[index_out_channel][index_in_channel][tmp_i][tmp_j]);
				}
			}
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < height; j++)
				{
					// Take the area where the current point is the center of the convolution kernel size, convolution, 
					// which is equivalent to the corresponding point multiplication and finally add, so use the line vector storage
					vector<Ciphertext> tmp_line;
					for (int tmp_i = 0; tmp_i < height_core; tmp_i++)
					{
						for (int tmp_j = 0; tmp_j < width_core; tmp_j++)
						{
							tmp_line.push_back(new_in_images[index_in_channel][(i + tmp_i)*(height + 2 * num_new_fill) + j + tmp_j]);
						}
					}
					for (int index = 0; index < tmp_line.size(); index++)
					{
						// cout << conv_kernel[index_out_channel][index_in_channel][4][4] << " ";
						if (one_kernel[index].is_zero())
							tmp_line[index] = tmp_0;
						else
							evaluator.multiply_plain(tmp_line[index], one_kernel[index]);

						evaluator.relinearize(tmp_line[index], ev_keys);
					}
					evaluator.add_many(tmp_line, tmp_out[i * height + j]);

					Plaintext test;
					decryptor.decrypt(tmp_out[i * height + j], test);
					vector<int64_t> testt;
					crtbuilder.decompose(test, testt);
				}
			}
			// Each individual output is added to the final output.
			for (int index = 0; index < tmp_out.size(); index++)
			{
				evaluator.add(out_images[index_out_channel][index], tmp_out[index]);
				Plaintext test;
				decryptor.decrypt(out_images[index_out_channel][index], test);
				vector<int64_t> testt;
				crtbuilder.decompose(test, testt);
			}

		}

		// add the bias
		Plaintext tmp_bias;
		generate_Plain(crtbuilder, crtbuilder.slot_count(), tmp_bias, bias[index_out_channel]);
		for (int index = 0; index < height * height; index++)
		{
			evaluator.add_plain(out_images[index_out_channel][index], tmp_bias);
		}
	}
}

void conv_valid(PolyCRTBuilder & crtbuilder, Encryptor & encryptor, Evaluator & evaluator, EvaluationKeys & ev_keys, vector<vector<Ciphertext>>& in_images, vector<vector<Ciphertext>>& out_images, vector<vector<vector<vector<int64_t>>>>& conv_kernel, int conv_stride[], vector<int64_t> bias, Decryptor & decryptor)
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

	// Ciphertext with slot_size 0s
	Ciphertext tmp_0;
	generate_Cipher(crtbuilder, encryptor, crtbuilder.slot_count(), tmp_0, 0);
	// Predefined the output matrix size
	for (int i = 0; i < num_outchannel; i++)
	{
		vector<Ciphertext> tmp_image;
		for (int j = 0; j < pow(new_images_width, 2); j++)
		{
			tmp_image.push_back(tmp_0);
		}
		out_images.push_back(tmp_image);
	}
	
	// The calculation of each channel can be parallel
#pragma omp parallel for shared(in_images, out_images)
	// Convolution operation, for each output channel, that is, for each convolution kernel, multiply the corresponding bits, then add
	for (int index_out_channel = 0; index_out_channel < num_outchannel; index_out_channel++)
	{
		printf("channel: %d\n", index_out_channel);
		//cout << "channel: " << index_out_channel << endl;
		// This layer of results needs to be added. Equivalent to one row vector multiplied by one column vector.
		for (int index_in_channel = 0; index_in_channel < num_inchannel; index_in_channel++)
		{
			// Predefine the output of a single-layer convolution result
			vector<Ciphertext> tmp_out(new_images_width * new_images_width);
			// Take out the convolution kernel
			vector<Plaintext> one_kernel(height_core * height_core);
			for (int tmp_i = 0; tmp_i < height_core; tmp_i++)
			{
				for (int tmp_j = 0; tmp_j < width_core; tmp_j++)
				{
					generate_Plain(crtbuilder, crtbuilder.slot_count(),
						one_kernel[tmp_i * height_core + tmp_j], conv_kernel[index_out_channel][index_in_channel][tmp_i][tmp_j]);
				}
			}
			for (int i = 0; i < new_images_width; i++)
			{
				for (int j = 0; j < new_images_width; j++)
				{
					// Take the area where the current point is the left-up of the convolution kernel size, convolution, 
					// which is equivalent to the corresponding point multiplication and finally add, so use the line vector storage
					vector<Ciphertext> tmp_line;
					for (int tmp_i = 0; tmp_i < height_core; tmp_i++)
					{
						for (int tmp_j = 0; tmp_j < width_core; tmp_j++)
						{
							tmp_line.push_back(in_images[index_in_channel][(i * stride_height + tmp_i) * height + j * stride_width + tmp_j]);
						}
					}
					for (int index = 0; index < tmp_line.size(); index++)
					{
						// cout << conv_kernel[index_out_channel][index_in_channel][4][4] << " ";
						if (one_kernel[index].is_zero())
							tmp_line[index] = tmp_0;
						else
							evaluator.multiply_plain(tmp_line[index], one_kernel[index]);

						// evaluator.relinearize(tmp_line[index], ev_keys);
					}
					evaluator.add_many(tmp_line, tmp_out[i * new_images_width + j]);
					/*Plaintext test;
					decryptor.decrypt(tmp_out[i * height + j], test);
					vector<int64_t> testt;
					crtbuilder.decompose(test, testt);*/
				}
			}
			// Each individual output is added to the final output.
			for (int index = 0; index < tmp_out.size(); index++)
			{
				evaluator.add(out_images[index_out_channel][index], tmp_out[index]);
				/*Plaintext test;
				decryptor.decrypt(out_images[index_out_channel][index], test);
				vector<int64_t> testt;
				crtbuilder.decompose(test, testt);*/
			}
		}

		// add the bias
		Plaintext tmp_bias;
		generate_Plain(crtbuilder, crtbuilder.slot_count(), tmp_bias, bias[index_out_channel]);
		for (int index = 0; index < new_images_width * new_images_width; index++)
		{
			evaluator.add_plain(out_images[index_out_channel][index], tmp_bias);
		}
	}
}

void sum_pool(
	Evaluator& evaluator,
	vector<int>& ksize,
	vector<int>& stride,
	vector<vector<Ciphertext>>& in_images,
	vector<vector<Ciphertext>>& out_images)
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

	if (pool_channel != 1 && stride_channel != 1)
	{
		printf("Wrong shape of input channel! (%d), (%d) Please rewrite the code to accomplish this function\n",
			pool_channel, stride_channel);
		return;
	}

	int new_height = height / stride_h;

	// Predefined output matrix size
	vector<Ciphertext> tmp(new_height*new_height);
	for (int index_channel = 0; index_channel < num_channel; index_channel++)
	{
		int index_out = 0;
		out_images.push_back(tmp);
		for (int i = 0; i < height; i += stride_h)
		{
			for (int j = 0; j < height; j += stride_w)
			{
				vector<Ciphertext> tmp_add;
				for (int tmp_i = 0; tmp_i < h_pooling; tmp_i++)
				{
					for (int tmp_j = 0; tmp_j < w_pooling; tmp_j++)
					{
						tmp_add.push_back(in_images[index_channel][(i + tmp_i)*height + j + tmp_j]);
					}
				}
				evaluator.add_many(tmp_add, out_images[index_channel][index_out++]);
			}
		}
	}
}

void sum_pool_valid(Evaluator & evaluator, vector<int>& ksize, vector<int>& stride, vector<vector<Ciphertext>>& in_images, vector<vector<Ciphertext>>& out_images)
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
	vector<Ciphertext> tmp(new_height*new_height);
	for (int index_channel = 0; index_channel < num_channel; index_channel++)
	{
		out_images.push_back(tmp);
	}
#pragma omp parallel for shared(in_images, out_images)
	for (int index_channel = 0; index_channel < num_channel; index_channel++)
	{
		int index_out = 0;
		
		for (int i = 0; i < new_height; i += stride_h)
		{
			for (int j = 0; j < new_height; j += stride_w)
			{
				// Calculate the pool sum centered on the current point
				vector<Ciphertext> tmp_add;
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
						continue;
					for (int tmp_j = j - pre_size; tmp_j <= j + pre_size; tmp_j++)
					{
						if (tmp_i < 0 || tmp_i >= new_height || tmp_j < 0 || tmp_j >= height)
							continue;
						tmp_add.push_back(in_images[index_channel][tmp_i * new_height + tmp_j]);
					}
				}
				evaluator.add_many(tmp_add, out_images[index_channel][index_out++]);
			}
		}
	}
}
