#include <stdio.h>

#include "normalize.h"

int find_inx_min(double a[], int n) {
  int c, index = 0;
  double min = a[0];   
 
  for (c = 0; c < n; c++){
    if (a[c] < min){
    	index = c;
    	min = a[c];
	}
  }
  return index;
}

int find_inx_max(double a[], int n) {
  int c, index = 0;
  double max = a[0];   
 
  for (c = 0; c < n; c++){
    if (a[c] > max){
    	index = c;
    	max = a[c];
	}
  }
  return index;
}

double *getNormalize( double signal[] , int size , int lb , int ub){

	int result_min = find_inx_min(signal,size);
	int result_max = find_inx_max(signal,size);

	float sub1 = ub - lb;
	float sub2 = sub1/2;
	float sub3 = ub-sub2;
	
	float mid = sub3;
	
    double min_v = signal[result_min]; //np.min(sig)
    double max_v = signal[result_max]; //np.max(sig)
    
    float sub11 = max_v - min_v;
	float sub22 = sub11/2;
	float sub33 = max_v - sub22;
	float mid_v = sub33;
	
	float coef1 = ub - lb;
	float coef2 = max_v - min_v;
    float coef = coef1 / coef2;
	
	for(int i=0;i<size;i++){
		float sig1 = mid_v * coef;
		float sig2 = signal[i] * coef;
		float sig3 = sig2 - sig1 + mid; //0.3273
		signal[i] = sig3;
	}
    return signal;
}