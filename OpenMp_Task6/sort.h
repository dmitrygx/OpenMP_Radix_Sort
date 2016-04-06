#ifndef SORT_H
#define SORT_H

#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <stack>
#include <cmath>
#include "numeric.h"
#include "util.h"

#ifndef CHUNK 
#define CHUNK 2
#endif

#define NUM_VAL 2

using namespace std;

double* OmpRadixSortMSD(const double* array, const uint len, uint radix);

#endif
