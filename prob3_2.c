#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RAND_MAX pow(2,32)
#define SAMPLE 100
#define LOOP 10000

static unsigned int z;

double xor128(){ //[0,1)の乱数生成
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
  int i,j,k,check = 100;
  double prob[SAMPLE+1]; //prob[k]は表がk回出る確率
  FILE *fp;
  char filename[20];

  z = (unsigned int)time(NULL); //乱数の初期値

  for(i=0;i<SAMPLE+1;i++){
    prob[i] = 0;
  }

  for(i=0;i<LOOP;i++){
    k = 0;
    for(j=0;j<SAMPLE;j++){ //コインをSAMPLE回投げるシミュレーション
      if(xor128() < 0.5) k++; //表が出たらkをインクリメント
    }
    prob[k]++;

    if(i == (check-1)){ //シミュレーション回数がcheck回のとき相対頻度を出力
      sprintf(filename,"prob3_2_%d.txt",check);
      fp = fopen(filename,"w");
      for(j=0;j<SAMPLE+1;j++){
        fprintf(fp,"%d %lf\n",j,prob[j]/(i+1));
      }
      fclose(fp);
      check *= 10;
    }
  }

  return 0;
}