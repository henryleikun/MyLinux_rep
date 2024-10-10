#include <stdio.h>
#include <unistd.h>

int main(){
  while(1){
   printf("Hello Linux! pid:%d\n",getpid());
   sleep(1);
  }

  return 0;
}
