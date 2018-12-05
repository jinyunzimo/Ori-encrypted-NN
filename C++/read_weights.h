#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include "read_weights.h"

using namespace std;

class model_parameters
{
public:
	vector<vector<vector<vector<int64_t>>>> conv1_w;
	vector<int64_t> conv1_b;
	vector<vector<vector<vector<int64_t>>>> conv2_w;
	vector<int64_t> conv2_b;
	vector<vector<int64_t>> fc1_w;
	vector<int64_t> fc1_b;
	vector<vector<int64_t>> fc2_w;
	vector<int64_t> fc2_b;
	// read model parameters
	void set_parameters(string flodername);
};

class model_conv1_parameters
{
public:
	vector<vector<vector<vector<int64_t>>>> conv1_w;
	vector<int64_t> conv1_b;
	vector<vector<int64_t>> fc1_w;
	vector<int64_t> fc1_b;
	// read model parameters
	void set_parameters(string flodername);
};

// read a 4-dimention array
void read_4d_array(string filename, vector<vector<vector<vector<int64_t>>>>& kernel);
// read a 1-dimention array
void read_1d_array(string filename, vector<int64_t>& bias);
// read a 2-dimention array
void read_2d_array(string filename, vector<vector<int64_t>>& weights);