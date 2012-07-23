#include "MDScaling.h"
#include <math.h>
#include <algorithm>

using namespace std;

complex<double>* doubleToComplex(double *B, int n){
	complex<double>* B_complex = (complex<double>*)malloc(sizeof(complex<double>) * n);
	int i;
	for(i = 0; i < n; ++ i){
		B_complex[i] = complex<double>(B[i],0.0);
	}
	return B_complex;
}

double* getOutMatrix(double *B, int n, double eps, int jt)
{
	complex<double>* B_complex = doubleToComplex(B,n);
	complex<double> eps_complex = complex<double>(eps,0.0);
	complex<double> *B2_complex, *BB_complex;
	B2_complex = (complex<double> *)malloc(n*n*sizeof(complex<double>));
	BB_complex = (complex<double> *)malloc(n*n*sizeof(complex<double>));
	//step1: get B2 = B.*B
	innerproduct(B_complex, n, B2_complex);
	
	//step2: get BB(j,k) = -0.5*B(j,k)^2+0.5*sum(B2(1:n,k))/n+0.5*sum(B2(j,1:n))/n-0.5*sum(sum(B2(1:n,1:n)))/n^2;
	complex<double> *sum1_complex, *sum2_complex;
	complex<double> sum3_complex;
	sum1_complex = (complex<double> *)malloc(n*sizeof(complex<double> *));//sum(B2(1:n,k))
	sum2_complex = (complex<double> *)malloc(n*sizeof(complex<double> *));//sum(B2(j,1:n))
	sum3_complex = complex<double>(0.0,0.0);//sum(sum(B2(1:n,1:n)))
	for(int j = 0; j < n; j ++)
	{
		sum1_complex[j] = complex<double>(0.0,0.0);
		sum2_complex[j] = complex<double>(0.0,0.0);
		for(int k = 0; k < n; k++)
		{
			sum3_complex = sum3_complex + B2_complex[j*n+k];
			sum1_complex[j] = sum1_complex[j] + B2_complex[j*n+k];
			sum2_complex[j] = sum2_complex[j] + B2_complex[k*n+j];
		}
	}
	//BB(j,k)
	for(int j = 0; j < n; j++)
	{
		for(int k = 0; k < n; k++)
		{
			BB_complex[j*n+k] = complex<double>(-0.5,0.0)*B_complex[j*n+k]*B_complex[j*n+k] +
					complex<double>(0.5,0.0)*sum1_complex[k]/complex<double>((double)n,0.0)
					+ complex<double>(0.5,0.0)*sum2_complex[j]/complex<double>((double)n,0.0)
					- complex<double>(0.5,0.0)*sum3_complex/complex<double>((double)(n*n),0.0);
		}
	}

	//Step3: [V,D] = eig(BB)

	int result = cjcbi(BB_complex, n, B_complex, eps_complex,jt); //D = BB, V = B

	//if(result < 0) return null;//error

	//step4: DD=sqrtm(D)
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
		{
			if(i==j) B2_complex[i*n+j] = sqrt(complex<double>(abs(BB_complex[i*n+j]),0.0));
			else
				B2_complex[i*n+j] = complex<double>(0.0,0.0);
		}

	//Step5: OutMatrix = BB = V*DD
	brmul(B_complex, B2_complex,  n, n,  n, BB_complex);
	
	//Step6: XX = BB(1:n,1)		YY = BB(1:n,2);
	double *xy;
	xy = (double *)malloc(2*n*sizeof(double));
	for(int i = 0; i < n; i++)
	   {
		//xy[i*2+0] = BB_complex[i*n+0];
		//xy[i*2+0] = imag(BB_complex[i*n+0]);
		xy[i*2+0] = abs(BB_complex[i*n+0]);
		//xy[i*2+1] = BB_complex[i*n+1];
		//xy[i*2+1] = imag(BB_complex[i*n+1]);
		xy[i*2+1] = abs(BB_complex[i*n+1]);
	   }
	//Step7: free
	free(B_complex);
	free(B2_complex);
	free(BB_complex);
	//Step8: end, return 
	return xy;
}
void brmul(complex<double> *a, complex<double> *b, int n, int m, int k, complex<double> *c)
{

	int i, j, l, u;
	for (i = 0; i <= m - 1; i++)
		for (j = 0; j <= k - 1; j++) {
			u = i * k + j;
			c[u] = complex<double>(0.0,0.0);
			for (l = 0; l <= n - 1; l++)
				c[u] = c[u] + a[i * n + l] * b[l * k + j];
		}
	return;
}

