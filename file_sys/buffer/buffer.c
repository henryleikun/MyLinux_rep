#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(){
  const char* str1 = "Hello fwrite\n";
  const char* str2 = "Hello write\n";

  printf("Hello printf\n");
  sleep(2);
  fprintf(stdout,"Hello fprintf\n");
  sleep(2);
  fwrite(str1,strlen(str1),1,stdout);
  sleep(2);

  write(1,str2,strlen(str2));
  sleep(5);
  //close(1);
  
  //fork();

  return 0;
}
