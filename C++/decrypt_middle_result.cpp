#include "pch.h"
#include "decrypt_middle_result.h"



void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, vector<vector<vector<int64_t>>>& num_result)
{
	// Decrypt intermediate calculation results
	// Note: The decoding part has been changed so that the output form after decoding is [channel, image, pixel]
	for (int i = 0; i < result_middle_image.size(); i++)
	{
		vector<Plaintext> encoded_images;
		decrypt_images(decryptor, result_middle_image[i], encoded_images);
		vector<vector<int64_t>> test_results;
		// Used to convert the format ([pixels, number of pictures] -> [number of pictures, pixels])
		vector<vector<int64_t>> tmp_results;
		crtbuild_decode(crtbuilder, encoded_images, test_results);
		for (int j = 0; j < crtbuilder.slot_count(); j++)
		{
			vector<int64_t> pixels;
			for (int k = 0; k < test_results.size(); k++)
			{
				pixels.push_back(int64_t(log(test_results[k][j])));
			}
			tmp_results.push_back(pixels);
		}
		num_result.push_back(tmp_results);
	}
	ofstream middle_file;
	int row_num = sqrt(num_result[0][0].size());
	middle_file.open(filename, ios::out);
	for (int index_image = 0; index_image < 10; index_image++)
	{
		for (int index_channel = 0; index_channel < result_middle_image.size(); index_channel++)
		{
			for (int index_pixel = 0; index_pixel < num_result[0][0].size(); index_pixel++)
			{
				/*cout << "testeste3" << endl;*/
				middle_file << num_result[index_channel][index_image][index_pixel];
				if ((index_pixel + 1) % row_num == 0)
				{
					middle_file << "\n";
				}
				else
				{
					middle_file << ",";
				}
			}
		}
	}
	middle_file.close();
}

void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, vector<vector<int64_t>>& tmp_results)
{
	vector<Plaintext> encoded_images;
	decrypt_images(decryptor, result_middle_image, encoded_images);
	vector<vector<int64_t>> test_results;
	crtbuild_decode(crtbuilder, encoded_images, test_results);
	for (int j = 0; j < crtbuilder.slot_count(); j++)
	{
		vector<int64_t> pixels;
		for (int k = 0; k < test_results.size(); k++)
		{
			pixels.push_back(int64_t(log(test_results[k][j])));
		}
		tmp_results.push_back(pixels);
	}

	ofstream middle_file;
	middle_file.open(filename, ios::out);
	for (int index_image = 0; index_image < 10; index_image++)
	{
		for (int i = 0; i < tmp_results[0].size(); i++)
		{
			middle_file << tmp_results[index_image][i] << ",";
		}
		middle_file << "\n";
	}
	middle_file.close();
}

void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, vector<vector<vector<int64_t>>>& num_result, int64_t divide_number)
{
	for (int i = 0; i < result_middle_image.size(); i++)
	{
		vector<Plaintext> encoded_images;
		decrypt_images(decryptor, result_middle_image[i], encoded_images);
		vector<vector<int64_t>> test_results;
		vector<vector<int64_t>> tmp_results;
		crtbuild_decode(crtbuilder, encoded_images, test_results);
		for (int j = 0; j < crtbuilder.slot_count(); j++)
		{
			vector<int64_t> pixels;
			for (int k = 0; k < test_results.size(); k++)
			{
				pixels.push_back(test_results[k][j] / divide_number);
			}
			tmp_results.push_back(pixels);
		}
		num_result.push_back(tmp_results);
	}
	ofstream middle_file;
	int row_num = sqrt(num_result[0][0].size());
	middle_file.open(filename, ios::out);
	for (int index_image = 0; index_image < 10; index_image++)
	{
		for (int index_channel = 0; index_channel < result_middle_image.size(); index_channel++)
		{
			for (int index_pixel = 0; index_pixel < num_result[0][0].size(); index_pixel++)
			{
				/*cout << "testeste3" << endl;*/
				middle_file << num_result[index_channel][index_image][index_pixel];
				if ((index_pixel + 1) % row_num == 0)
				{
					middle_file << "\n";
				}
				else
				{
					middle_file << ",";
				}
			}
		}
	}
	middle_file.close();
}

