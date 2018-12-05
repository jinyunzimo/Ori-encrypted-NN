#include "pch.h"
#include "images_encryptor.h"


void crtbuild_encode(PolyCRTBuilder& crtbuider, vector<vector<int64_t>>&images, vector<Plaintext>& encoded_images)
{
	int num_images = images.size();
	int num_rows = images[0].size();
	// For each pixel in the position, there are num_images numbers, which make up a vector, encoding
	for (int i = 0; i < num_rows; i++)
	{
		vector<int64_t> new_row;
		for (int j = 0; j < num_images; j++)
		{
			new_row.push_back(images[j][i]);
		}
		Plaintext tmp_plain;
		crtbuider.compose(new_row, tmp_plain);
		encoded_images.push_back(tmp_plain);
	}
}

void encrypt_images(Encryptor& encryptor, vector<Plaintext>& encoded_images, vector<Ciphertext>& encrypted_images)
{
	int num_rows = encoded_images.size();
	for (int i = 0; i < num_rows; i++)
	{
		Ciphertext tmp_cipher;
		encryptor.encrypt(encoded_images[i], tmp_cipher);
		encrypted_images.push_back(tmp_cipher);
	}
}

void crtbuild_decode(PolyCRTBuilder& crtbuider, vector<Plaintext>& encoded_array, vector<vector<int64_t>>& result)
{
	int num = encoded_array.size();
	for (int i = 0; i < num; i++)
	{
		vector<int64_t> tmp;
		crtbuider.decompose(encoded_array[i], tmp);
		result.push_back(tmp);
	}
}
void decrypt_images(Decryptor& decryptor, vector<Ciphertext>& encrypted_array, vector<Plaintext>& encoded_array)
{
	int num = encrypted_array.size();
	for (int i = 0; i < num; i++)
	{
		encoded_array.push_back(Plaintext());
		decryptor.decrypt(encrypted_array[i], encoded_array[i]);
		Plaintext tmp;
		decryptor.decrypt(encrypted_array[i], tmp);
	}
}

void re_encrypt(PolyCRTBuilder& crtbuider, Encryptor& encryptor, vector<vector<vector<int64_t>>> middle_image, vector<vector<Ciphertext>>& encrypted_images)
{
	for (int index_channel = 0; index_channel < middle_image.size(); index_channel++)
	{
		vector<Plaintext> image_encoded;
		crtbuild_encode(crtbuider, middle_image[index_channel], image_encoded);
		vector<Ciphertext> image_encrypted;
		encrypt_images(encryptor, image_encoded, image_encrypted);
		encrypted_images.push_back(image_encrypted);
	}
}

void re_encrypt(PolyCRTBuilder& crtbuider, Encryptor& encryptor, vector<vector<int64_t>> middle_image, vector<Ciphertext>& encrypted_images) 
{
	vector<Plaintext> image_encoded;
	crtbuild_encode(crtbuider, middle_image, image_encoded);
	encrypt_images(encryptor, image_encoded, encrypted_images);
}

void decrypt_re_encrypt(Decryptor & decryptor, PolyCRTBuilder & crtbuider, Encryptor & encryptor, vector<vector<Ciphertext>>& encrypted_images)
{
	vector<vector<vector<int64_t>>> num_result;
	decrpt_middle_result(decryptor, crtbuider, encrypted_images, "test.csv", num_result, 1000);
	encrypted_images.clear();
	re_encrypt(crtbuider, encryptor, num_result, encrypted_images);
}

void decrypt_re_encrypt(Decryptor& decryptor, PolyCRTBuilder& crtbuider, Encryptor& encryptor, vector<Ciphertext>& encrypted_images) 
{
	vector<vector<int64_t>> num_result;
	decrpt_middle_result(decryptor, crtbuider, encrypted_images, "test.csv", num_result, 1000);
	encrypted_images.clear();
	re_encrypt(crtbuider, encryptor, num_result, encrypted_images);
}
