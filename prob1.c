#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define LOOP 10
#define RAND_MAX pow(2,32)

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
  int i,j,k,sample=10;
  double x,sum=0,memo[10],ave,dev;

  z = ((unsigned int)time(NULL)); //乱数の初期値

  for(i=0;i<4;i++){
    ave = 0;
    for(j=0;j<LOOP;j++){
      sum = 0;
      for(k=0;k<sample;k++){
        x = xor128();
        sum += 4.0 / (1 + x*x);
      }
      memo[j] = sum / sample;
      ave += memo[j];
    }
    ave /= LOOP; //推定値を計算

    dev = 0;
    for(j=0;j<LOOP;j++){
      dev += pow(memo[j]-ave,2);
    }
    dev = sqrt(dev/(LOOP*(LOOP-1))); //精度を計算

    printf("サンプル数=%8d: 推定値=%lf,精度=%.6e\n",sample,ave,dev);

    sample *= 100; //サンプル数を100倍
  }
  return 0;
}