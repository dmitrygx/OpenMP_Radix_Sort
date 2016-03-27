#include "sort.h"

static uint OmpGetCountOfDigitsInteger(int integer)
{
  int count = (integer == 0) ? 1 : 0;

  while (integer != 0) 
  {
    count++;
    integer /= 10;
  }

  return count;
}

static uint OmpGetCountOfDigits(double number, int &integ, double &fract)
{
  int count = (number == 0.0) ? 1 : 0;
  double fraction, integer;
  int integerInt = 0;
  fraction = modf(number, &integer);
  //cout << "fract = " << fraction << " " << "integer = " << integer << endl;
  integerInt = (int) integer;
  
  integ = (int) integer;
  fract = fraction;

  count = OmpGetCountOfDigitsInteger(integerInt);
  
  return count;
}

static uint OmpGetMaxCountOfDigits(const double* numbers, uint len, int* integ, double* fract)
{
  uint max = 0;

#pragma omp parallel for schedule(dynamic, CHUNK)
  for (int i = 0; i < len; ++i)
  {
    uint current = OmpGetCountOfDigits(numbers[i], integ[0], fract[0]);
    if (current > max)
    {
      max = current;
    }
  }
  
  return max;
}

static double* OmpRadixSortMSDStackDouble(uint count, stack<double> st, uint precision, uint radix)
{
  uint len = st.size();
  double* result = OmpGetMemoryPool()->OmpAlloc(len);
  stack<double> stack[NUM_VAL];

  if (st.empty())
  {
    return NULL;
  }
  else
  {
    if (precision < radix)
    {
      uint counter = 0;

      while (!st.empty())
      {
	result[counter] = st.top();
	st.pop();
	counter++;
      }
      return result;
    }
  }

  while (!st.empty())
  {
    double value = st.top();
    st.pop();
    stack[(int)(value * pow(10, radix)) % 10].push(value);
  }
  uint counter = 0;
  uint j = 0;
//#pragma omp parallel shared(counter, stack) private(j)
  {
//#pragma omp for schedule(dynamic, CHUNK) nowait
    for (int i = NUM_VAL - 1; i >= 0; i--)
    {
      double* res = OmpRadixSortMSDStackDouble(count, stack[i], precision, radix + 1);
      if (NULL != res)
      {
	for(j = 0; j < stack[i].size(); j++)
	{
	  result[counter] = res[j];
	  counter++;
	}
      }
    }
  }
  return result;
}

static double* OmpRadixSortMSDStack(uint count, stack<double> st, uint precision, uint radix)
{
  uint len = st.size();
  double* result = OmpGetMemoryPool()->OmpAlloc(len);
  stack<double> stack[NUM_VAL];

  if (st.empty())
  {
    return NULL;
  }
  else
  {
    if (radix - 1  == count)
    {
      /*uint counter = 0;
      while (!st.empty())
      {
	result[counter] = st.top();
	//cout << "res[" << counter << "] = " << result[counter] << endl;
	cout << "radix = " << radix << " count = " << count << endl;
	st.pop();
	counter++;
	}*/
      result = OmpRadixSortMSDStackDouble(count, st, precision, 1);
      return result;
    }
  }

  while (!st.empty())
  {
    double value = st.top();
    st.pop();
    stack[(int)(value/pow(10, count-radix)) - (int)(value/pow(10, count-(radix-1))) * 10].push(value);
  }
  uint counter = 0;
  uint j = 0;
  //#pragma omp parallel shared(counter, stack) private(j)
  {
  //#pragma omp for schedule(dynamic, CHUNK) nowait
    for (int i = NUM_VAL - 1; i >= 0; i--)
    {
      double* res = OmpRadixSortMSDStack(count, stack[i], precision, radix + 1);
      if (NULL != res)
      {
	for(j = 0; j < stack[i].size(); j++)
	{
	  result[counter] = res[j];
	  counter++;
	}
      }
    }
  }
  return result;
}

double* OmpRadixSortMSD(const double* array, const uint len, uint precision, uint radix)
{
  double* result = OmpGetMemoryPool()->OmpAlloc(len);

  stack<double> stackNeg[NUM_VAL];
  stack<double> stack[NUM_VAL];

  int integ[1];
  double fract[1];
  uint count = OmpGetMaxCountOfDigits(array, len, integ, fract);
  double val = 0;

  //create_lock(mutex);
  //init_lock(&mutex);
#pragma omp parallel shared(stack, stackNeg/*, mutex*/) private(val)
  {
#pragma omp for schedule(dynamic, CHUNK) nowait
    for (int i = 0; i < len; ++i)
    {
      //lock(&mutex);
      val = array[i];
      if (val >= 0.0)
      {
#pragma omp critical
	{
	  stack[(int)(val/pow(10, count-radix))].push(val);
	}
      }
      else
      {
	val = val * (-1);
#pragma omp critical
	{
	  stackNeg[(int)(val/pow(10, count-radix))].push(val);
	}
      }
      //unlock(&mutex);
    }
  }
  //destroy_lock(&mutex);
  uint counter = 0;
  int j = 0;
#pragma omp parallel shared(counter,  stack) private(j)
  {
#pragma omp for schedule(dynamic, CHUNK) nowait
    for (int i = NUM_VAL - 1; i >= 0; --i)
    {
      double* res = OmpRadixSortMSDStack(count, stack[i], precision, radix + 1);
      if (NULL != res)
      {
//#pragma omp parallel shared(counter,  stack) private(j)
	{
//#pragma omp for schedule(dynamic, CHUNK) nowait
	  for(j = 0; j < stack[i].size(); j++)
	  {
	    result[counter] = res[j];
	    counter++;
	  }
	}
      }
    }  
  }
#pragma omp parallel shared(stackNeg) private(j)
  {
#pragma omp for schedule(dynamic, CHUNK) nowait 
    for (int i = 0; i < NUM_VAL; ++i)
    {
      double* res = OmpRadixSortMSDStack(count, stackNeg[i], precision, radix + 1);
      if (NULL != res)
      {
//#pragma omp parallel shared(counter, stackNeg) private(j)
	{
//#pragma omp for schedule(dynamic, CHUNK) nowait 
	  for(j = stackNeg[i].size() - 1; j >= 1; --j)
      {
	    result[counter] = res[j] * (-1);
	    counter++;
	  }
	}
	result[counter] = res[0] * (-1);
	counter++;
      }
    }
  }
  return result;
}

