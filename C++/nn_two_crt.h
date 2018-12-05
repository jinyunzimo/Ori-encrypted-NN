#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <mpirxx.h>

#include "read_weights.h"
#include "seal\seal.h"
#include "print_all.h"
#include "images_encryptor.h"
#include "decrypt_middle_result.h"
#include "ops.h"
#include "exgcd.h"
#include "identify.h"

using namespace std;
using namespace seal;

/*
Use the square activation function to complete a inference
@model_para: the model parameters of the trained neraul network model
@context: the encryption parameter
@images: input images
@return: the result of the inference
*/
vector<vector<int64_t>> process(model_parameters model_para, SEALContext context, vector<vector<int64_t>> images);
/*
Experiment with two sets of parameters, and finally calculate the original value with CRT code.
*/
void test_two_crt();