void decrpt_middle_result(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, vector<vector<int64_t>>& tmp_results, int64_t divide_number)
{
	vector<Plaintext> encoded_images;
	decrypt_images(decryptor, result_middle_image, encoded_images);
	vector<vector<int64_t>> test_results;
	crtbuild_decode(crtbuilder, encoded_images, test_results);
	for (int j = 0; j < crtbuilder.slot_count(); j++)
	{
		vector<int64_t> pixels;
		for (int k = 0; k < test_results.size(); k++)
		{
			pixels.push_back(test_results[k][j] / divide_number);
		}
		tmp_results.push_back(pixels);
	}

	ofstream middle_file;
	middle_file.open(filename, ios::out);
	for (int index_image = 0; index_image < 10; index_image++)
	{
		for (int i = 0; i < tmp_results[0].size(); i++)
		{
			middle_file << tmp_results[index_image][i] << ",";
		}
		middle_file << "\n";
	}
	middle_file.close();
}

void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, vector<vector<vector<int64_t>>>& num_result)
{
	for (int i = 0; i < result_middle_image.size(); i++)
	{
		vector<Plaintext> encoded_images;
		decrypt_images(decryptor, result_middle_image[i], encoded_images);
		vector<vector<int64_t>> test_results;
		vector<vector<int64_t>> tmp_results; 
		crtbuild_decode(crtbuilder, encoded_images, test_results);
		for (int j = 0; j < crtbuilder.slot_count(); j++)
		{
			vector<int64_t> pixels;
			for (int k = 0; k < test_results.size(); k++)
			{
				pixels.push_back(test_results[k][j]);
			}
			tmp_results.push_back(pixels);
		}
		num_result.push_back(tmp_results);
	}
	ofstream middle_file;
	int row_num = sqrt(num_result[0][0].size());
	middle_file.open(filename, ios::out);
	for (int index_image = 0; index_image < 10; index_image++)
	{
		for (int index_channel = 0; index_channel < result_middle_image.size(); index_channel++)
		{
			for (int index_pixel = 0; index_pixel < num_result[0][0].size(); index_pixel++)
			{
				/*cout << "testeste3" << endl;*/
				middle_file << num_result[index_channel][index_image][index_pixel];
				if ((index_pixel + 1) % row_num == 0)
				{
					middle_file << "\n";
				}
				else
				{
					middle_file << ",";
				}
			}
		}
	}
	middle_file.close();
}

void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename)
{
	vector<vector<vector<int64_t>>> num_result;
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_middle_image, filename, num_result);
}

void decrpt_middle_result_no_ops(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, vector<vector<int64_t>>& tmp_results)
{
	vector<Plaintext> encoded_images;
	decrypt_images(decryptor, result_middle_image, encoded_images);
	vector<vector<int64_t>> test_results;
	crtbuild_decode(crtbuilder, encoded_images, test_results);
	for (int j = 0; j < crtbuilder.slot_count(); j++)
	{
		vector<int64_t> pixels;
		for (int k = 0; k < test_results.size(); k++)
		{
			pixels.push_back(test_results[k][j]);
		}
		tmp_results.push_back(pixels);
	}

	ofstream middle_file;
	middle_file.open(filename, ios::out);
	for (int index_image = 0; index_image < 10; index_image++)
	{
		for (int i = 0; i < tmp_results[0].size(); i++)
		{
			middle_file << tmp_results[index_image][i] << ",";
		}
		middle_file << "\n";
	}
	middle_file.close();
}

void decrpt_middle_result_no_ops(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<Ciphertext>& result_middle_image, string filename)
{
	vector<vector<int64_t>> tmp_results;
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_middle_image, filename, tmp_results);
}

