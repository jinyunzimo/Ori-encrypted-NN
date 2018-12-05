#include "pch.h"
#include "print_all.h"


/*
print_banner, which is used as separators.
@title: text to be printed
@symbol: the filling symbol
*/

void print_banner(string title, const char symbol)
{
	// get the title's length
	int length = title.size();
	if (length % 2 != 0)
		length += 1;
	// calculate the length of two side
	int side = (60 - length) / 2;
	if (!title.empty())
	{
		// construct a banner_middle contain title
		string banner_middle = string(side, symbol) + " " + title + " " + string(side, symbol);
		// print the banner
		cout << banner_middle << endl;
		// Write the outfile at the same time
		// outfile << banner_middle << endl;
	}
}

/*
Print the matrix which is shaped as print_size*print_size
*/
void print_matrix(const vector<int64_t> &matrix, int print_size)
{
	cout << endl;
	cout << "[";
	for (int i = 0; i < print_size; i++)
	{
		for (int j = 0; j < print_size; j++)
		{
			cout << setw(5) << matrix[i * print_size + j] << ",";
		}
		cout << endl;
	}
	cout << "]" << endl;
}


void print_parameters(const SEALContext &context)
{
	cout << "/ Encryption parameters:" << endl;
	cout << "| poly_modulus: " << context.poly_modulus().to_string() << endl;

	/*
	Print the size of the true (product) coefficient modulus
	*/
	cout << "| coeff_modulus size: "
		<< context.total_coeff_modulus().significant_bit_count() << " bits" << endl;

	cout << "| plain_modulus: " << context.plain_modulus().value() << endl;
	cout << "\\ noise_standard_deviation: " << context.noise_standard_deviation() << endl;
	cout << endl;
}