#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RAND_MAX pow(2,32)

int size; //システムサイズ

static unsigned int v;

double xor128(){ //[0,1)の乱数を生成
  static unsigned int x=123456789,y=362436069,w=88675123;
  unsigned int t;
  t=x^(x<<11);
  x=y;
  y=v;
  v=w;
  w=(w^(w>>19))^(t^(t>>8));
  return w / RAND_MAX;
}

int delta(int i,int j){ //i=Jなら1,そうでなければ0を返す
  if(i == j){
    return 1;
  }else{
    return 0;
  }
}

double W_calc(int S[][size],int select,double z){ //W(S_next)/W(S)を返す
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
  double E,z = sqrt(2),ave_E=0; //zは活動度

  v = (unsigned int)time(NULL); //乱数の初期値

  //Sの初期値,完全にランダムに状態を0または1にする
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
    //E(S)の計算
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        E += delta(S[i][j],S[i][(j+1)%size]);
        E += delta(S[i][j],S[(i+1)%size][j]);
      }
    }
    E = E/(2*size*size);
    printf("%d %lf\n",k,E);
    ave_E += E;

    //Sの更新
    for(i=0;i<size*size;i++){
      select = floor(xor128()*size*size); //候補となる要素
      //確率min[1,W(S_next)/W(S)]で候補要素をフリップ
      if(xor128() < fmin(1,W_calc(S,select,z))){
        S[select/size][select%size] = (S[select/size][select%size]+1)%2;
      }
    }
  }

  E = 0;
  //E(S)の計算
  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      E += delta(S[i][j],S[i][(j+1)%size]);
      E += delta(S[i][j],S[(i+1)%size][j]);
    }
  }
  E /= 2*size*size;
  ave_E += E;
  printf("%d %lf\n",k,E);

  return 0;
}