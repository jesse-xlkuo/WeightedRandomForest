#ifndef MDSCALING_H
#define MDSCALING_H
#include<iostream>
#include<complex>
using namespace std;
//class MDScaling
//{
//public:
	double* getOutMatrix(double *B, int n, double eps, int jt);
	void innerproduct(complex<double> *a, int n, complex<double> *b);
	void brmul(complex<double> *a, complex<double> *b, int n, int m, int k, complex<double> *c);
	int cjcbi(complex<double> *a, int n, complex<double> *v, complex<double> eps,int jt);
	
//public:
//	int n;
	//double *OutMatrix;
	//double *InMatrix;
//};
#endif
