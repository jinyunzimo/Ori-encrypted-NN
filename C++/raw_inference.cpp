#include "pch.h"
#include "raw_inference.h"
using namespace std;
#pragma warning( disable : 4996 )
void write_to_csv(string filename, vector<vector<int64_t>> num_result)
{
	ofstream middle_file;
	int row_num = sqrt(num_result[0].size());
	middle_file.open(filename, ios::app);
	for (int index_channel = 0; index_channel < num_result.size(); index_channel++)
	{
		for (int index_pixel = 0; index_pixel < num_result[0].size(); index_pixel++)
		{
			/*cout << "testeste3" << endl;*/
			middle_file << num_result[index_channel][index_pixel];
			if ((index_pixel + 1) % row_num == 0)
			{
				middle_file << "\n";
			}
			else
			{
				middle_file << ",";
			}
		}
	}
	middle_file.close();
}

void write_to_csv(string filename, vector<int64_t> num_result)
{
	ofstream middle_file;
	int row_num = num_result.size();
	middle_file.open(filename, ios::app);

	for (int index_pixel = 0; index_pixel < row_num; index_pixel++)
	{
		/*cout << "testeste3" << endl;*/
		middle_file << num_result[index_pixel];
		if ((index_pixel + 1) % row_num == 0)
		{
			middle_file << "\n";
		}
		else
		{
			middle_file << ",";
		}
	}

	middle_file.close();
}

void inference()
{
	// generate a random number, which will be contained in the middel result file
	srand((int)time(0));
	int rand_int = rand() / 100;

	ofstream outfile;
	time_t a_time_start = time(0);
	char log_file_name[64];
	strftime(log_file_name, sizeof(log_file_name), "result/raw_two_crt_%Y%m%d_result.txt", localtime(&a_time_start));
	cout << log_file_name << endl;

	// open the outfile
	outfile.open(log_file_name, ios::app);

	// write the current time into the file
	char str_time[64];
	strftime(str_time, sizeof(str_time), "%Y%m%d %T", localtime(&a_time_start));
	outfile << str_time << endl << endl;
	cout << str_time << endl << endl;
	// read the images and labels of the test set
	print_banner("Read test images and labels", '*');
	vector<int64_t>labels;
	read_Mnist_Label("MINIST/t10k-labels.idx1-ubyte", labels);
	vector<vector<int64_t>> images;
	read_Mnist_Images("MINIST/t10k-images.idx3-ubyte", images);
	print_banner("Finish reading images and labels", '#');
	cout << endl;
	// end reading

	//read the model parameter
	print_banner("Reading model weights", '*');
	model_parameters model_para;
	model_para.set_parameters("weights_5");
	print_banner("Finish reading model weights", '#');
	cout << endl;
	// end


	// time of inference
	chrono::microseconds time_inference(0);

	chrono::high_resolution_clock::time_point time_start, time_end;
	time_start = chrono::high_resolution_clock::now();

	int kernel_stride[2] = { 2,2 };
	vector<vector<int64_t>> results;
	for (int i = 0; i < 10000; i++)
	{
		vector<vector<int64_t>> in_images;
		in_images.push_back(images[i]);
		//write_to_csv("raw_in_images_" + to_string(rand_int) + ".csv", in_images);
		vector<vector<int64_t>> conv1_image;
		conv_valid(in_images, conv1_image, model_para.conv1_w, kernel_stride, model_para.conv1_b);

		//write_to_csv("raw_conv1_" + to_string(rand_int) + ".csv", conv1_image);

		square(conv1_image);

		//write_to_csv("raw_sq1_" + to_string(rand_int) + ".csv", conv1_image);

		vector<int> ksize;
		ksize.push_back(1);
		ksize.push_back(3);
		ksize.push_back(3);
		ksize.push_back(1);
		// step size
		vector<int> stride;
		stride.push_back(1);
		stride.push_back(1);
		stride.push_back(1);
		stride.push_back(1);

		vector<vector<int64_t>> pool1_image;
		sum_pool_valid(ksize, stride, conv1_image, pool1_image);

		//write_to_csv("raw_pool1_" + to_string(rand_int) + ".csv", pool1_image);

		vector<vector<int64_t>> conv2_image;
		conv_valid(pool1_image, conv2_image, model_para.conv2_w, kernel_stride, model_para.conv2_b);

		//write_to_csv("raw_conv2_" + to_string(rand_int) + ".csv", conv2_image);

		vector<vector<int64_t>> pool2_image;
		sum_pool_valid(ksize, stride, conv2_image, pool2_image);

		//write_to_csv("raw_pool2_" + to_string(rand_int) + ".csv", pool2_image);

		vector<int64_t> flat_image;
		for (int i = 0; i < pool2_image[0].size(); i++)
		{
			for (int j = 0; j < pool2_image.size(); j++)
			{
				flat_image.push_back(pool2_image[j][i]);
			}
		}
		//write_to_csv("raw_flat_" + to_string(rand_int) + ".csv", flat_image);

		vector<int64_t> fc1_image;
		weight_sum(flat_image, fc1_image, model_para.fc1_w, model_para.fc1_b);

		//write_to_csv("raw_fc1_" + to_string(rand_int) + ".csv", fc1_image);

		square(fc1_image);

		//write_to_csv("raw_sq2_" + to_string(rand_int) + ".csv", fc1_image);

		vector<int64_t> fc2_image;
		weight_sum(fc1_image, fc2_image, model_para.fc2_w, model_para.fc2_b);

		//write_to_csv("raw_fc2_" + to_string(rand_int) + ".csv", fc2_image);
		results.push_back(fc2_image);		
	}

	time_end = chrono::high_resolution_clock::now();
	// record the interval between encoding and encryption(us)
	auto time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "inference [" << time_inference.count() << " microseconds]" << endl;

	identify_num(results, labels);
}
