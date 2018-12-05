#pragma once
#include <iostream>
#include <vector>
//#include "bignumber.h"
#include <mpirxx.h>

using namespace std;

/*
Find out the inverse element of b about a
*/
//int64_t exgcd_find_reverse(int64_t a, int64_t b);
mpz_class exgcd_find_reverse(mpz_class a, mpz_class b);

/*
Restore the original number according to the large prime numbers m and the mod numbers a
*/
//int64_t CRT(vector<int64_t> m, vector<int64_t> a);
mpz_class CRT(vector<int64_t> m, vector<int64_t> a);

//int64_t mod(int64_t a, int64_t b);
mpz_class mod(mpz_class a, mpz_class b);