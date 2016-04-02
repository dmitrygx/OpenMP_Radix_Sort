#include "main.h"
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
	double min, max;
	uint num, precision;

	(void)OmpParseArgs(argc, argv, min, max, num, precision);
	OmpInitMemoryPool(num * 64);

	double* array = OmpNumRandomGenerate(min, max, num);
	double* result;
	if (NULL == array)
	{
		cout << "Random array hasn't been obtained" << endl;
		return -1;
	}

	OmpOutput(true, "omp_array.txt", "Source array:", array, num, precision);
	time_t start = clock();
	result = OmpRadixSortMSD(array, num, 0);
	//result = NULL;
	//double a = -7.8;
	//for (int i = 31; i >= 0; --i)
	//{
	//	printf("%d", GETBIT(*(((unsigned int*)&a) + 1), i));
	//}
	//printf("\n");
	//for (int i = 63; i > 31; --i)
	//{
	//	printf("%d", GETBIT(*(unsigned int*)&a, i));
	//}
	//printf("\n");

	//for (int i = 0; i < 64; ++i)
	//	printf("%d", OmpGetBit(a, i));

	std::cout << std::endl;
	time_t end = clock();
	time_t diff = end - start;
	cout << "Time: " << ((double)diff) / CLOCKS_PER_SEC << " sec." << endl;
	if (NULL == result)
	{
		cout << "Array hasn't been sorted" << endl;
		return -1;
	}
	OmpOutput(true, "omp_result.txt", "Result of array sorting:", result, num, precision);

	OmpTerminateMemoryPool();

	return 0;
}