void innerproduct(complex<double> *a, int n, complex<double> *aa)
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

int cjcbi(complex<double> *a_complex, int n, complex<double> *v_complex, complex<double> eps_complex,int jt)
{ 
	int i, j, p, q, u, w, t, s, tt;
	complex<double> fm_complex, cn_complex, sn_complex, omega_complex, x_complex, y_complex, d_complex;
	tt = 1;
	for (i = 0; i <= n - 1; i++) {
		v_complex[i * n + i] = complex<double>(1.0, 0.0);
		for (j = 0; j <= n - 1; j++)
			if (i != j)
				v_complex[i * n + j] = complex<double>(1.0, 0.0);
	}
	while (1 == 1) {
		fm_complex = complex<double>(0.0,0.0);
		for (i = 1; i <= n - 1; i++)
			for (j = 0; j <= i - 1; j++) {
				//d = fabs(a[i * n + j]);
				d_complex = complex<double>(abs(a_complex[i * n + j]),0.0);
				if ((i != j) && (abs(d_complex) > abs(fm_complex))) {
					fm_complex = d_complex;
					p = i;
					q = j;
				}
			}
		if (abs(fm_complex) < abs(eps_complex))
			return (1);
		if (tt > jt)
			return (-1);
		tt = tt + 1;
		u = p * n + q;
		w = p * n + p;
		t = q * n + p;
		s = q * n + q;
		x_complex = -a_complex[u];
		y_complex = (a_complex[s] - a_complex[w]) / complex<double>(2.0,0.0);
		omega_complex = x_complex / sqrt(x_complex * x_complex + y_complex * y_complex);
		if (abs(y_complex) < abs(complex<double>(0.0,0.0)))
			omega_complex = -omega_complex;
		sn_complex = 1.0 + sqrt(1.0 - omega_complex * omega_complex);
		sn_complex = omega_complex / sqrt(complex<double>(2.0,0.0) * sn_complex);
		cn_complex = sqrt(complex<double>(1.0,0.0) - sn_complex * sn_complex);
		fm_complex = a_complex[w];
		a_complex[w] = fm_complex * cn_complex * cn_complex + a_complex[s] * sn_complex * sn_complex + a_complex[u] * omega_complex;
		a_complex[s] = fm_complex * sn_complex * sn_complex + a_complex[s] * cn_complex * cn_complex - a_complex[u] * omega_complex;
		a_complex[u] = complex<double>(0.0,0.0);
		a_complex[t] = complex<double>(0.0,0.0);
		for (j = 0; j <= n - 1; j++)
			if ((j != p) && (j != q)) {
				u = p * n + j;
				w = q * n + j;
				fm_complex = a_complex[u];
				a_complex[u] = fm_complex * cn_complex + a_complex[w] * sn_complex;
				a_complex[w] = -fm_complex * sn_complex + a_complex[w] * cn_complex;
			}
		for (i = 0; i <= n - 1; i++)
			if ((i != p) && (i != q)) {
				u = i * n + p;
				w = i * n + q;
				fm_complex = a_complex[u];
				a_complex[u] = fm_complex * cn_complex + a_complex[w] * sn_complex;
				a_complex[w] = -fm_complex * sn_complex + a_complex[w] * cn_complex;
			}
		for (i = 0; i <= n - 1; i++) {
			u = i * n + p;
			w = i * n + q;
			fm_complex = v_complex[u];
			v_complex[u] = fm_complex * cn_complex + v_complex[w] * sn_complex;
			v_complex[w] = -fm_complex * sn_complex + v_complex[w] * cn_complex;
		}
	}
	return (1);
}



