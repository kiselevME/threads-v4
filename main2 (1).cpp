#include <iostream>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <stdexcept>
#include <fstream>
#include <string>
#include "header.h"
#include <thread>

using namespace std;


typedef struct
{
    double *matrix;
    double *inverse_matrix;
    int n;
    int thread_num;
    int threads_count;
    int *return_flag;
    
} Args;

void *Inverse(void *Arg)
{
    Args *arg = (Args*)Arg;
	long double total_time;
	
	synchronize(arg->threads_count);
	total_time = get_full_time();

    inverse(arg->matrix, arg->inverse_matrix, arg->n, arg->thread_num,
                 arg->threads_count, arg->return_flag);
	
	total_time = get_full_time() - total_time;
	synchronize(arg->threads_count);  
	printf("время работы алгоритма %d thread: %Lf sec.\n", arg->thread_num, total_time);  

    return NULL;
    
}

/*int main1(int argn, char **args_);

int main(int argn, char **args_)
{
	while(1)
	main1(argn, args_);
}
*/

int main(int argn, char **args_) 
{
    if (argn < 5) // Проверки на некорректный ввод
    {
    	cout << "Некорректные входные данные";
    	return -1;
    }

    if ((argn == 5)&&!(args_[4][0] == '1'||args_[4][0] == '2'||args_[4][0] == '3'||args_[4][0] == '4'))
    {
    	cout << "Некорректные входные данные";
    	return -1;
    }
    if (((stoi(args_[4]) == 0) && (argn == 6)) || (((0 < stoi(args_[4])) && (stoi(args_[4]) < 5)) && (argn == 5)))
    {}
    else
    {
    	cout << "Некорректные входные данные";
    	return -1;
    }
    
    clock_t start, end;
    
    long double total_time;
    int return_flag = 1;
    int i, j, error;
    double *matrix;
    double *inverse_matrix;
    double *Copymatrix;
    
    int n = stoi(args_[1]); //считываем переменные
    int threads_count = stoi(args_[2]);
    int m = stoi(args_[3]);
    int form = stoi(args_[4]);
    pthread_t *threads;
    Args *args;
    
    if (n <= 0){cout<<"Некорректные входные данные"; return -1;}
    if (m > n||m <= 0){cout<<"Некорректные входные данные"; return -1;}
    if (threads_count <= 0){cout<<"Некорректные входные данные"; return -1;}

    try
    {
	    matrix = new double[n*n];
	    Copymatrix = new double[n*n];
	    inverse_matrix = new double[n*n];
	    args = new Args [threads_count];
	    threads = new pthread_t [threads_count];
	    
	    error = entermatrix(matrix, args_[5], n, form); // ENTERMATRIX
	    
	    if (error == -1)
	    {
	    	cout << "Ошибка при открытии файла";
	    	delete [] matrix;
	    	return -1;
	    }
	    else if (error == -2)
	    {
	    	cout << "Ошибка при чтении файла";
	    	delete [] matrix;
	    	return -1;
	    }
	    
	    
	    for (i = 0; i < n; ++i)
	    {
	    	for (j = 0; j < n; ++j)
	    	{
	    		Copymatrix[i*n+j] = matrix[i*n+j];
	    	}
	    }//Копируем матрицу
	    
	    
	    for (i = 0; i < n; ++i)
	    {
		for (j = 0; j < n; ++j)
		{   
		    if (i == j)
		        inverse_matrix[i*n+j] = 1;
		    else
		        inverse_matrix[i*n+j] = 0;
		}
	    }
	    
	    
		for (i = 0; i < threads_count; ++i)
	    {
		args[i].matrix = matrix;
		args[i].inverse_matrix = inverse_matrix;
		args[i].n = n;
		args[i].thread_num = i;
		args[i].threads_count = threads_count;
		args[i].return_flag = &return_flag;
	    }
	    
	    total_time = get_full_time();
	    for (i = 0; i < threads_count; ++i)
	    {
		if (pthread_create(threads+i, 0, Inverse, args+i))
		{
		    printf("Поток не создался.\n");
		    
		    delete []matrix;
		    delete []inverse_matrix;
		    delete []Copymatrix;
		    delete []args;
		    delete []threads;
		    
		    return -1;
		}
	    }
	    
	    for (i = 0; i < threads_count; ++i)
	    {
	    	//time[i] = get_time();
		if (pthread_join(threads[i], 0))
		{
		    
		    
		    delete []matrix;
		    delete []inverse_matrix;
		    delete []Copymatrix;
		    delete []args;
		    delete []threads;
		    
		    return -1;
		}
		//time[i] = get_time() - time[i];
	    }
	    
	    total_time = get_full_time() - total_time;
	    
	    //printf("Общее время работы алгоритма: %Lf sec.\n", total_time);
	    
	    if(!return_flag)
	    {
		printf("Матрица вырождена.\n");

		delete []matrix;
		delete []inverse_matrix;
		delete []Copymatrix;
		delete []args;
		delete []threads;
		
		return -1;
	    }
	    
	    total_time = get_full_time();
	    double norma_ = norma(n, Copymatrix, inverse_matrix, matrix);
	    total_time = get_full_time() - total_time;
	    printf("Время вычисления нормы невязки: %Lf sec.\n", total_time);
	    
	    outmatrix(n, n, m, inverse_matrix); // OUTMATRIX
	    
	    printf("%s : residual = %e elapsed = %.2Lf s = %d n = %d m = %d p = %d\n", args_[0], norma_, total_time, form, n, m, threads_count);
    }
    catch(const exception& e)
    {
    	std::cerr << e.what() << endl;
    	delete []matrix;
    	delete []Copymatrix;
    	delete []inverse_matrix;
    	delete []args;
    	delete []threads;
    	return -1; 
    }
    
    delete []matrix;
    delete []Copymatrix;
    delete []inverse_matrix;
    delete []args;
    delete []threads;
    
    return 0;
}

