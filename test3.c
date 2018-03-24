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

int number(int i,int j,int L[][size]){
  if(L[i][j] == -1){
    return size*i+j;
  }else{
    return number(L[i][j]/size,L[i][j]%size,L);
  }
}

void print_map(int bond[][size]){
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
      printf("%3d",i*size+j);
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

int main(void){
  printf("size? -> ");
  scanf("%d",&size);
  int i,j,k,sample=1e4,r_1,r_2;
  int num[size][size],L[size][size],cnt[size*size],bond[2*size][size];
  double p = 0.5,prob = 0;

  for(k=0;k<sample;k++){
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        L[i][j] = -1;
        cnt[i*size+j] = 0;
      }
    }

    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        if(xor128() < p){
          bond[2*i][j] = 1;
        }else{
          bond[2*i][j] = 0;
        }

        if(xor128() < p){
          bond[2*i+1][j] = 1;
        }else{
          bond[2*i+1][j] = 0;
        }
      }
    }

    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        if(bond[2*i][j] == 1){
          r_1 = number(i,j,L);
          r_2 = number(i,(j+1)%size,L);
          if(r_1 < r_2){
            L[r_2/size][r_2%size] = r_1;
          }else if(r_1 > r_2){
            L[r_1/size][r_1%size] = r_2;
          }
        }

        if(bond[2*i+1][j] == 1){
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

    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        if(num[0][i] == num[size/2][j]){
          prob++;
          i = size - 1;
          break;
        }
      }
    }
  }

  prob /= sample;
  printf("%lf\n",prob);


  return 0;
}