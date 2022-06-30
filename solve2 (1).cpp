#include <iostream>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdexcept>
#include <fstream>
#include <string>
#include "header.h"
#include <thread>

using namespace std;

void Transponse(double*, int);

void synchronize(int threads_count)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
    static int threads_in = 0;
    static int threads_out = 0;
    
    pthread_mutex_lock(&mutex);
    //заблокировали mutex для работы с threads_in и threads_out до сигнала
    //дальше идет только один процесс, остальные если есть ждут
    
    threads_in++;
    if (threads_in >= threads_count)
    {
        //текущий поток пришел последним
        threads_out = 0;
        pthread_cond_broadcast(&condvar_in);
        //подали остальным сигнал на разблокировку
    }
    else
    {
        while (threads_in < threads_count)
            pthread_cond_wait(&condvar_in, &mutex);
            //разблокировали mutex (следующий может выполнять)
            //и ждем сигнала разблокировки
    }
    //то же самое для подсчета количества вышедших из фукнции
    //(так как возможно использование в цикле)
    threads_out++;
    if (threads_out >= threads_count)
    {
        threads_in = 0;
        pthread_cond_broadcast(&condvar_out);
    }
    else
    {
        while (threads_out < threads_count)
            pthread_cond_wait(&condvar_out, &mutex);
    }
    
    pthread_mutex_unlock(&mutex);
    //все одновременно выходят
}


void Transponse(double* matrix, int n)
{
	int i, j;
	for (i = 0; i < n; ++i)
	{
		for (j = i+1; j < n; ++j)
		{
			swap(matrix[i*n+j], matrix[j*n+i]);
		}
	}
}


void inverse(double* matrix, double* inverse_matrix, int n,
                  int thread_num, int threads_count, int *return_flag)
{
    int i, j, k,z, b,g,u, begin_row, last_row, begin_cow, last_cow;
    double a, x;
    double f;
    double eps = norm_matrix(n, matrix)*1e-15;
    
    
    printf("Поток %d запущен\n", thread_num+1);
	
	if(thread_num == 0){Transponse(matrix, n);}
	
    for (i = 0; i < n; ++i)
    {
        if(thread_num==0){
        	
            a = matrix[i*n+i];
            b = i;

            for (j = i+1; j < n; ++j)
            {
                if (fabs(matrix[j*n+i]) > fabs(a))
                {
                    a = matrix[j*n+i];
                    b = j;
                }
            }

            if (fabs(a) < eps)
               *return_flag=0;

            if(*return_flag!=0){
                if (b != i)
                {
                    for (j = i; j < n; ++j)
                    {
                    	x = matrix[b*n+j];
                    	matrix[b*n+j] = matrix[i*n+j];
                    	matrix[i*n+j] = x;
                    }
                     for (j = 0; j < n; ++j)
                    {
                    	x = inverse_matrix[b*n+j];
                    	inverse_matrix[b*n+j] = inverse_matrix[i*n+j];
                    	inverse_matrix[i*n+j] = x;
                    }
                }

                a = 1.0/matrix[i*n+i];

                for (j = i; j < n; ++j)
                    {
                        matrix[i*n+j] *= a;
                    }
                  for (j = 0; j < n; ++j)
                   {inverse_matrix[i*n+j] *= a;}
            }
        }
        synchronize(threads_count);

        if(!*return_flag)
            return;

        begin_row = (n - i - 1) * thread_num;
        begin_row = begin_row/threads_count + i + 1;
        last_row = (n - i - 1) * (thread_num + 1);
        last_row = last_row/threads_count + i + 1;
	
	
        for (j =  begin_row; j <last_row ; ++j)
        {
            a = matrix[j*n+i];

            for (k = i; k < n; ++k)
                {
                    matrix[j*n+k] -= matrix[i*n+k] * a;
                }
             for (k = 0; k < n; ++k)
                {
                    inverse_matrix[j*n+k] -= inverse_matrix[i*n+k] * a;
                }
        }
    
        synchronize(threads_count);
    }
    
    begin_cow = n*thread_num/threads_count;
    last_cow = n*(thread_num + 1)/threads_count;

        for(k = begin_cow; k <last_cow ; ++k)
        {
            for(i = n-1; i >= 0; --i)
            
                {   a=inverse_matrix[i * n + k];
                    
                    for(j = i+1; j < n; ++j)
                    {
                        a -= inverse_matrix[j * n + k] * matrix[i * n + j];
                    }
                    
                    inverse_matrix[i * n + k] = a;
                }
        }

    synchronize(threads_count);

    if(thread_num == 0){Transponse(inverse_matrix, n);}
    
    return;
    
}
