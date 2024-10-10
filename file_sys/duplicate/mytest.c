#include <stdio.h>

int main(){
  fprintf(stdout,"hello normal message\n");
  fprintf(stdout,"hello normal message\n");
  fprintf(stdout,"hello normal message\n");
  fprintf(stdout,"hello normal message\n");

  fprintf(stderr,"hello error message\n");
  fprintf(stderr,"hello error message\n");
  fprintf(stderr,"hello error message\n");
  fprintf(stderr,"hello error message\n");

  return 0;
}
