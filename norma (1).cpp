#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "header.h"

using namespace std;

double norm_matrix(int n, double* Prod)
{
	int i, j;
	double sum, result = 0;
	for (i = 0; i < n; ++i)
	{
		sum = 0;
		for (j = 0; j < n; ++j)
		{
			sum += abs(Prod[i*n+j]);
		}
		if (sum > result)
		{
			result = sum;
		}
	}
	return result;
}

double norma(int n, double* A, double* X, double* Prod)
{
	double result, sum = 0;
	
	
	int i, j, h;
	
	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < n; ++j)
		{
			Prod[i*n+j] = 0;
		}
	}
	
	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < n; ++j)
		{
			for (h = 0; h < n; ++h)
			{
				Prod[i*n+j] += A[i*n+h]*X[h*n+j];
			}
		}
		Prod[i*n+i] -= 1;
	}
	
	
	return norm_matrix(n, Prod);
}