void decrpt_middle_result_ReLU(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, vector<vector<vector<int64_t>>>& num_result)
{
	for (int i = 0; i < result_middle_image.size(); i++)
	{
		vector<Plaintext> encoded_images;
		decrypt_images(decryptor, result_middle_image[i], encoded_images);
		vector<vector<int64_t>> test_results;
		vector<vector<int64_t>> tmp_results;
		crtbuild_decode(crtbuilder, encoded_images, test_results);
		for (int j = 0; j < crtbuilder.slot_count(); j++)
		{
			vector<int64_t> pixels;
			for (int k = 0; k < test_results.size(); k++)
			{
				if (test_results[k][j] >= 0)
					pixels.push_back(test_results[k][j]);
				else
					pixels.push_back(int64_t(0));
			}
			tmp_results.push_back(pixels);
		}
		num_result.push_back(tmp_results);
	}
	ofstream middle_file;
	int row_num = sqrt(num_result[0][0].size());
	middle_file.open(filename, ios::out);
	for (int index_image = 0; index_image < 10; index_image++)
	{
		for (int index_channel = 0; index_channel < result_middle_image.size(); index_channel++)
		{
			for (int index_pixel = 0; index_pixel < num_result[0][0].size(); index_pixel++)
			{
				/*cout << "testeste3" << endl;*/
				middle_file << num_result[index_channel][index_image][index_pixel];
				if ((index_pixel + 1) % row_num == 0)
				{
					middle_file << "\n";
				}
				else
				{
					middle_file << ",";
				}
			}
		}
	}
	middle_file.close();
}


void decrpt_middle_result_ReLU(Decryptor& decryptor, PolyCRTBuilder& crtbuilder, vector<Ciphertext>& result_middle_image, string filename, vector<vector<int64_t>>& tmp_results)
{
	vector<Plaintext> encoded_images;
	decrypt_images(decryptor, result_middle_image, encoded_images);
	vector<vector<int64_t>> test_results;
	crtbuild_decode(crtbuilder, encoded_images, test_results);
	for (int j = 0; j < crtbuilder.slot_count(); j++)
	{
		vector<int64_t> pixels;
		for (int k = 0; k < test_results.size(); k++)
		{
			if (test_results[k][j] >= 0)
				pixels.push_back(test_results[k][j] / 1000);
			else
				pixels.push_back(int64_t(0));
		}
		tmp_results.push_back(pixels);
	}

	ofstream middle_file;
	middle_file.open(filename, ios::out);
	for (int index_image = 0; index_image < 10; index_image++)
	{
		for (int i = 0; i < tmp_results[0].size(); i++)
		{
			middle_file << tmp_results[index_image][i] << ",";
		}
		middle_file << "\n";
	}
	middle_file.close();
}

//void decrpt_middle_result(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, int ***& num_result)
//{
//}
//
//void decrpt_middle_result(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<Ciphertext>& result_middle_image, string filename, int **& tmp_results)
//{
//}
//
//void decrpt_middle_result(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, int ***& num_result, int64_t divide_number)
//{
//}
//
//void decrpt_middle_result(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<Ciphertext>& result_middle_image, string filename, int **& tmp_results, int64_t divide_number)
//{
//}
//
//void decrpt_middle_result_no_ops(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, int ***& num_result)
//{
//}
//
//void decrpt_middle_result_no_ops(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<Ciphertext>& result_middle_image, string filename, int **& tmp_results)
//{
//}
//
//void decrpt_middle_result_ReLU(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<vector<Ciphertext>>& result_middle_image, string filename, int ***& num_result)
//{
//}
//
//void decrpt_middle_result_ReLU(Decryptor & decryptor, PolyCRTBuilder & crtbuilder, vector<Ciphertext>& result_middle_image, string filename, int **& tmp_results)
//{
//}
