#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>


#include "seal/seal.h"
#include "decrypt_middle_result.h"
#include "image_reader.h"

using namespace std;
using namespace seal;

/*
Use CRT to encode input data. When using CRTBuilder, the corresponding pixels of each picture are encoded together into a polynomial.
Input: 
	crtbuider:
	images: input data, shaped as [num_images, horizontally arranged pixels], e.g.[1024, 784]
	encoded_images: record the encoded data, shaped as [num_pixels], e.g.[784]
*/
void crtbuild_encode(PolyCRTBuilder& crtbuider, vector<vector<int64_t>>& images, vector<Plaintext>& encoded_images);

/*
Encrypt the polynomial of CRT encoding. 1-dimention
Input:
	encryptor:
	encoded_images: 
	encoded_images:
*/
void encrypt_images(Encryptor& encryptor, vector<Plaintext>& encoded_images, vector<Ciphertext>& encrypted_images);

/*
Use CRT to decode the polynomials. The inverse of crtbuild_encode.
Input:
	crtbuider:
	encoded_images: record the encoded data, shaped as [num_pixels], e.g.[784]
	images: input data, shaped as [num_images, horizontally arranged pixels], e.g.[1024, 784], where crtbuilder.slot_size=1024
*/
void crtbuild_decode(PolyCRTBuilder& crtbuider, vector<Plaintext>& encoded_images, vector<vector<int64_t>>& images);

/*
Decrypt the input ciphertext.
Input:
	decryptor:
	encrypted_images:
	encoded_images:
*/
void decrypt_images(Decryptor& decryptor, vector<Ciphertext>& encrypted_images, vector<Plaintext>& encoded_images);

/*
Re-encrypt the array.
Input:
	crtbuider:
	encryptor:
	middle_image: the array to be re-encrypted, shaped as [num_images, num_channel, pixels]
	encrypted_images: the array of ciphertext, shaped as [num_channel, pixels]
*/
void re_encrypt(PolyCRTBuilder& crtbuider, Encryptor& encryptor, vector<vector<vector<int64_t>>> middle_image, vector<vector<Ciphertext>>& encrypted_images);

void re_encrypt(PolyCRTBuilder& crtbuider, Encryptor& encryptor, vector<vector<int64_t>> middle_image, vector<Ciphertext>& encrypted_images);

void decrypt_re_encrypt(Decryptor& decryptor, PolyCRTBuilder& crtbuider, Encryptor& encryptor, vector<vector<Ciphertext>>& encrypted_images);

void decrypt_re_encrypt(Decryptor& decryptor, PolyCRTBuilder& crtbuider, Encryptor& encryptor, vector<Ciphertext>& encrypted_images);