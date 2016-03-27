#include "util.h"

static OmpPool *Pool;

OmpPool *OmpGetMemoryPool(void)
{
  return Pool;
}

/**
 * Print @array and @text to output
 * @param out       True - print to @file, False - print to stdout 
 * @param file      The file that specified to output @array (If NULL default file will be choice)      
 * @param text      The text that will be printed in top of @file or stdout      
 * @param array     The pointer to array that will be prented   
 * @param len       The length of @array
 * @return None
 */
void OmpOutput(bool out, const char* file, const char* text, double* array, uint len)
{
  if (out == true)
  {
    ofstream fout;
    if (file != NULL)
    {
      fout.open(file);
    }
    else
    {
      fout.open("default.txt");
    }
    fout << text << endl;
    for (uint i = 0; i < len; i++)
    {
      fout << "array[" << i << "] = " << array[i] << endl;
    }
    fout.close();
  }
  else
  {
    cout << text << endl;
    for (uint i = 0; i < len; i++)
    {
      cout << "array[" << i << "] = " << array[i] << endl;  
    }
  }

}

/**
 * Parse command line arguments and initialize value of @min, @max and @num
 * @param argc      The number of CLI arguments 
 * @param argv      The CLI arguments
 * @param min       The reference to "min" value
 * @param max       The reference to "max" value
 * @param num       The reference to "num" value
 * @param precision The reference to "precision" value 
 * @return None
 */
void OmpParseArgs(int argc, char** argv, double &min, double &max, uint &num, uint &precision)
{
  bool excess = false;
  min = 0.0;
  max = 100.0;
  num = 10;
  precision = 3;

  for (int i = 1; i < argc; ++i)
  {
    switch(i)
    {
    case 1: 
      {
	min = atoi(argv[i]);
	break;
      }
    case 2:
      {
	max = atoi(argv[i]);
	break;
      }
    case 3:
      {
	num = atoi(argv[i]);
	break;
      }
    case 4:
      {
	precision = atoi(argv[i]);
	break;
      }
    default:
      {
	excess = true;
	break;
      }
    }
    if (excess)
    {
      break;
    }
  }

  return;
}

void OmpPool::OmpMemoryPoolAlloc(size_t len)
{
  pool = new double[len];
}

void OmpPool::OmpMemoryPollFree()
{
  delete [] pool;
}

double* OmpPool::OmpAlloc(size_t len)
{
  if (currentSize + len < size)
  {
    current = pool + currentSize;
    currentSize += len;
    return current;
  }
  else
  {
    cout << "Allocation error" << endl;
    exit(0);
  }
}

/*void OmpPool::OmpFree(size_t len)
{
  current = current - len;
  currentSize -= len;
}
*/
void OmpInitMemoryPool(size_t len)
{
  Pool = new OmpPool(len);
}

void OmpTerminateMemoryPool(void)
{
  Pool->OmpMemoryPollFree();
  delete Pool;
}
