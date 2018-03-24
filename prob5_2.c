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

int delta(int ,int ); //引数が同じなら1,そうでなければ0を返す
double W_calc(int S[][size],int ,double ); //W(S_next)/W(S)を返す
void update_S(int S[][size],double ); //Sを更新
void update_G(int S[][size],int G[][size][2],double ); //Gを更新
int number(int ,int ,int L[][size]); //(i,j)要素のクラスター番号を求める
void cluster_number(int num[][size],int L[][size],int G[][size][2],int cnt[]); //全ての要素にクラスター番号をつけ,各クラスターの要素の個数を求める

int main(void){
  size = 12;
  int i,j,k,l,loop,sample=1e4,select;
  int S[size][size],G[size][size][2],num[size][size],L[size][size],cnt[size*size];
  //num[i][j]は(i,j)要素のクラスター番号,cnt[i]はクラスター番号がiである要素の個数,G[i][j][0],G[i][j][1]は(i,j)要素の右向き,下向きの枝
  double z = 1.2,M,sum_M,ave_M,f,sum_f,ave_f; //zは活動度

  v = (unsigned int)time(NULL); //乱数の初期値

  for(loop=0;loop<12;loop++){
    z += 0.025;
    ave_M = 0;
    ave_f = 0;

    for(l=0;l<10;l++){
      sum_M = 0;
      sum_f = 0;

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

      //最初の1000MCS
      for(k=0;k<1000;k++){
        update_S(S,z);
      }

      //SAMPLE回のMCS
      for(k=0;k<sample;k++){
        update_S(S,z);
        update_G(S,G,z);
        cluster_number(num,L,G,cnt);

        f=0;
        M=0;
        //浸透しているかどうか,浸透している最大クラスターに含まれる要素の個数を調べる
        for(i=0;i<size;i++){
          for(j=0;j<size;j++){
            if(num[0][i] == num[size/2][j]){
              f = 1;
              M = fmax(M,cnt[num[0][i]]);
              break;
            }
          }
        }
        sum_M += M/(size*size);
        sum_f += f;
      }
      ave_M += sum_M/sample;
      ave_f += sum_f/sample;
    }
    printf("%lf %lf %lf\n",z,ave_f/10,ave_M/10);
  }

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
    select = floor(xor128()*size*size); //候補となる要素
    //確率min[1,W(S_next)/W(S)]で候補要素をフリップ
    if(xor128() < fmin(1,W_calc(S,select,z))){
      S[select/size][select%size] = (S[select/size][select%size]+1)%2;
    }
  }
}

void update_G(int S[][size],int G[][size][2],double z){
  int i,j;

  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      //確率1/(z*delta+1)で(i,j)要素の右向きの枝を結ばない
      if(xor128()< 1/(z*delta(S[i][j],S[i][(j+1)%size])+1)){
        G[i][j][0] = 0;
      }else{
        G[i][j][0] = 1;
      }

      //確率1/(z*delta+1)で(i,j)要素の下向きの枝を結ばない
      if(xor128()< 1/(z*delta(S[i][j],S[(i+1)%size][j])+1)){
        G[i][j][1] = 0;
      }else{
        G[i][j][1] = 1;
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

void cluster_number(int num[][size],int L[][size],int G[][size][2],int cnt[]){
  int i,j,r_1,r_2;

  //Lとcntの初期化
  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      L[i][j] = -1;
      cnt[i*size+j] = 0;
    }
  }

  //配列Lの更新
  for(i=0;i<size;i++){
    for(j=0;j<size;j++){
      if(G[i][j][0] == 1){
        r_1 = number(i,j,L);
        r_2 = number(i,(j+1)%size,L);
        if(r_1 < r_2){
          L[r_2/size][r_2%size] = r_1;
        }else if(r_1 > r_2){
          L[r_1/size][r_1%size] = r_2;
        }
      }
      if(G[i][j][1] == 1){
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
}