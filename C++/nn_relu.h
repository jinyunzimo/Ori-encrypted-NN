#pragma once
#include <time.h>
#include <iostream>
#include <fstream>


#include "print_all.h"
#include "image_reader.h"
#include "read_weights.h"
#include "images_encryptor.h"
#include "ops.h"
#include "decrypt_middle_result.h"
#include "identify.h"
#include "seal\seal.h"

using namespace std;
using namespace seal;
void repeated_encryption_ReLU();