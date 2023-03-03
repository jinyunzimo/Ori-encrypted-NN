#include <random>
#include <chrono>
#include "seal/randomgen.h"

using namespace std;

template <int s>
class MyRandom : public mt19937
{
public:
	MyRandom():mt19937(s){}
};

namespace seal
{
	//mt19937 mt(0);
	UniformRandomGeneratorFactory *UniformRandomGeneratorFactory::default_factory_ = new StandardRandomAdapterFactory<MyRandom<14>>();

    //UniformRandomGeneratorFactory *UniformRandomGeneratorFactory::default_factory_ = new StandardRandomAdapterFactory<random_device>();
}
