#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RAND_MAX pow(2,32)

int size;

static unsigned int v;

double xor128(){
  static unsigned int x=123456789,y=362436069,w=88675123;
  unsigned int t;
  t=x^(x<<11);
  x=y;
  y=v;
  v=w;
  w=(w^(w>>19))^(t^(t>>8));
  return w / RAND_MAX;
}

int delta(int i,int j){
  if(i == j){
    return 1;
  }else{
    return 0;
  }
}

double W_calc(int S[][size],int select,double z){
  double W=1;
  int i=select/size,j=select%size;
  int next = (S[i][j]+1)%2;

  W *= (z*delta(next,S[(i-1+size)%size][j])+1)*(z*delta(next,S[(i+1)%size][j])+1)*(z*delta(next,S[i][(j-1+size)%size])+1)*(z*delta(next,S[i][(j+1)%size])+1);
  W /= (z*delta(S[i][j],S[(i-1+size)%size][j])+1)*(z*delta(S[i][j],S[(i+1)%size][j])+1)*(z*delta(S[i][j],S[i][(j-1+size)%size])+1)*(z*delta(S[i][j],S[i][(j+1)%size])+1);

  return W;
}

int main(void){
  size = 48;
  int i,j,k,l,sample=1e3,select;
  int S[size][size];
  double E,z = sqrt(2),ave_E=0;

  v = (unsigned int)time(NULL);

  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      if(xor128()<0.5){
        S[i][j] = 0;
      }else{
        S[i][j] = 1;
      }
    }
  }

  for(k=0;k<sample;k++){
    E = 0;
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        E += delta(S[i][j],S[i][(j+1)%size]);
        E += delta(S[i][j],S[(i+1)%size][j]);
      }
    }
    E = E/(2*size*size);
    ave_E += E;
    printf("%d %lf\n",k,ave_E/(k+1));

    for(i=0;i<size*size;i++){
      select = floor(xor128()*size*size);
      if(xor128() < fmin(1,W_calc(S,select,z))){
        S[select/size][select%size] = (S[select/size][select%size]+1)%2;
      }
    }
  }

  E = 0;
  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      E += delta(S[i][j],S[i][(j+1)%size]);
      E += delta(S[i][j],S[(i+1)%size][j]);
    }
  }
  E /= 2*size*size;
  ave_E += E;
  printf("%d %lf\n",k,ave_E/(k+1));

  return 0;
}