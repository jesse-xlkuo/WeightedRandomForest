#ifndef MDSCALING_H
#define MDSCALING_H
#include<iostream>

//class MDScaling
//{
//public:
	double* getOutMatrix(double *B, int n, double eps, int jt);
	void innerproduct(double *a, int n, double *b);//������
	void brmul(double *a, double *b, int n, int m, int k, double *c);//�������
	int cjcbi(double *a, int n, double *v, double eps,int jt);//��Jocabi������ʵ�Գƾ�������ֵ����������
	
//public:
//	int n;
	//double *OutMatrix;
	//double *InMatrix;
//};
#endif