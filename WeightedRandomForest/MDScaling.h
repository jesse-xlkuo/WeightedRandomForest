#ifndef MDSCALING_H
#define MDSCALING_H
#include<iostream>

//class MDScaling
//{
//public:
	double* getOutMatrix(double *B, int n, double eps, int jt);
	void innerproduct(double *a, int n, double *b);//矩阵点乘
	void brmul(double *a, double *b, int n, int m, int k, double *c);//矩阵相乘
	int cjcbi(double *a, int n, double *v, double eps,int jt);//用Jocabi方法求实对称矩阵特征值与特征向量
	
//public:
//	int n;
	//double *OutMatrix;
	//double *InMatrix;
//};
#endif