#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <math.h>

#include <math.h>
#include <stdio.h>
#include <omp.h>

using namespace std;

void square(vector<vector<int64_t>>& in_images);

void square(vector<int64_t>& in_images);

void weight_sum(vector<int64_t>& in_images, vector<int64_t>& out_images, vector<vector<int64_t>> weight, vector<int64_t> bias);

void conv_valid(
	vector<vector<int64_t>>& in_images,
	vector<vector<int64_t>>& out_images,
	vector<vector<vector<vector<int64_t>>>>& conv_kernel,
	int conv_stride[],
	vector<int64_t> bias);

void sum_pool_valid(
	vector<int>& ksize,
	vector<int>& stride,
	vector<vector<int64_t>>& in_images,
	vector<vector<int64_t>>& out_images);