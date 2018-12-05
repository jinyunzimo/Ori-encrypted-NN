#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <math.h>


#include "seal/seal.h"
#include "image_reader.h"
#include "images_encryptor.h"

using namespace std;
using namespace seal;

/*
Decrypt the ciphertext to obtain a three-dimensional matrix and take the logarithm of the plaintext.
Input: 
	Decryptor: decryption, 
	PolyCRTBuilder: decoding, 
	result_middle_image: ciphertext matrix to be decrypted, which is a two-dimensinal ciphertext matrix,
	filename: file name to write the decrypted result,
	num_result: a three-dimensional matrix that stores decrypted results.
*/
void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, vector<vector<vector<int64_t>>>& num_result);

/*
Decrypt the ciphertext to obtain a two-dimensional matrix and take the logarithm of the plaintext.
Input:
	Decryptor: decryption,
	PolyCRTBuilder: decoding,
	result_middle_image: ciphertext matrix to be decrypted, which is a one-dimensinal ciphertext matrix,
	filename: file name to write the decrypted result,
	tmp_result: a two-dimensional matrix that stores decrypted results.
*/
void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, vector<vector<int64_t>>& tmp_results);

/*
Decrypt the ciphertext to obtain a three-dimensional matrix and then narrow down the plaintext using division.
Input:
	Decryptor: decryption,
	PolyCRTBuilder: decoding,
	result_middle_image: ciphertext matrix to be decrypted, which is a two-dimensinal ciphertext matrix,
	filename: file name to write the decrypted result,
	num_result: a three-dimensional matrix that stores decrypted results,
	divide_number: Degree of reduction
*/
void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, vector<vector<vector<int64_t>>>& num_result, int64_t divide_number);


/*
Decrypt the ciphertext to obtain a two-dimensional matrix and then narrow down the plaintext using division.
Input:
	Decryptor: decryption,
	PolyCRTBuilder: decoding,
	result_middle_image: ciphertext matrix to be decrypted, which is a one-dimensinal ciphertext matrix,
	filename: file name to write the decrypted result,
	tmp_result: a two-dimensional matrix that stores decrypted results,
	divide_number: Degree of reduction
*/
void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, vector<vector<int64_t>>& tmp_results, int64_t divide_number);

/*
Decrypt the ciphertext to obtain a three-dimensional matrix.
Input:
	Decryptor: decryption,
	PolyCRTBuilder: decoding,
	result_middle_image: ciphertext matrix to be decrypted, which is a two-dimensinal ciphertext matrix,
	filename: file name to write the decrypted result,
	num_result: a three-dimensional matrix that stores decrypted results.
*/
void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, vector<vector<vector<int64_t>>>& num_result);

void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename);

/*
Decrypt the ciphertext to obtain a two-dimensional matrix.
Input:
	Decryptor: decryption,
	PolyCRTBuilder: decoding,
	result_middle_image: ciphertext matrix to be decrypted, which is a one-dimensinal ciphertext matrix,
	filename: file name to write the decrypted result,
	tmp_result: a two-dimensional matrix that stores decrypted results.
*/
void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, vector<vector<int64_t>>& tmp_results);

void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename);

/*
Decrypt the ciphertext to obtain a three-dimensional matrix and do ReLU operation.
Input:
	Decryptor: decryption,
	PolyCRTBuilder: decoding,
	result_middle_image: ciphertext matrix to be decrypted, which is a two-dimensinal ciphertext matrix,
	filename: file name to write the decrypted result,
	num_result: a three-dimensional matrix that stores decrypted results.
*/
void decrpt_middle_result_ReLU(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, vector<vector<vector<int64_t>>>& num_result);

/*
Decrypt the ciphertext to obtain a two-dimensional matrix  and do ReLU operation.
Input:
	Decryptor: decryption,
	PolyCRTBuilder: decoding,
	result_middle_image: ciphertext matrix to be decrypted, which is a one-dimensinal ciphertext matrix,
	filename: file name to write the decrypted result,
	tmp_result: a two-dimensional matrix that stores decrypted results.
*/
void decrpt_middle_result_ReLU(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, vector<vector<int64_t>>& tmp_results);



