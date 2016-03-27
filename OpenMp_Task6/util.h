#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <omp.h>
#include "numeric.h"

using namespace std;

#define uint unsigned long long int

class OmpPool
{
private:
  double* pool;
  uint size;
  double* current;
  uint currentSize;
public:
  OmpPool(uint sizeOfMem = 0)
  {
    size = sizeOfMem;
    currentSize = 0;
    OmpMemoryPoolAlloc(size);
  }
  ~OmpPool()
  {
    size = 0;
    currentSize = 0;
  }
  void OmpMemoryPoolAlloc(uint len);
  void OmpMemoryPollFree();
  double* OmpAlloc(uint len);
};

void OmpOutput(bool out, const char* file, const char* text, double* array, uint len);

void OmpParseArgs(int argc, char** argv, double &min, double &max, uint &num, uint &precision);

void OmpInitMemoryPool(uint len);

void OmpTerminateMemoryPool(void);

OmpPool *OmpGetMemoryPool(void);

#ifdef openmp
#define lock(a) omp_set_lock(a)
#else
#define lock(a)
#endif

#ifdef openmp
#define unlock(a) omp_unset_lock(a)
#else
#define unlock(a)
#endif

#ifdef openmp
#define init_lock(a) omp_init_lock(a)
#else
#define init_lock(a)
#endif

#ifdef openmp
#define destroy_lock(a) omp_destroy_lock(a)
#else
#define destroy_lock(a)
#endif

#ifdef openmp
#define create_lock(a) omp_lock_t a
#else
#define create_lock(a) omp_lock_t a;		\
  (void) a
#endif

#endif
