#include "mymath.h"

int myerrno = 0;

int Add(int a,int b){
  return a + b;
}

int Sub(int a,int b){
  return a - b;
}

int Mul(int a,int b){
  return a *  b;
}

int Div(int a,int b){
  if(b == 0){
    myerrno = 1;
    return  -1;
  }

  return a / b;
}
