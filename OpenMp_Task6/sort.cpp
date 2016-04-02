#include "sort.h"

double* OmpRadixSortMSDStack(stack<double> st, uint radix)
{
	double* result;
	stack<double> stack[NUM_VAL];
	if (st.empty())
	{
		return NULL;
	}
	else
	{
		result = OmpGetMemoryPool()->OmpAlloc(st.size());
		if (64 == radix)
		{
			uint counter = 0;
			while (!st.empty())
			{
				result[counter] = st.top();
				//cout << "res[" << counter << "] = " << result[counter] << endl;
				//cout << "radix = " << radix << " count = " << count << endl;
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
		stack[OmpGetBit(value, radix)].push(value);
	}
	uint counter = 0;
	uint j = 0;
	//#pragma omp parallel shared(counter, stack) private(j)
	{
		//#pragma omp for schedule(dynamic, CHUNK) nowait
		for (int i = NUM_VAL - 1; i >= 0; --i)
		{
			uint stSize = stack[i].size();
			double* res = OmpRadixSortMSDStack(stack[i], radix + 1);
			if (NULL != res)
			{
				for (j = 0; j < stack[i].size(); j++)
				{
					result[counter] = res[j];
					counter++;
				}

			}
			OmpGetMemoryPool()->OmpFree(stSize);
		}
	}
	return result;
}

double* OmpRadixSortMSD(const double* array, const uint len, uint radix)
{
	double* result = OmpGetMemoryPool()->OmpAlloc(len);
	stack<double> stack[NUM_VAL];

	double val = 0;
	//create_lock(mutex);
	//init_lock(&mutex);
#pragma omp parallel shared(stack/*, mutex*/) private(val)
	{
#pragma omp for schedule(dynamic, CHUNK) nowait
		for (int i = 0; i < len; ++i)
		{
			//lock(&mutex);
			{
#pragma omp critical
				{
					val = array[i];
					stack[OmpGetBit(val, radix)].push(val);
				}
			}
			//unlock(&mutex);
		}
	}
	//destroy_lock(&mutex);
	uint counter = 0;
	int j = 0;
#pragma omp parallel shared(counter, stack) private(j)
	{
#pragma omp for schedule(dynamic, CHUNK)
		for (int i = 0; i < NUM_VAL; ++i)
		{
			uint stSize = stack[i].size();
			double* res = OmpRadixSortMSDStack(stack[i], radix + 1);
			if (NULL != res)
			{
				//#pragma omp parallel shared(counter,  stack) private(j)
				{
					//#pragma omp for schedule(dynamic, CHUNK) nowait
					if (0 == i)
					{	
						for (j = 0; j < (int)stack[i].size(); j++)
						{
							result[counter] = res[j];
							counter++;
						}
					}
					else
					{
						for (j = (int)stack[i].size() - 1; j >= 0; --j)
						{
							result[counter] = res[j];
							counter++;
						}
					}
				}
				OmpGetMemoryPool()->OmpFree(stSize);
			}
		}
	}
	return result;
}

