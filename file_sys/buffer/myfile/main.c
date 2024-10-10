#include "mystdio.h"
#include <stdio.h>
#include <string.h>
#define filename "file.txt"

int main(){
  _FILE* fp = _fopen(filename,"a");

  if(fp == NULL){
    perror("open failed\n");
    return 1;
  }

  const char* mes = "Hello Linux\n";
  
  int cnt = 5;
  while(cnt--){

  _fwrite(fp,mes,strlen(mes));
  }

   
  _fclose(fp);
  return 0;
}
