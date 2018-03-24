#include <stdio.h>
#include <string.h>
#include <math.h>

#define RAND_MAX pow(2,32)

int size;

double xor128(){
  static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
  unsigned int t;
  t=x^(x<<11);
  x=y;
  y=z;
  z=w;
  w=(w^(w>>19))^(t^(t>>8));
  return w / RAND_MAX;
}

void print_map(int S[][size],int bond[][size]){
  int i,j;
  printf(" ");
  for(j=0;j<size;j++){
    if(bond[2*size-1][j] == 1){
      printf("  | ");
    }else{
      printf("    ");
    }
  }
  printf("\n");

  for(i=0;i<size;i++){
    if(bond[2*i][size-1] == 1){
      printf("-");
    }else{
      printf(" ");
    }
    for(j=0;j<size;j++){
      printf("%3d",S[i][j]);
      if(bond[2*i][j] == 1){
        printf("-");
      }else{
        printf(" ");
      }
    }
    printf("\n ");
    for(j=0;j<size;j++){
      if(bond[2*i+1][j] == 1){
        printf("  | ");
      }else{
        printf("    ");
      }
    }
    printf("\n");
  }
}

void print_num(int num[][size],int bond[][size]){
  int i,j;
  printf(" ");
  for(j=0;j<size;j++){
    if(bond[2*size-1][j] == 1){
      printf("  | ");
    }else{
      printf("    ");
    }
  }
  printf("\n");

  for(i=0;i<size;i++){
    if(bond[2*i][size-1] == 1){
      printf("-");
    }else{
      printf(" ");
    }
    for(j=0;j<size;j++){
      printf("%3d",num[i][j]);
      if(bond[2*i][j] == 1){
        printf("-");
      }else{
        printf(" ");
      }
    }
    printf("\n ");

    for(j=0;j<size;j++){
      if(bond[2*i+1][j] == 1){
        printf("  | ");
      }else{
        printf("    ");
      }
    }
    printf("\n");
  }
}

int delta(int ,int );
double W_calc(int S[][size],int ,double );
void update_S(int S[][size],double );
void update_G(int S[][size],int G[][size],double );
int number(int ,int ,int L[][size]);
void cluster_number(int num[][size],int L[][size],int G[][size],int cnt[]);

int main(void){
  size = 16;
  int i,j,k,l,sample=1e3,select;
  int S[size][size],G[2*size][size],num[size][size],L[size][size],cnt[size*size];
  double z = 2,M,sum_M=0,f,sum_f=0;
  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      cnt[i*size+j] = 0;
      if(xor128()<0.5){
        S[i][j] = 0;
      }else{
        S[i][j] = 1;
      }
    }
  }

  for(k=0;k<100;k++){
    update_S(S,z);
  }

  for(k=0;k<sample;k++){
    update_S(S,z);
    update_G(S,G,z);
    cluster_number(num,L,G,cnt);

    f=0;
    M=0;
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        if(num[0][i] == num[size/2][j]){
          f = 1;
          M = fmax(M,cnt[num[0][i]]);
          break;
        }
      }
    }
    sum_M += M;
    sum_f += f;
  }
  print_map(S,G);
  print_num(num,G);
  printf("%lf %lf\n",sum_f/sample,sum_M/sample);

  return 0;
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

void update_S(int S[][size],double z){
  int i,select;
  for(i=0;i<size*size;i++){
    select = floor(xor128()*size*size);
    if(xor128() < fmin(1,W_calc(S,select,z))){
      S[select/size][select%size] = (S[select/size][select%size]+1)%2;
    }
  }
}

void update_G(int S[][size],int G[][size],double z){
  int i,j;

  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      if(xor128()< 1/(z*delta(S[i][j],S[i][(j+1)%size])+1)){
        G[2*i][j] = 0;
      }else{
        G[2*i][j] = 1;
      }
      if(xor128()< 1/(z*delta(S[i][j],S[(i+1)%size][j])+1)){
        G[2*i+1][j] = 0;
      }else{
        G[2*i+1][j] = 1;
      }
    }
  }
}

int number(int i,int j,int L[][size]){
  if(L[i][j] == -1){
    return size*i+j;
  }else{
    return number(L[i][j]/size,L[i][j]%size,L);
  }
}

void cluster_number(int num[][size],int L[][size],int G[][size],int cnt[]){
  int i,j,r_1,r_2;

  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      L[i][j] = -1;
      cnt[i*size+j] = 0;
    }
  }

  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      if(G[2*i][j] == 1){
        r_1 = number(i,j,L);
        r_2 = number(i,(j+1)%size,L);
        if(r_1 < r_2){
          L[r_2/size][r_2%size] = r_1;
        }else if(r_1 > r_2){
          L[r_1/size][r_1%size] = r_2;
        }
      }
      if(G[2*i+1][j] == 1){
        r_1 = number(i,j,L);
        r_2 = number((i+1)%size,j,L);
        if(r_1 < r_2){
          L[r_2/size][r_2%size] = r_1;
        }else if(r_1 > r_2){
          L[r_1/size][r_1%size] = r_2;
        }
      }
    }
  }

  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      num[i][j] = number(i,j,L);
      cnt[num[i][j]]++;
    }
  }
}