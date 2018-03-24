#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define RAND_MAX pow(2,32)

int size; //システムサイズ

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


int number(int i,int j,int L[][size]){ //(i,j)要素にクラスター番号をつける
  if(L[i][j] == -1){
    return size*i+j;
  }else{
    return number(L[i][j]/size,L[i][j]%size,L);
  }
}

int main(void){
  printf("size? -> ");
  scanf("%d",&size);
  int i,j,k,l,sample=1e4,r_1,r_2,M;
  int num[size][size],L[size][size],cnt[size*size],bond[size][size][2];
  //num[i][j]は(i,j)要素のクラスター番号,cnt[i]はクラスター番号がiである要素の個数,bond[i][j][0],bond[i][j][1]は(i,j)要素の右向き,下向きの枝が接続してるなら1,接続してないなら0
  double p = 0.5,ave_M = 0; //pは枝が接続される確率

  z = (unsigned int)time(NULL); //乱数の初期値

  //Lとcntの初期化
  for(l=0;l<sample;l++){
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        L[i][j] = -1;
        cnt[i*size+j] = 0;
      }
    }

    //全ての枝を確率0.5で結ぶ
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        for(k=0;k<2;k++){
          if(xor128() < p){
            bond[i][j][k] = 1;
          }else{
            bond[i][j][k] = 0;
          }
        }
      }
    }

    //配列Lの更新
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        if(bond[i][j][0] == 1){
          r_1 = number(i,j,L);
          r_2 = number(i,(j+1)%size,L);
          if(r_1 < r_2){
            L[r_2/size][r_2%size] = r_1;
          }else if(r_1 > r_2){
            L[r_1/size][r_1%size] = r_2;
          }
        }

        if(bond[i][j][1] == 1){
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

    //各要素のクラスター番号を計算し,そのクラスター番号の個数を求める
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        num[i][j] = number(i,j,L);
        cnt[num[i][j]]++;
      }
    }

    M = 0;
    //浸透している最大クラスターに含まれる要素の個数を調べる
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        if(num[0][i] == num[size/2][j]){
          M = fmax(M,cnt[num[0][i]]);
          break;
        }
      }
    }
    ave_M += M;
  }

  ave_M /= sample;
  printf("%lf\n",ave_M/(size*size));

  return 0;
}