/***************************************************************************************************************/
/*
The following uses an array implementation
*/
/***************************************************************************************************************/
//
///*
//Decrypt the ciphertext to obtain a three-dimensional matrix and take the logarithm of the plaintext.
//Input:
//	Decryptor: decryption,
//	PolyCRTBuilder: decoding,
//	result_middle_image: ciphertext matrix to be decrypted, which is a two-dimensinal ciphertext matrix,
//	filename: file name to write the decrypted result,
//	num_result: a three-dimensional matrix that stores decrypted results.
//*/
//void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, int*** &num_result);
//
///*
//Decrypt the ciphertext to obtain a two-dimensional matrix and take the logarithm of the plaintext.
//Input:
//	Decryptor: decryption,
//	PolyCRTBuilder: decoding,
//	result_middle_image: ciphertext matrix to be decrypted, which is a one-dimensinal ciphertext matrix,
//	filename: file name to write the decrypted result,
//	tmp_result: a two-dimensional matrix that stores decrypted results.
//*/
//void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, int** &tmp_results);
//
///*
//Decrypt the ciphertext to obtain a three-dimensional matrix and then narrow down the plaintext using division.
//Input:
//	Decryptor: decryption,
//	PolyCRTBuilder: decoding,
//	result_middle_image: ciphertext matrix to be decrypted, which is a two-dimensinal ciphertext matrix,
//	filename: file name to write the decrypted result,
//	num_result: a three-dimensional matrix that stores decrypted results,
//	divide_number: Degree of reduction
//*/
//void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, int*** &num_result, int64_t divide_number);
//
//
///*
//Decrypt the ciphertext to obtain a two-dimensional matrix and then narrow down the plaintext using division.
//Input:
//	Decryptor: decryption,
//	PolyCRTBuilder: decoding,
//	result_middle_image: ciphertext matrix to be decrypted, which is a one-dimensinal ciphertext matrix,
//	filename: file name to write the decrypted result,
//	tmp_result: a two-dimensional matrix that stores decrypted results,
//	divide_number: Degree of reduction
//*/
//void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, int** &tmp_results, int64_t divide_number);
//
///*
//Decrypt the ciphertext to obtain a three-dimensional matrix.
//Input:
//	Decryptor: decryption,
//	PolyCRTBuilder: decoding,
//	result_middle_image: ciphertext matrix to be decrypted, which is a two-dimensinal ciphertext matrix,
//	filename: file name to write the decrypted result,
//	num_result: a three-dimensional matrix that stores decrypted results.
//*/
//void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, int*** &num_result);
//
///*
//Decrypt the ciphertext to obtain a two-dimensional matrix.
//Input:
//	Decryptor: decryption,
//	PolyCRTBuilder: decoding,
//	result_middle_image: ciphertext matrix to be decrypted, which is a one-dimensinal ciphertext matrix,
//	filename: file name to write the decrypted result,
//	tmp_result: a two-dimensional matrix that stores decrypted results.
//*/
//void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, int** &tmp_results);
//
///*
//Decrypt the ciphertext to obtain a three-dimensional matrix and do ReLU operation.
//Input:
//	Decryptor: decryption,
//	PolyCRTBuilder: decoding,
//	result_middle_image: ciphertext matrix to be decrypted, which is a two-dimensinal ciphertext matrix,
//	filename: file name to write the decrypted result,
//	num_result: a three-dimensional matrix that stores decrypted results.
//*/
//void decrpt_middle_result_ReLU(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, int*** &num_result);
//
///*
//Decrypt the ciphertext to obtain a two-dimensional matrix  and do ReLU operation.
//Input:
//	Decryptor: decryption,
//	PolyCRTBuilder: decoding,
//	result_middle_image: ciphertext matrix to be decrypted, which is a one-dimensinal ciphertext matrix,
//	filename: file name to write the decrypted result,
//	tmp_result: a two-dimensional matrix that stores decrypted results.
//*/
//void decrpt_middle_result_ReLU(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, int** &tmp_results);