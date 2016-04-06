#include "sort.h"

double* OmpRadixSortMSDStack(stack<double> st, uint radix)
{
	double* result;
	stack<double> stack[NUM_VAL];
	uint thr1 = 0;
	uint thr0 = 0;
	if (st.empty())
	{
		return NULL;
	}
	else
	{
		result = OmpGetMemoryPool()->OmpAlloc(st.size());
		if ((64 == radix) || (st.size() == 1))
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
		(OmpGetBit(value, radix)) == 0 ? thr0++ : thr1++;
		stack[OmpGetBit(value, radix)].push(value);
	}
	//cout << thr0 << "    " << thr1 << endl;
	uint counter = 0;
	uint counter1 = 0;
	uint counter2 = 0;
	int i;
	double* res1;
	double* res2;
#pragma omp parallel shared(stack) num_threads(2)
	{
#pragma omp sections
		{
			//cout << " " << omp_get_num_threads() << endl;
#pragma omp section
			{
				i = 1;
				res1 = OmpRadixSortMSDStack(stack[i], radix + 1);
				if (NULL != res1)
				{
					for (int j = 0; j < stack[i].size(); j++)
					{
						result[counter] = res1[j];
						counter++;
					}
				}
			}
#pragma omp section
			{
				int i1 = 0;
				res2 = OmpRadixSortMSDStack(stack[i1], radix + 1);
				if (NULL != res2)
				{
					for (int j = 0; j < stack[i1].size(); j++)
					{
						result[thr1 + counter1] = res2[j];
						counter1++;
					}
				}
			}
		}
	}
	OmpGetMemoryPool()->OmpFree(thr1, res1);
	OmpGetMemoryPool()->OmpFree(thr0, res2);
	return result;
}

double* OmpRadixSortMSD(const double* array, const uint len, uint radix)
{
	double* result;
//#pragma omp critical
	{
		result = OmpGetMemoryPool()->OmpAlloc(len);
	}
	stack<double> stack[NUM_VAL];
	uint thr1 = 0;
	uint thr0 = 0;
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
					(OmpGetBit(val, radix)) == 0 ? thr0++ : thr1++;
					stack[OmpGetBit(val, radix)].push(val);
				}
			}
			//unlock(&mutex);
		}
	}
	//destroy_lock(&mutex);
	uint counter1 = 0;
	uint counter2 = 0;
	int j = 0;
	double* res1;
	double* res2;

#pragma omp parallel shared(stack) private(j) num_threads(2)
	{
#pragma omp sections
		{
			//cout << " " << omp_get_num_threads() << endl;
#pragma omp section
			{
				res1 = OmpRadixSortMSDStack(stack[0], radix + 1);
				if (NULL != res1)
				{
					//#pragma omp parallel shared(counter,  stack) private(j)
					{
						//#pragma omp for schedule(dynamic, CHUNK) nowait
						for (j = 0; j < (int)thr0; j++)
						{
							result[counter1] = res1[j];
							counter1++;
						}
					}
//#pragma omp critical
					OmpGetMemoryPool()->OmpFree(thr0, res1);
				}
			}
		//
#pragma omp section
			{
				res2 = OmpRadixSortMSDStack(stack[1], radix + 1);
				if (NULL != res2)
				{
					//#pragma omp parallel shared(counter,  stack) private(j)
					{
						//#pragma omp for schedule(dynamic, CHUNK) nowait
						{
							for (j = (int)thr1 - 1; j >= 0; --j)
							{
								result[thr0 + counter2] = res2[j];
								counter2++;
							}
						}
					}
//#pragma omp critical
					OmpGetMemoryPool()->OmpFree(thr1, res2);
				}
			}
		}
	}
	return result;
}

