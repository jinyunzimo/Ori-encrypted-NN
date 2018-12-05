#include "pch.h"
#include "identify.h"


void identify_num(vector<vector<int64_t>> result, vector<int64_t> target)
{
	// the file reacord the identificated number
	ofstream result_num_indentified;
	result_num_indentified.open("result_num_indentified.csv", ios::out);
	// the number of pictures which is identified correctly
	int num_right = 0;
	// Parallel the process of identification of numbers
	//#pragma omp parallel for shared(result, target, indentified_num)
	// notice: It can't be parallel, because I want to write the fstream file. In addition, num_right is a global variable, which is need a lock.
	for (int index_image = 0; index_image < result.size(); index_image++)
	{
		int j = 0;
		// record the index of the max one
		int index = j;
		// the max value
		int64_t max = result[index_image][j];
		for (j = 1; j < result[0].size(); j++)
		{
			if (result[index_image][j] > max)
			{
				max = result[index_image][j];
				index = j;
			}
		}
		// write the orginal target and the identified target to the file
		result_num_indentified << target[index_image] << "," << index << endl;
		// also write to the global file
		// outfile << target[index_image] << "    " << index << endl;
		// if the predict is correct
		if (target[index_image] == index)
			num_right++;
	}
	cout << "num_right: " << num_right << endl;
	// outfile << "num_right: " << num_right << endl;
	result_num_indentified.close();
}

void identify_num(vector<vector<mpz_class>> result, vector<int64_t> target)
{
	// the file reacord the identificated number
	ofstream result_num_indentified;
	result_num_indentified.open("result_num_indentified.csv", ios::out);
	// the number of pictures which is identified correctly
	int num_right = 0;
	// Parallel the process of identification of numbers
	//#pragma omp parallel for shared(result, target, indentified_num)
	// notice: It can't be parallel, because I want to write the fstream file. In addition, num_right is a global variable, which is need a lock.
	for (int index_image = 0; index_image < result.size(); index_image++)
	{
		int j = 0;
		// record the index of the max one
		int index = j;
		// the max value
		mpz_class max = result[index_image][j];
		for (j = 1; j < result[0].size(); j++)
		{
			if (result[index_image][j] > max)
			{
				max = result[index_image][j];
				index = j;
			}
		}
		// write the orginal target and the identified target to the file
		result_num_indentified << target[index_image] << "      " << index << endl;
		// also write to the global file
		//outfile << target[index_image] << "    " << index << endl;
		// if the predict is correct
		if (target[index_image] == index)
			num_right++;
	}
	cout << "num_right: " << num_right << endl;
	//outfile << "num_right: " << num_right << endl;
	result_num_indentified.close();
}