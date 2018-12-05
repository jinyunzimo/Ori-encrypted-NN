#include "pch.h"
#include "nn_relu.h"
#include "nn_two_crt.h"
#include "conv1_net.h"
#include "nn_2conv.h"
#include "raw_inference.h"

using namespace std;
//using namespace seal;

// Ignore errors, this is about multi-thread
#pragma warning( disable : 4996 )

void test()
{
	//vector<int64_t> T(2);
	//T[0] = 1099511922689;
	//T[1] = 1099512004609;
	//print_banner("Set encryption parameters", '*');

	//EncryptionParameters parms1, parms2;
	//parms1.set_poly_modulus("1x^2048 + 1");
	//parms2.set_poly_modulus("1x^2048 + 1");
	//parms1.set_coeff_modulus(coeff_modulus_192(8192));
	//parms2.set_coeff_modulus(coeff_modulus_192(8192));
	//parms1.set_plain_modulus(T[0]);
	//parms2.set_plain_modulus(T[1]);
	//SEALContext context1(parms1);
	//SEALContext context2(parms2);
	//print_parameters(context1);
	//cout << endl;
	//print_parameters(context2);
	//print_banner("Finishing setting", '#');
	//cout << endl;

	////IntegerEncoder encoder1(context1.plain_modulus());
	////IntegerEncoder encoder2(context2.plain_modulus());

	//KeyGenerator keygen1(context1);
	//PublicKey public_key1 = keygen1.public_key();
	//SecretKey secret_key1 = keygen1.secret_key();

	//KeyGenerator keygen2(context2);
	//PublicKey public_key2 = keygen2.public_key();
	//SecretKey secret_key2 = keygen2.secret_key();

	//Encryptor encryptor1(context1, public_key1);
	//Encryptor encryptor2(context2, public_key2);

	//PolyCRTBuilder crtbuilder1(context1);
	//PolyCRTBuilder crtbuilder2(context2);

	//int slot_size = crtbuilder1.slot_count();

	//Evaluator evaluator1(context1);
	//Evaluator evaluator2(context2);


	//Decryptor decryptor1(context1, secret_key1);
	//Decryptor decryptor2(context2, secret_key2);

	//int64_t t = 1073741824;
	//vector<int64_t> ts;
	//for (int i = 0; i < slot_size; i++)
	//{
	//	ts.push_back(t + i);
	//}
	//cout << "the initial number is " << t << endl;


	//Plaintext p1;
	//crtbuilder1.compose(ts, p1);
	//Plaintext p2;
	//crtbuilder2.compose(ts, p2);

	//Ciphertext c1;
	//encryptor1.encrypt(p1, c1);

	//Ciphertext c2;
	//encryptor2.encrypt(p2, c2);

	//
	//evaluator1.square(c1);
	//evaluator2.square(c2);

	//Plaintext p3;
	//decryptor1.decrypt(c1, p3);
	//
	//Plaintext p4;
	//decryptor2.decrypt(c2, p4);

	//vector<int64_t> a, b;
	//crtbuilder1.decompose(p3, a);
	//crtbuilder2.decompose(p4, b);

	//for (int i = 0; i < 15; i++)
	//{
	//	cout << a[i] << "---------" << b[i] << endl;
	//	vector<int64_t> tmp;
	//	tmp.push_back(a[i]);
	//	tmp.push_back(b[i]);
	//	mpz_class ooout;
	//	ooout = mpz_class(ts[i] * (i + 1)) * mpz_class(ts[i] * (i + 1));
	//	cout << ooout << endl;
	//	cout << CRT(T, tmp) << endl << endl;
	//}

	/*vector<int64_t> q(2);
	q[0] = 13;
	q[1] = 17;
	vector<int64_t> w(2);
	w[0] = 9;
	w[1] = 1;
	cout << CRT(q, w) << endl << endl;

	vector<int64_t> m;
	m.push_back(1099511922689);
	m.push_back(1099512004609);
	vector<int64_t> a;
	a.push_back(1099512);
	a.push_back(1099512);
	cout << CRT(m, a) << endl<<endl;
	vector<int64_t> b;
	b.push_back(2);
	b.push_back(1);
	cout << CRT(m, b) << endl;*/
}

int main()
{
	//repeated_encryption_ReLU();
	//test_two_crt();
	//conv1_nn();
	//nn_2conv();
	inference();
	return 0;
}

