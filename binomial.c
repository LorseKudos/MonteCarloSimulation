#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void){
  int i,n = 100;
  double p,q,pr;

  p = 0.5;
  q = 1 - p;
  pr = pow(q, 100);
  for(i=0;i<n;i++){
    printf("%d %.10lf\n",i,pr);
    pr *= (n-i)*p/((i+1)*q);
  }
  printf("%d %.10lf\n",i,pr);

  return 0;
}