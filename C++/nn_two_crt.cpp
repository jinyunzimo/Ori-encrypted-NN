#include "pch.h"
#include "nn_two_crt.h"



// the file that records some intermediate information, including time and memory usage, and the file will open in the process function.
ofstream outfile;
// Ignore errors, this is about multi-thread
#pragma warning( disable : 4996 )

/*
Use the square activation function to complete a inference
@model_para: the model parameters of the trained neraul network model
@context: the encryption parameter
@images: input images
@return: the result of the inference
*/
vector<vector<int64_t>> process(model_parameters model_para, SEALContext context, vector<vector<int64_t>> images)
{
	// generate a random number, which will be contained in the middel result file
	int rand_int = rand() / 100;
	// output memory usage
	cout << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB\n" << endl;

	// time of encoding and encryption
	chrono::microseconds time_encoding_encryption(0);
	// time of inference
	chrono::microseconds time_inference(0);
	// time of decryption and decoding
	chrono::microseconds time_decryption_decoding(0);
	// time of square
	chrono::microseconds time_square(0);

	// generate the secret and public keys
	print_banner("Generating secret/public keys/ev_keys: ", '*');
	KeyGenerator keygen(context);
	auto public_key = keygen.public_key();
	auto secret_key = keygen.secret_key();
	EvaluationKeys ev_keys;
	int dbc = dbc_max();
	keygen.generate_evaluation_keys(dbc, ev_keys);
	print_banner("Done", '#');
	cout << endl;
	// end generating

	// generate the decryptor and evaluator
	print_banner("Generating en(de)cryptor/evaluator...: ", '*');
	Encryptor encryptor(context, public_key);
	Evaluator evaluator(context);
	Decryptor decryptor(context, secret_key);
	print_banner("Done", '#');
	cout << endl;
	// end generating

	// generate the poly-encoder
	print_banner("Generating poly_encoder...: ", '*');
	PolyCRTBuilder crtbuilder(context);
	int slot_count = crtbuilder.slot_count();
	int row_size = slot_count / 2;
	cout << "Plaintext matrix row size: " << row_size << endl;
	print_banner("Done", '#');
	cout << endl;
	// end generating

	// encrypt the images
	print_banner("Encrypt the images...: ", '*');
	// the test images which is shaped as [num_images, horizontally arranged pixels]
	vector<vector<int64_t>> test_images;

	// First import slot_size test images 
	for (int i = 0; i < crtbuilder.slot_count(); i++)
	{
		test_images.push_back(images[i]);
	}

	// the encoded images
	vector<Plaintext> images_encoded;
	// record start and end time
	chrono::high_resolution_clock::time_point time_start, time_end;
	// start_time
	time_start = chrono::high_resolution_clock::now();
	cout << "crtencoding..." << endl;
	// CRT encoding
	crtbuild_encode(crtbuilder, test_images, images_encoded);
	// the encrypted images
	vector<Ciphertext> images_encrypted;
	cout << "encrypting" << endl;
	// encrypting
	encrypt_images(encryptor, images_encoded, images_encrypted);
	// end time
	time_end = chrono::high_resolution_clock::now();
	// record the interval between encoding and encryption(us)
	auto time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);

	time_encoding_encryption += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	print_banner("Finish encryption!", '#');
	cout << endl;
	// end CRT encoding and encryption

	// the first convolution layer
	print_banner("The first conv:", '*');
	// 1st convolution layer start time
	time_start = chrono::high_resolution_clock::now();

	// Record the result of the first layer of convolution
	vector<vector<Ciphertext>> result_conv1_image;

	// Generate a single-channel image, ie [num_channel, num_images, length]
	// Convolution input
	vector<vector<Ciphertext>> in_images;
	in_images.push_back(images_encrypted);
	// kernel step size
	int kernel_stride[2] = { 2,2 };
	// calculate
	conv_valid(crtbuilder, encryptor, evaluator, ev_keys, in_images, result_conv1_image, model_para.conv1_w, kernel_stride, model_para.conv1_b, decryptor);
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Done", '#');
	cout << endl;
	// end 1st conv

	// activiating
	print_banner("activating...", '*');
	time_start = chrono::high_resolution_clock::now();
	square(evaluator, ev_keys, result_conv1_image);

	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_square += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Finishing the first act", '#');
	cout << endl;
	// end 1st act

	//vector<vector<vector<int64_t>>> middle_result_act1;
	//decrpt_middle_result_no_ops(decryptor, crtbuilder, result_conv1_image, "square1_" + to_string(rand_int) + ".csv", middle_result_act1);
	//middle_result_act1.clear();

	/// pooling
	// pooling size
	vector<int> ksize;
	ksize.push_back(1);
	ksize.push_back(3);
	ksize.push_back(3);
	ksize.push_back(1);
	// step size
	vector<int> stride;
	stride.push_back(1);
	stride.push_back(1);
	stride.push_back(1);
	stride.push_back(1);

	print_banner("pooling", '*');
	time_start = chrono::high_resolution_clock::now();
	vector<vector<Ciphertext>> result_pool1_image;

	// sum_pool
	sum_pool_valid(evaluator, ksize, stride, result_conv1_image, result_pool1_image);

	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	// Minimize memory
	{
		vector<vector<Ciphertext>> tmp;
		result_conv1_image.clear();
		result_conv1_image.swap(tmp);
	}
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Finishing the first pool", '#');
	cout << endl;


	//vector<vector<vector<int64_t>>> middle_result_pool1;
	//decrpt_middle_result(decryptor, crtbuilder, result_pool1_image, "pool1_" + to_string(rand_int) + ".csv", middle_result_pool1, 1000);
	///*middle_result_pool1.clear();
	//{
	//	vector<vector<Ciphertext>> tmp;
	//	result_pool1_image.clear();
	//	result_pool1_image.swap(tmp);
	//}*/

	//vector<vector<Ciphertext>> new_pool1_images;
	//re_encrypt(crtbuilder, encryptor, middle_result_pool1, new_pool1_images);
	/*print_banner("Encrypt the result of layer1");
	vector<vector<Ciphertext>> enc_layer1_image;
	time_start = chrono::high_resolution_clock::now();
	re_encrypt(crtbuilder, encryptor, layer1_middel_results, enc_layer1_image);

	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	print_finish_banner("Finish encryption!");*/

	// the second convolution
	print_banner("the second conv...", '*');
	time_start = chrono::high_resolution_clock::now();
	// record the result of 2nd conv
	vector<vector<Ciphertext>> result_conv2_image;
	// 2nd conv
	conv_valid(crtbuilder, encryptor, evaluator, ev_keys, result_pool1_image, result_conv2_image, model_para.conv2_w, kernel_stride, model_para.conv2_b, decryptor);
	/*conv_valid(crtbuilder, encryptor, evaluator, ev_keys, new_pool1_images, result_conv2_image, model_para.conv2_w, kernel_stride, model_para.conv2_b, decryptor);*/
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	{
		vector<vector<Ciphertext>> tmp;
		result_pool1_image.clear();
		result_pool1_image.swap(tmp);
	}
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Finishing the second conv", '#');
	cout << endl;

	/*vector<vector<vector<int64_t>>> middle_result_conv2;
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_conv2_image, "conv2_" + to_string(rand_int) + ".csv", middle_result_act1);
	middle_result_conv2.clear();*/

	// activating
	/*print_banner("activating", '*');
	time_start = chrono::high_resolution_clock::now();
	square(evaluator, ev_keys, result_conv2_image);

	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_square += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Finishing the second square", '#');
	cout << endl;*/

	/*vector<vector<vector<int64_t>>> middle_result_act2;
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_conv2_image, "square2_" + to_string(rand_int) + ".csv", middle_result_act1);
	middle_result_act2.clear();*/

	// pooling
	print_banner("pooling", '*');
	time_start = chrono::high_resolution_clock::now();
	vector<vector<Ciphertext>> result_pool2_image;
	sum_pool_valid(evaluator, ksize, stride, result_conv2_image, result_pool2_image);

	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);

	{
		vector<vector<Ciphertext>> tmp;
		result_conv2_image.clear();
		result_conv2_image.swap(tmp);
	}
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Finishing the second pool", '#');
	cout << endl;

	/*vector<vector<vector<int64_t>>> middle_result_pool2;
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_pool2_image, "square2_" + to_string(rand_int) + ".csv", middle_result_pool2);
	middle_result_pool2.clear();*/


	/*vector<vector<vector<int64_t>>> layer2_middel_results;
	decrpt_middle_result(decryptor, crtbuilder, result_pool2_image, "enc_layer2.csv", layer2_middel_results, (int64_t)1000);

	vector<vector<Ciphertext>> new_pool2_image;
	re_encrypt(crtbuilder, encryptor, layer2_middel_results, new_pool2_image);*/


	//{
	//	vector<vector<Ciphertext>> tmp;
	//	result_pool2_image.clear();
	//	result_pool2_image.swap(tmp);
	//}



	/*print_banner("Encrypt the result of layer2");
	vector<vector<Ciphertext>> enc_layer2_image;
	time_start = chrono::high_resolution_clock::now();
	re_encrypt(crtbuilder, encryptor, layer2_middel_results, enc_layer2_image);

	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	print_finish_banner("Finish encryption!");*/


	// flatten
	time_start = chrono::high_resolution_clock::now();
	print_banner("Flatten", '*');
	// the result of flatten
	vector<Ciphertext> flat_result_pool2;
	for (int i = 0; i < result_pool2_image.size(); i++)
	{
		for (int j = 0; j < result_pool2_image[0].size(); j++)
		{
			flat_result_pool2.push_back(result_pool2_image[i][j]);
		}
	}
	{
		vector<vector<Ciphertext>> tmp;
		result_pool2_image.clear();
		result_pool2_image.swap(tmp);
	}
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;

	print_banner("end", '#');
	cout << endl;

	// the first fully connection layer
	print_banner("the first fc", '*');
	time_start = chrono::high_resolution_clock::now();
	// the result of the 1st fc
	vector<Ciphertext> result_fc1;
	weight_sum(crtbuilder, encryptor, evaluator, ev_keys, flat_result_pool2, result_fc1, model_para.fc1_w, model_para.fc1_b);
	time_end = chrono::high_resolution_clock::now();
	{
		vector<Ciphertext> ttmp;
		flat_result_pool2.clear();
		flat_result_pool2.swap(ttmp);
	}
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	print_banner("Finishing FC1", '#');
	cout << endl;

	/*vector<vector<int64_t>> fc1_middel_results;
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_fc1, "enc_fc1" + to_string(rand_int) + ".csv", fc1_middel_results);
	fc1_middel_results.clear();*/

	// activiating
	cout << "activating" << endl;
	time_start = chrono::high_resolution_clock::now();
	square(evaluator, ev_keys, result_fc1);
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_square += time_diff;

	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Finish the first FC", '#');
	cout << endl;


	vector<vector<int64_t>> sq3_middel_results;
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_fc1, "enc_sq3" + to_string(rand_int) + ".csv", sq3_middel_results);
	sq3_middel_results.clear();


	//print_banner("Encrypt the result of layer3");
	//time_start = chrono::high_resolution_clock::now();
	//vector<Plaintext> images_encoded_layer3;
	//crtbuild_encode(crtbuilder, sq3_middel_results, images_encoded_layer3);


	//vector<Ciphertext> images_encrypted_layer3;
	//encrypt_images(encryptor, images_encoded_layer3, images_encrypted_layer3);

	//time_end = chrono::high_resolution_clock::now();
	//time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	//cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	//outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	//print_finish_banner("Finish encryption!");



	// the second fully connection layer
	print_banner("the second fc", '*');
	time_start = chrono::high_resolution_clock::now();
	// the result of 2nd fc
	vector<Ciphertext> result_fc2;
	weight_sum(crtbuilder, encryptor, evaluator, ev_keys, result_fc1, result_fc2, model_para.fc2_w, model_para.fc2_b);
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Total memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("finishing the second fc", '#');
	cout << endl;

	print_banner("Decryption", '*');
	time_start = chrono::high_resolution_clock::now();
	// result of 2nd fc, shaped as [num_images, 10]
	vector<vector<int64_t>> fc2_middel_results;
	// decrypt the result
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_fc2, "enc_fc2" + to_string(rand_int) + ".csv", fc2_middel_results);
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_decryption_decoding += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	print_banner("end decryption", '#');
	cout << endl;

	print_banner("summarize:", '*');
	// output time spent in each stage
	cout << "encoding and encryption [" << time_encoding_encryption.count() << " microseconds]" << endl;
	cout << "inference [" << time_inference.count() << " microseconds]" << endl;
	cout << "decryption and decoding [" << time_decryption_decoding.count() << " microseconds]" << endl;
	cout << "relu [" << time_square.count() << " microseconds]" << endl;

	outfile << "encoding and encryption [" << time_encoding_encryption.count() << " microseconds]" << endl;
	outfile << "inference [" << time_inference.count() << " microseconds]" << endl;
	outfile << "decryption and decoding [" << time_decryption_decoding.count() << " microseconds]" << endl;
	outfile << "relu [" << time_square.count() << " microseconds]" << endl;
	print_banner("finishing summarizing", '#');
	cout << endl;
	return fc2_middel_results;
}

