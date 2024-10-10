//#include "mymath.h"
#include "myprint.h"
#include "myerror.h"

int main(){
  //extern int myerrno;
  //printf("2 + 3 = %d\n",Add(2,3));
  
  //int ret = Div(1,0);
  //printf("1 / 0 = %d,myerrno = %d\n",ret,myerrno);

  Print();
  Error("Open failed\n");
   
  return 0;
}
