#include <stdio.h>

int GetSum(int num){
  int sum = 0;
  
  for(int i = 0; i < num; i++){
      sum += i;
  }

  return sum;
}

int main(){
  int times = 100;
  printf("%d\n",GetSum(times));

  return 0;
}
