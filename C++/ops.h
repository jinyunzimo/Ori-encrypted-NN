#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>

#include "seal/seal.h"
#include <math.h>
#include <stdio.h>
#include <omp.h>

using namespace std;
using namespace seal;


#include "seal/seal.h"
#include "image_reader.h"

using namespace std;
using namespace seal;

void generate_Cipher(PolyCRTBuilder& crtbuilder,
	Encryptor& encryptor,
	int slot_size,
	Ciphertext& tmp_cipher,
	int fill_number);
void generate_Plain(PolyCRTBuilder& crtbuilder,
	int slot_size,
	Plaintext& tmp_plain,
	int fill_number);

/*
Square activation function, the input array is 2-dimention.
*/
void square(Evaluator& evaluator, EvaluationKeys& ev_keys, vector<vector<Ciphertext>>& in_images);
/*
Square activation function, the input array is 1-dimention.
*/
void square(Evaluator& evaluator, EvaluationKeys& ev_keys, vector<Ciphertext>& in_images);
/*
weight sum
Input:
	crtbuilder:
	encryptor:
	evaluator:
	ev_keys:
	in_images: the input array
	out_images: the output array
	weight: weight matrix
	bias: 
*/
void weight_sum(PolyCRTBuilder& crtbuilder,	Encryptor& encryptor, Evaluator& evaluator, EvaluationKeys& ev_keys, vector<Ciphertext>& in_images, vector<Ciphertext>& out_images, vector<vector<int64_t>> weight, vector<int64_t> bias);

/*
convolution
Input:
	crtbuilder:
	encryptor:
	evaluator:
	ev_keys:
	in_images: the input array
	out_images: the output array
	conv_kernel: convolution kernel
	bias:
*/
void conv_same(PolyCRTBuilder& crtbuilder,
	Encryptor& encryptor,
	Evaluator& evaluator,
	EvaluationKeys& ev_keys,
	vector<vector<Ciphertext>>& in_images,
	vector<vector<Ciphertext>>& out_images,
	vector<vector<vector<vector<int64_t>>>> conv_kernel,
	vector<int64_t> bias,
	Decryptor& decryptor);

void conv_valid(PolyCRTBuilder& crtbuilder,
	Encryptor& encryptor,
	Evaluator& evaluator,
	EvaluationKeys& ev_keys,
	vector<vector<Ciphertext>>& in_images,
	vector<vector<Ciphertext>>& out_images,
	vector<vector<vector<vector<int64_t>>>> conv_kernel,
	vector<int64_t> bias,
	Decryptor& decryptor);

void conv_valid(PolyCRTBuilder& crtbuilder,
	Encryptor& encryptor,
	Evaluator& evaluator,
	EvaluationKeys& ev_keys,
	vector<vector<Ciphertext>>& in_images,
	vector<vector<Ciphertext>>& out_images,
	vector<vector<vector<vector<int64_t>>>>& conv_kernel,
	int conv_stride[],
	vector<int64_t> bias,
	Decryptor& decryptor);

/*
Take the sum in a pool
Input:
	evaluator:
	ksize: pool size
	stride: step size
	in_images:
	out_images:
*/
void sum_pool(
	Evaluator& evaluator,
	vector<int>& ksize,
	vector<int>& stride,
	vector<vector<Ciphertext>>& in_images,
	vector<vector<Ciphertext>>& out_images);

void sum_pool_valid(
	Evaluator& evaluator,
	vector<int>& ksize,
	vector<int>& stride,
	vector<vector<Ciphertext>>& in_images,
	vector<vector<Ciphertext>>& out_images);