#pragma once
#include <vector>
#include <mpirxx.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

/*
The identified number is determined based on the final output.
Each picture's output is shaped as [10*1], and the index corresponding to the largest value is selected as the recognized number.
@result: the final output of the NN
@target: the original number of the picture
*/
void identify_num(vector<vector<int64_t>> result, vector<int64_t> target);

void identify_num(vector<vector<mpz_class>> result, vector<int64_t> target);

