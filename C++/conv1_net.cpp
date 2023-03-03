#include "pch.h"
#include "conv1_net.h"


// Ignore errors, this is about multi-thread
#pragma warning( disable : 4996 )

void conv1_nn()
{
	ofstream outfile;
	time_t time_s = time(0);
	// generate the filename which contains date
	char log_file_name[64];
	strftime(log_file_name, sizeof(log_file_name), "result/ReLU_%Y%m%d_result.txt", localtime(&time_s));
	cout << log_file_name << endl;
	// open the outfile
	outfile.open(log_file_name, ios::app);

	// write the current time into the file
	char str_time[64];
	strftime(str_time, sizeof(str_time), "%Y%m%d %T", localtime(&time_s));
	outfile << str_time << endl;
	cout << str_time << endl;

	// output memory usage
	cout << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB\n" << endl;


	// read the images and labels of the test set
	print_banner("Read test images and labels", '*');
	vector<int64_t>labels;
	read_Mnist_Label("MINIST/t10k-labels.idx1-ubyte", labels);
	vector<vector<int64_t>> images;
	read_Mnist_Images("MINIST/t10k-images.idx3-ubyte", images);
	print_banner("Finish reading images and labels", '#');
	// end reading


	//read the model parameter
	print_banner("Reading model weights", '*');
	model_conv1_parameters model_para;
	model_para.set_parameters("weights_7");
	print_banner("Finish reading model weights", '#');
	// end reading the model parameter

	// time of encoding and encryption
	chrono::microseconds time_encoding_encryption(0);
	// time of inference
	chrono::microseconds time_inference(0);
	// time of decryption and decoding
	chrono::microseconds time_decryption_decoding(0);
	// time of act
	chrono::microseconds time_square(0);

	// set the encryption parameter
	print_banner("Set encryption parameters", '*');
	EncryptionParameters parms1;
	parms1.set_poly_modulus("1x^8192 + 1");
	parms1.set_coeff_modulus(coeff_modulus_192(8192));
	parms1.set_plain_modulus(1099511922689);
	SEALContext context1(parms1);
	int dbc = dbc_max();
	print_banner("Finishing setting", '#');
	// end setting

	// generate the secret and public keys
	print_banner("Generating secret/public keys/ev_keys: ", '*');
	KeyGenerator keygen(context1);
	auto public_key = keygen.public_key();
	auto secret_key = keygen.secret_key();
	EvaluationKeys ev_keys;
	keygen.generate_evaluation_keys(dbc, ev_keys);
	print_banner("Done", '#');
	// end generating

	// generate the decryptor and evaluator
	print_banner("Generating en(de)cryptor/evaluator...: ", '*');
	Encryptor encryptor(context1, public_key);
	Evaluator evaluator(context1);
	Decryptor decryptor(context1, secret_key);
	print_banner("Done", '#');
	// end generating

	// generate the poly-encoder
	print_banner("Generating poly_encoder...: ", '*');
	PolyCRTBuilder crtbuilder(context1);
	int slot_count = crtbuilder.slot_count();
	int row_size = slot_count / 2;
	cout << "Plaintext matrix row size: " << row_size << endl;
	print_banner("Done", '#');
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
	// end time
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Done", '#');
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


	// pooling
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

	// Minimize memory
	{
		vector<vector<Ciphertext>> tmp;
		result_conv1_image.clear();
		result_conv1_image.swap(tmp);
	}
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	print_banner("Finishing the first pool", '#');


	// flatten
	time_start = chrono::high_resolution_clock::now();
	print_banner("Flatten", '*');
	// the result of flatten
	vector<Ciphertext> flat_result_pool1;
	for (int j = 0; j < result_pool1_image[0].size(); j++)
	{
		for (int i = 0; i < result_pool1_image.size(); i++)
		{
			flat_result_pool1.push_back(result_pool1_image[i][j]);
		}
	}
	print_banner("end", '#');
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	cout << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;

	// the first fully connection layer
	print_banner("the first fc", '*');
	time_start = chrono::high_resolution_clock::now();
	// the result of the 1st fc
	vector<Ciphertext> result_fc1;
	weight_sum(crtbuilder, encryptor, evaluator, ev_keys, flat_result_pool1, result_fc1, model_para.fc1_w, model_para.fc1_b);
	// Minimize memory
	{
		vector<Ciphertext> ttmp;
		flat_result_pool1.clear();
		flat_result_pool1.swap(ttmp);
	}
	print_banner("Finishing FC1", '#');
	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_inference += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;

	print_banner("Decryption", '*');
	time_start = chrono::high_resolution_clock::now();
	// result of 2nd fc, shaped as [num_images, 10]
	vector<vector<int64_t>> fc2_middel_results;
	// decrypt the result
	decrpt_middle_result_no_ops(decryptor, crtbuilder, result_fc1, "enc_fc2.csv", fc2_middel_results);

	time_end = chrono::high_resolution_clock::now();
	time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
	time_decryption_decoding += time_diff;
	cout << "Done [" << time_diff.count() << " microseconds]" << endl;
	outfile << "Done [" << time_diff.count() << " microseconds]" << endl;
	print_banner("end", '#');

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

	print_banner("Identify the number:", '*');
	// Identify inferred numbers based on the results
	identify_num(fc2_middel_results, labels);

	cout << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	outfile << "\nTotal memory allocated by global memory pool: "
		<< (MemoryPoolHandle::Global().alloc_byte_count() >> 20) << " MB" << endl;
	// end_time
	time_t time_e = time(0);
	strftime(str_time, sizeof(str_time), "%Y%m%d %T", localtime(&time_e));
	outfile << str_time << endl;
	cout << str_time << endl;
	outfile.close();
}
