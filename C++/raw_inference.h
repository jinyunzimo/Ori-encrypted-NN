#pragma once

#include <string>
#include <chrono>
#include <time.h>
#include "raw_ops.h"
#include "print_all.h"
#include "read_weights.h"
#include "image_reader.h"
#include "identify.h"

using namespace std;

void write_to_csv(string filename, vector<vector<int64_t>> num_result);

void write_to_csv(string filename, vector<int64_t> num_result);

void inference();