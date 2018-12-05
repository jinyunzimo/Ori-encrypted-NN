#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "seal/seal.h"

#include "images_encryptor.h"
using namespace std;
using namespace seal;

void print_banner(string title, const char symbol);
void print_matrix(const vector<int64_t> &matrix, int print_size);
void print_parameters(const SEALContext &context);