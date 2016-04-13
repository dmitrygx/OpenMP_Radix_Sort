#include "main.h"
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
	double min, max;
	uint num, precision;

	(void)OmpParseArgs(argc, argv, min, max, num, precision);
	OmpInitMemoryPool(num * 64);

	omp_set_nested(true);

	double* array = OmpNumRandomGenerate(min, max, num);
	double* result;
	if (NULL == array)
	{
		cout << "Random array hasn't been obtained" << endl;
		return -1;
	}

	OmpOutput(true, "omp_array.txt", "Source array:", array, num, precision);
	time_t start = clock();
	result = OmpMSDRadixSort(array, num, 0, num);
	time_t end = clock();
	time_t diff = end - start;
	cout << "Time: " << ((double)diff) / CLOCKS_PER_SEC << " sec." << endl;
	if (NULL == result)
	{
		cout << "Array hasn't been sorted" << endl;
		return -1;
	}
	OmpOutput(true, "omp_result.txt", "Result of array sorting:", result, num, precision);
	OmpGetMemoryPool()->OmpFree(num, result);
	OmpTerminateMemoryPool();

	return 0;
}
