#pragma once
#include <vector>
#include <time.h>
#include <iostream>
#include <fstream>

#include "seal/seal.h"
#include "read_weights.h"
#include "print_all.h"
#include "image_reader.h"
#include "images_encryptor.h"
#include "ops.h"
#include "decrypt_middle_result.h"
#include "identify.h"


using namespace std;
using namespace seal;
/*
Use the square activation function to complete a inference
@model_para: the model parameters of the trained neraul network model
@context: the encryption parameter
@images: input images
@return: the result of the inference
*/
void conv1_nn();
