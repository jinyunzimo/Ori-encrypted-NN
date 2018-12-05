#include "pch.h"

#include "exgcd.h"

mpz_class mod(mpz_class a, mpz_class b) {
	if (a < 0) {
		mpz_class A = -a;
		a += A / b * b;
		if (a == 0)return 0;
		else return a + b;
	}
	return a - a / b * b;
}
//int64_t mod(int64_t a, int64_t b) {
//	if (a < 0) {
//		int64_t A = -a;
//		a += A / b * b;
//		if (a == 0)return 0;
//		else return a + b;
//	}
//	return a - a / b * b;
//}

//int64_t exgcd_find_reverse(int64_t a, int64_t b)
//{
//	int64_t x0 = 1, y0 = 0, x1 = 0, y1 = 1, tempx, tempy;
//	int64_t q, r;
//	int64_t m = a, n = b;
//	while (m%n != 0)
//	{
//		q = m / n;
//		tempx = x1;
//		tempy = y1;
//		x1 = x0 - q * x1;
//		y1 = y0 - q * y1;
//		x0 = tempx, y0 = tempy;
//		r = m % n;
//		m = n;
//		n = r;
//	}
//	if (n != 1)
//	{
//		cout << "There is no reverse number!" << endl;
//		return 0;
//	}
//	return mod(y1, a);
//}

mpz_class exgcd_find_reverse(mpz_class a, mpz_class b)
{
	mpz_class x0 = 1, y0 = 0, x1 = 0, y1 = 1, tempx, tempy;
	mpz_class q, r;
	mpz_class m = a, n = b;
	while (m%n != 0)
	{
		q = m / n;
		tempx = x1;
		tempy = y1;
		x1 = x0 - q * x1;
		y1 = y0 - q * y1;
		x0 = tempx, y0 = tempy;
		r = m % n;
		m = n;
		n = r;
	}
	if (n != 1)
	{
		cout << "There is no reverse number!" << endl;
		return 0;
	}
	//cout << y1 << endl;
	return mod(y1, a);
}

mpz_class CRT(vector<int64_t> m, vector<int64_t> a)
{
	if (m.size() != a.size())
	{
		throw "The parameter length does not match the given data length.";		
	}
	vector<mpz_class> m_bn;
	vector<mpz_class> a_bn;
	for (int i = 0; i < m.size(); i++)
	{
		m_bn.push_back(mpz_class(m[i]));
		a_bn.push_back(mpz_class(a[i]));
	}
	mpz_class M_bn(1);
	vector<mpz_class> Mi_bn;
	vector<mpz_class> inverse_Mi_bn;
	mpz_class result_bn(0);
	for (int i = 0; i < m.size(); i++)
	{
		M_bn *= m_bn[i];
	}
	for (int i = 0; i < m.size(); i++)
	{
		Mi_bn.push_back(M_bn / m_bn[i]);
		//cout << Mi_bn[i] << endl;
		inverse_Mi_bn.push_back(exgcd_find_reverse(m_bn[i], Mi_bn[i]));
		//cout << inverse_Mi_bn[i] << endl;
		result_bn += a[i] * inverse_Mi_bn[i] * Mi_bn[i];
		//cout << result_bn << endl;
	}
	result_bn = result_bn % M_bn;
	return result_bn;

	/*int64_t M = 1;
	vector<int64_t> Mi;
	vector<int64_t> inverse_Mi;
	int64_t result = 0;
	for (int i = 0; i < m.size(); i++)
	{
		M *= m[i];
	}
	for (int i = 0; i < m.size(); i++)
	{
		Mi.push_back(M / m[i]);
		inverse_Mi.push_back(exgcd_find_reverse(m[i], Mi[i]));
		result += a[i] * inverse_Mi[i] * Mi[i];
	}
	result = result % M;
	return result;*/
}

//int64_t CRT(vector<int64_t> m, vector<int64_t> a)
//{
//	if (m.size() != a.size())
//	{
//		throw "The parameter length does not match the given data length.";
//	}
//	
//	int64_t M = 1;
//	vector<int64_t> Mi;
//	vector<int64_t> inverse_Mi;
//	int64_t result = 0;
//	for (int i = 0; i < m.size(); i++)
//	{
//		M *= m[i];
//	}
//	for (int i = 0; i < m.size(); i++)
//	{
//		Mi.push_back(M / m[i]);
//		inverse_Mi.push_back(exgcd_find_reverse(m[i], Mi[i]));
//		result += a[i] * inverse_Mi[i] * Mi[i];
//	}
//	result = result % M;
//	return result;
//}
