#include "MDScaling.h"
#include <cmath>
#include <algorithm>
#include<iostream>

#include<fstream>
#include <boost/numeric/mtl/mtl.hpp>

double *getOutMatrix(double *source, int n, double eps, int jt)
{
	/*MultiDimensional Scaling
	Input:
	   B ˫����ʵ�Ͷ�ά���飬���ΪnXn
	   n ���ͣ���ʾ�����С
	   eps ˫����ʵ�ͱ����������������ֵ����������ľ���Ҫ��
	   jt ���ͣ�������������
	Output:
	   xy ˫���ȶ�ά���飬����Ϊ2xn����һ�б�ʾx��꣬�ڶ��б�ʾy���
	   
    */

	double* B = (double*)malloc(n*n*sizeof(double));
	void ConvertToDMatrix(double* a, int n, double* b);
	ConvertToDMatrix(source, n, B);
	
	double *B2, *BB;
	B2 = (double *)malloc(n*n*sizeof(double));
	BB = (double *)malloc(n*n*sizeof(double));
	//step1: get B2 = B.*B
	innerproduct(B, n, B2);
	
	//stepPrintDate2: get BB(j,k) = -0.5*B(j,k)^2+0.5*sum(B2(1:n,k))/n+0.5*sum(B2(j,1:n))/n-0.5*sum(sum(B2(1:n,1:n)))/n^2;
	double *sum1, *sum2;
	double sum3;
	sum1 = (double *)malloc(n*sizeof(double));//sum(B2(1:n,k))
	sum2 = (double *)malloc(n*sizeof(double));//sum(B2(j,1:n))
	sum3 = 0.0;//sum(sum(B2(1:n,1:n)))
	for(int j = 0; j < n; j++)
	{
		sum1[j] = 0.0;
		sum2[j] = 0.0;
		for(int k = 0; k < n; k++)
		{
			sum3 = sum3 + B2[j*n+k];
			sum1[j] = sum1[j] + B2[j*n+k];
			sum2[j] = sum2[j] + B2[k*n+j];
		}
	}
	//BB(j,k)
	for(int j = 0; j < n; j++)
	{
		for(int k = 0; k < n; k++)
		{
			BB[j*n+k] = -0.5*B[j*n+k]*B[j*n+k] + 0.5*sum1[k]/n + 0.5*sum2[j]/n - 0.5*sum3/(n*n); 
		}
	}


	//Step3: [V,D] = eig(BB)
	int result = cjcbi(BB, n, B, eps,jt); //D = BB, V = B
	if(result < 0) return NULL;//error

	//step4: DD=sqrtm(D)
	double s1 = -1.0, s2 = -1.0;
	int s1_index = -1, s2_index = -1;
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
		{

			if(i == j && BB[i*n+j] >= 0){
				if(s1 < 0 && s2 < 0){
					s1 = BB[i*n+j];
					s1_index = i;
				}else if(s2 < 0){
					if(BB[i*n+j] > s1){
						s2 = s1;
						s2_index = s1_index;
						s1 = BB[i*n+j];
						s1_index = i;
					}else{
						s2 = BB[i*n+j];
						s2_index = i;
					}
				}else{
					if(BB[i*n+j] > s1){
						s2 = s1;
						s2_index = s1_index;
						s1 = BB[i*n+j];
						s1_index = i;
					}else{
						if(BB[i*n+j] > s2){
							s2 = BB[i*n+j];
							s2_index = i;
						}

					}
				}
			}
		}

	double x1 = std::pow(s1,0.5);
	double x2 = std::pow(s2,0.5);
	double *xy;
	xy = (double *)malloc(2*n*sizeof(double));
	//std::cout << "max " << s1 << ":" << s2 << std::endl;

	for(int i = 0; i < n; i++)
	   {
		xy[i*2+0] = B[i*n + s1_index] * x1;
		xy[i*2+1] = B[i*n + s2_index] * x2;

	   }
	//Step7: free
	free(B);
	free(B2);
	free(BB);
	//Step8: end, return 
	return xy;
}
void brmul(double *a, double *b, int n, int m, int k, double *c)
{
	/*ʵ�������c = a*b
	    a ˫����ʵ�Ͷ�ά���飬���Ϊn x m
		b ˫����ʵ�Ͷ�ά���飬���Ϊm x k
		c ˫����ʵ�Ͷ�ά���飬���Ϊn x k,�洢������
	*/
 int i,j,l,u;
 for (i=0; i<=m-1; i++)
	for (j=0; j<=k-1; j++)
	{ u=i*k+j; c[u]=0.0;
	for (l=0; l<=n-1; l++)
	c[u]=c[u]+a[i*n+l]*b[l*k+j];
	}
return;
}
void innerproduct(double *a, int n, double *aa)
{
	int u;
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++)
		{
			u = i*n+j;
			aa[u] = a[u]*a[u];
		}
	}
}
int cjcbi(double *a, int n, double *v, double eps,int jt)
{ 
	/*
	��Jocabi������ʵ�Գƾ�������ֵ����������
	a ˫����ʵ�Ͷ�ά���飬���Ϊn x n������ʱ�Խ����ϴ��n������ֵ
	v ˫����ʵ�Ͷ�ά���飬���Ϊn x n��������������,���е�i����a(i,i)��Ӧ
	eps ˫����ʵ�ͱ��������ƾ���Ҫ��
	jt ���ͣ�������������
	�����صı�־ֵ<0�����ʾ�����jt�λ��ﲻ������Ҫ��
    �����صı�־ֵ>0�����ʾ���
	*/
	int i,j,p,q,u,w,t,s,tt;
	double fm,cn,sn,omega,x,y,d;
	tt=1;
	for (i=0; i<=n-1; i++)
		{ 
			v[i*n+i]=1.0;
		  for (j=0; j<=n-1; j++)
		    if (i!=j) v[i*n+j]=0.0;
		}
	while (1==1)
	{
			fm=0.0;
		for (i=1; i<=n-1; i++)
		for (j=0; j<=i-1; j++)
			{ d=fabs(a[i*n+j]);
			if ((i!=j)&&(d>fm))
			{ fm=d; p=i; q=j;}
			}
		if (fm<eps)  return(1);
		if (tt>jt)  return(-1);
		tt=tt+1;
		u=p*n+q; w=p*n+p; t=q*n+p; s=q*n+q;
		x=-a[u]; y=(a[s]-a[w])/2.0;
		omega=x/sqrt(x*x+y*y);
		if (y<0.0) omega=-omega;
		sn=1.0+sqrt(1.0-omega*omega);
		sn=omega/sqrt(2.0*sn);
		cn=sqrt(1.0-sn*sn);
		fm=a[w];
		a[w]=fm*cn*cn+a[s]*sn*sn+a[u]*omega;
		a[s]=fm*sn*sn+a[s]*cn*cn-a[u]*omega;
		a[u]=0.0; a[t]=0.0;
		for (j=0; j<=n-1; j++)
		if ((j!=p)&&(j!=q))
			{ u=p*n+j; w=q*n+j;
			fm=a[u];
			a[u]=fm*cn+a[w]*sn;
			a[w]=-fm*sn+a[w]*cn;
			}
		for (i=0; i<=n-1; i++)
		if ((i!=p)&&(i!=q))
			{ u=i*n+p; w=i*n+q;
			fm=a[u];
			a[u]=fm*cn+a[w]*sn;
			a[w]=-fm*sn+a[w]*cn;
			}
		for (i=0; i<=n-1; i++)
			{ u=i*n+p; w=i*n+q;
			fm=v[u];
			v[u]=fm*cn+v[w]*sn;
			v[w]=-fm*sn+v[w]*cn;
			}
	}
	return(1);
}

void ConvertToDMatrix(double* a, int n, double* b){

	int i, j;
	for(i = 0; i < n; ++ i){
		for(j = 0; j < n; ++ j){
			b[i*n + j] = a[i*n + i] + a[j*n + j] - 2*a[i*n + j];
		}

	}
}

void PrintDate(double* a, int n){
	//std::ofstream o("source");
	int i, j;
	for(i = 0; i < n; ++ i){
		for(j = 0; j < n; ++ j){
			//o << a[i*n +j] << "\t";
		}
		//o << std::endl;
}
}

