#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "header.h"

using namespace std;

void outmatrix(int l, int n, int m, double* A)
{
	int colmin = min(n, m);
	int rowmin = min(l, m);
	int i, j;
	for (i = 0; i < colmin; ++i)
	{
		for (j = 0; j < rowmin; ++j)
		{
			printf("%10.3e ", A[i*n+j]);
		}
		cout << endl;
	}
}

