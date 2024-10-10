#include <stdio.h>
#include <stdlib.h>

int g_val;
int g_val_init = 100;

int main(){
  char* str = "hello";
  char* ptr = (char*)malloc(16);
  
  printf("code_add:%p\n",main);
  printf("const_string_add:%p\n",str);
  printf("init_global_add:%p\n",&g_val_init);
  printf("uninit_global_add:%p\n",&g_val);
  printf("heap_add:%p\n",ptr);
  printf("stack_add:%p\n",&ptr);

  return 0;
}
