#include "pch.h"
#include "nn_2conv.h"


// Ignore errors, this is about multi-thread
#pragma warning( disable : 4996 )

void nn_2conv()
{
	// generate a random number, which will be contained in the middel result file
	srand((int)time(0));
	int rand_int = rand() / 100;
	// the file that records some intermediate information, including time and memory usage, and the file will open in the process function.
	ofstream outfile;
	// current time
	time_t a_time_start = time(0);
	// generate the filename which contains date
	char log_file_name[64];
	strftime(log_file_name, sizeof(log_file_name), "result/two_crt_%Y%m%d_result.txt", localtime(&a_time_start));
	cout << log_file_name << endl;
	// open the outfile
	outfile.open(log_file_name, ios::app);

	// write the current time into the file
	char str_time[64];
	strftime(str_time, sizeof(str_time), "%Y%m%d %T", localtime(&a_time_start));
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

	// Set the encryption parameters
	print_banner("Set encryption parameters", '*');

	EncryptionParameters parms1;
	parms1.set_poly_modulus("1x^2048 + 1");
	parms1.set_coeff_modulus(coeff_modulus_192(8192));
	parms1.set_plain_modulus(1099512004609);
	// parms1.set_plain_modulus(10000000999464961);
	SEALContext context(parms1);
	print_parameters(context);
	cout << endl;
	print_banner("Finishing setting", '#');
	cout << endl;
	// end

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

	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_conv1_image, "conv1_" + to_string(rand_int) + ".csv");

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
	// decrypt_re_encrypt(decryptor, crtbuilder, encryptor, result_conv1_image);
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_conv1_image, "act1_" + to_string(rand_int) + ".csv");

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

	//decrpt_middle_result_no_ops(decryptor, crtbuilder, result_pool1_image, "raw_pool1_" + to_string(rand_int) + ".csv");
	//decrypt_re_encrypt(decryptor, crtbuilder, encryptor, result_pool1_image);
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_pool1_image, "pool1_" + to_string(rand_int) + ".csv");

	cout << "Noise budget in fresh encryption: "
		<< decryptor.invariant_noise_budget(result_pool1_image[0][0]) << " bits" << endl;

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

	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_conv2_image, "conv2_" + to_string(rand_int) + ".csv");


	cout << "Noise budget in fresh encryption: "
		<< decryptor.invariant_noise_budget(result_conv2_image[0][0]) << " bits" << endl;

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

	//decrpt_middle_result_no_ops(decryptor, crtbuilder, result_pool2_image, "raw_pool2_" + to_string(rand_int) + ".csv");
	//decrypt_re_encrypt(decryptor, crtbuilder, encryptor, result_pool2_image);
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_pool2_image, "pool2_" + to_string(rand_int) + ".csv");

	// flatten
	time_start = chrono::high_resolution_clock::now();
	print_banner("Flatten", '*');
	// the result of flatten
	vector<Ciphertext> flat_result_pool2;
	for (int i = 0; i < result_pool2_image[0].size(); i++)
	{
		for (int j = 0; j < result_pool2_image.size(); j++)
		{
			flat_result_pool2.push_back(result_pool2_image[j][i]);
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

	//decrpt_middle_result_no_ops(decryptor, crtbuilder, flat_result_pool2, "raw_flat_" + to_string(rand_int) + ".csv");
	//decrypt_re_encrypt(decryptor, crtbuilder, encryptor, flat_result_pool2);
	decrpt_middle_result_no_ops(decryptor, crtbuilder, flat_result_pool2, "flat_" + to_string(rand_int) + ".csv");

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

	//decrypt_re_encrypt(decryptor, crtbuilder, encryptor, result_fc1);
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_fc1, "fc1_" + to_string(rand_int) + ".csv");

	cout << "Noise budget in fresh encryption: "
		<< decryptor.invariant_noise_budget(result_fc1[0]) << " bits" << endl;

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

	//decrpt_middle_result_no_ops(decryptor, crtbuilder, result_fc1, "raw_sq2_" + to_string(rand_int) + ".csv");
	//decrypt_re_encrypt(decryptor, crtbuilder, encryptor, result_fc1);
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_fc1, "sq2_" + to_string(rand_int) + ".csv");

	cout << "Noise budget in fresh encryption: "
		<< decryptor.invariant_noise_budget(result_fc1[0]) << " bits" << endl;

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

	cout << "Noise budget in fresh encryption: "
		<< decryptor.invariant_noise_budget(result_fc2[0]) << " bits" << endl;


	print_banner("Decryption", '*');
	time_start = chrono::high_resolution_clock::now();
	// result of 2nd fc, shaped as [num_images, 10]
	vector<vector<int64_t>> fc2_middel_results;
	// decrypt the result
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_fc2, "fc2_" + to_string(rand_int) + ".csv", fc2_middel_results);
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


	// Restore the result with CRT

	//file of result of 1st parameter, result of 2nd parameter, reuslt of crt 
	
	// 计算预测准确率
	print_banner("test", '*');
	identify_num(fc2_middel_results, labels);
	cout << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile.close();
	time_t a_time_end = time(0);
	strftime(str_time, sizeof(str_time), "%Y%m%d %T", localtime(&a_time_end));
	outfile << str_time << endl << endl;
	cout << str_time << endl << endl;
}