/*
Experiment with two sets of parameters, and finally calculate the original value with CRT code.
*/
void test_two_crt()
{
	// current time
	time_t time_start = time(0);
	// generate the filename which contains date
	char log_file_name[64];
	strftime(log_file_name, sizeof(log_file_name), "result/two_crt_%Y%m%d_result.txt", localtime(&time_start));
	cout << log_file_name << endl;
	// open the outfile
	outfile.open(log_file_name, ios::app);

	// write the current time into the file
	char str_time[64];
	strftime(str_time, sizeof(str_time), "%Y%m%d %T", localtime(&time_start));
	outfile << str_time << endl << endl;
	cout << str_time << endl << endl;
	// read the images and labels of the test set
	print_banner("Read test images and labels", '*');
	vector<int64_t>labels;
	read_Mnist_Label("t10k-labels.idx1-ubyte", labels);
	vector<vector<int64_t>> images;
	read_Mnist_Images("t10k-images.idx3-ubyte", images);
	print_banner("Finish reading images and labels", '#');
	cout << endl;
	// end reading

	//read the model parameter
	print_banner("Reading model weights", '*');
	model_parameters model_para;
	model_para.set_parameters("weights_5");
	print_banner("Finish reading model weights", '#');
	cout << endl;
	// end

	// Set two sets of encryption parameters
	vector<int64_t> T(2);
	T[0] = 1099511922689;
	T[1] = 1099512004609;
	print_banner("Set encryption parameters", '*');

	EncryptionParameters parms1, parms2;
	parms1.set_poly_modulus("1x^1024 + 1");
	parms2.set_poly_modulus("1x^1024 + 1");
	parms1.set_coeff_modulus(coeff_modulus_192(8192));
	parms2.set_coeff_modulus(coeff_modulus_192(8192));
	parms1.set_plain_modulus(T[0]);
	parms2.set_plain_modulus(T[1]);
	SEALContext context1(parms1);
	SEALContext context2(parms2);
	print_parameters(context1);
	cout << endl;
	print_parameters(context2);
	print_banner("Finishing setting", '#');
	cout << endl;
	// end

	// Calculate the results using two sets of parameters
	print_banner("the first inference", '*');
	vector<vector<int64_t>> result1 = process(model_para, context1, images);
	print_banner("finishing the first inference", '#');

	print_banner("the second inference", '*');
	vector<vector<int64_t>> result2 = process(model_para, context2, images);
	print_banner("finishing the second inference", '#');

	// Restore the result with CRT

	//file of result of 1st parameter, result of 2nd parameter, reuslt of crt 
	ofstream file_result1, file_result2, file_result;
	file_result1.open("number_result1.txt", ios::app);
	file_result2.open("number_result2.txt", ios::app);
	file_result.open("number_result.txt", ios::app);
	// result of crt
	//vector<vector<int64_t>> result;
	vector<vector<mpz_class>> result;
	for (int i = 0; i < result1.size(); i++)
	{
		//vector<int64_t> tmp;
		vector<mpz_class> tmp;
		for (int j = 0; j < result1[0].size(); j++)
		{
			vector<int64_t> mod_num;
			mod_num.push_back(result1[i][j]);
			mod_num.push_back(result2[i][j]);
			tmp.push_back(CRT(T, mod_num));
			file_result1 << result1[i][j] << ",";
			file_result2 << result2[i][j] << ",";
			file_result << tmp[j] << ",";
		}
		result.push_back(tmp);
		file_result1 << "\n";
		file_result2 << "\n";
		file_result << "\n";
	}
	file_result1.close();
	file_result2.close();
	file_result.close();

	// 计算预测准确率
	print_banner("test", '*');
	identify_num(result, labels);
	cout << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile.close();
	time_t time_end = time(0);
	strftime(str_time, sizeof(str_time), "%Y%m%d %T", localtime(&time_end));
	outfile << str_time << endl << endl;
	cout << str_time << endl << endl;
}
