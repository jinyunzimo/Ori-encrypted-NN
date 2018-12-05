#include "pch.h"
#include "read_weights.h"
#include<cmath>

void read_4d_array(string filename, vector<vector<vector<vector<int64_t>>>>& kernel)
{
	ifstream infile(filename, ios::in);
	string line;
	getline(infile, line);
	cout << "The kernel shape is：" << line << endl;
	// Store convolution kernel information
	stringstream ss_line(line);
	string str;
	vector<int> line_array;
	//Divided by comma and saved as integer data
	while (getline(ss_line, str, ','))
	{
		stringstream str_temp(str);
		int int_temp;
		str_temp >> int_temp;
		line_array.push_back(int_temp);
	}
	int num_out_channel = line_array[0];
	int	num_in_channel = line_array[1];
	int	num_width = line_array[2];

	// Read file information line by line and store
	for (int index_out_channel = 0; index_out_channel < num_out_channel; index_out_channel++)
	{
		// Used to store an in_channel convolution kernel
		vector<vector<vector<int64_t>>> one_in_kernel;
		for (int index_in_channel = 0; index_in_channel < num_in_channel; index_in_channel++)
		{
			// Used to store a two-dimensional convolution kernel
			vector<vector<int64_t>> one_kernel;
			for (int index_row = 0; index_row < num_width; index_row++)
			{
				getline(infile, line);
				stringstream tmp_line(line);
				// a row used to store the convolution kernel
				vector<int64_t> tmp_int_line;
				while (getline(tmp_line, str, ','))
				{
					stringstream str_temp(str);
					float int_temp;
					str_temp >> int_temp;
					tmp_int_line.push_back(round(int_temp/100));
				}
				one_kernel.push_back(tmp_int_line);
			}
			one_in_kernel.push_back(one_kernel);
		}
		kernel.push_back(one_in_kernel);
	}
}
void read_1d_array(string filename, vector<int64_t>& bias)
{
	// Read the offset item, the offset item is a number of one line
	// open a file
	ifstream infile(filename, ios::in);
	string line;
	while (getline(infile, line))
	{
		stringstream tmp_line(line);
		float int_tmp;
		tmp_line >> int_tmp;
		bias.push_back(round(int_tmp/100));
	}
}
void read_2d_array(string filename, vector<vector<int64_t>>& weights)
{
	// Read the two-dimensional stored file, mainly the weight vector of the matrix multiplication.
	ifstream infile(filename, ios::in);
	string line;
	while (getline(infile, line))
	{
		stringstream ss_line(line);
		string str;
		vector<int64_t> line_array;
		while (getline(ss_line, str, ','))
		{
			stringstream str_temp(str);
			float int_temp;
			str_temp >> int_temp;
			line_array.push_back(round(int_temp/100));
		}
		weights.push_back(line_array);
	}
}

void model_parameters::set_parameters(string flodername)
{
	read_4d_array(flodername + "/conv1_w.txt", conv1_w);
	read_4d_array(flodername + "/conv2_w.txt", conv2_w);
	read_2d_array(flodername + "/fc1_w.txt", fc1_w);
	read_2d_array(flodername + "/fc2_w.txt", fc2_w);
	read_1d_array(flodername + "/conv1_b.txt", conv1_b);
	read_1d_array(flodername + "/conv2_b.txt", conv2_b);
	read_1d_array(flodername + "/fc1_b.txt", fc1_b);
	read_1d_array(flodername + "/fc2_b.txt", fc2_b);
}

void model_conv1_parameters::set_parameters(string flodername)
{
	read_4d_array(flodername + "/conv1_w.txt", conv1_w);
	read_1d_array(flodername + "/conv1_b.txt", conv1_b);
	read_2d_array(flodername + "/fc1_w.txt", fc1_w);
	read_1d_array(flodername + "/fc1_b.txt", fc1_b);
}
