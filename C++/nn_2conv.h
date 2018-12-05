#pragma once
#include <fstream>
#include <iostream>
#include <vector>

#include "read_weights.h"
#include "seal\seal.h"
#include "print_all.h"
#include "images_encryptor.h"
#include "decrypt_middle_result.h"
#include "ops.h"
#include "exgcd.h"
#include "identify.h"

using namespace std;
using namespace seal;

void nn_2conv();
