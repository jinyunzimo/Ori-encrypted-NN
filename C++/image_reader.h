#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <math.h>

using namespace std;

/*
read the mnist's label
Input:
	filename: filename
	labels: label
*/
void read_Mnist_Label(string filename, vector<int64_t>& lables);

/*
read the mnist's images
Input:
	filename: filename
	images: images
*/
void read_Mnist_Images(string filename, vector<vector<int64_t>>&images);

/*
Convert input data to integers
*/
int ReverseInt(int i);