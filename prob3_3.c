#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RAND_MAX pow(2,32)
#define SAMPLE 10000
#define N 100

static unsigned int z;

double xor128(){ //[0,1)の乱数を生成
  static unsigned int x=123456789,y=362436069,w=88675123;
  unsigned int t;
  t=x^(x<<11);
  x=y;
  y=z;
  z=w;
  w=(w^(w>>19))^(t^(t>>8));
  return w / RAND_MAX;
}

int main(void){
  int i,j,k_init,k,next,check = 100;
  double p,q,pr,binomial[N+1],prob[N+1];
  FILE *fp;
  char filename[20];

  z = (unsigned int)time(NULL); //乱数の初期値

  printf("kの初期値 -> ");
  scanf("%d",&k_init);

  //二項分布の計算
  p = 0.5;
  q = 1 - p;
  pr = pow(q, 100);
  for(i=0;i<N;i++){
    binomial[i] = pr;
    pr *= (N-i)*p/((i+1)*q);
  }
  binomial[i] = pr; //binomial[i]は二項分布のx=iの値

  for(i=0;i<N+1;i++){
    prob[i] = 0;
  }

  k = k_init;

  for(i=0;i<SAMPLE;i++){
    //0.5の確立で1増減する
    if(xor128() < 0.5){
      next = (k+1)%(N+1);
    }else{
      next = (k-1)%(N+1);
    }

    //確立min[1,W(next)/W(k)]でnextへ遷移
    if(xor128() < fmin(1,binomial[next]/binomial[k])){
      k = next;
    }

    prob[k]++;

    if(i == (check-1)){ //サンプル数がcheck個のとき相対頻度を出力
      sprintf(filename,"prob3_3_%d_%d.txt",k_init,check);
      fp = fopen(filename,"w");
      for(j=0;j<N+1;j++){
        fprintf(fp,"%d %lf\n",j,prob[j]/(i+1));
      }
      fclose(fp);
      check *= 10;
    }
  }

  return 0;
}