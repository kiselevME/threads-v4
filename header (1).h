#ifndef header_h
#define header_h
int entermatrix(double*, char*, int, int);
void inverse(double*, double*, int, int, int, int*);
void synchronize(int threads_count);
double norma(int, double*, double*, double*); 
double norm_matrix(int, double*);
void outmatrix(int, int, int, double*); 
long double get_time();
long double get_full_time();
#endif//header

