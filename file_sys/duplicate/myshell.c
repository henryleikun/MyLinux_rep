#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int lastcode = 0;
int quit = 0;

#define RIGHT "]"
#define LEFT "["
#define DIVI " \t"
#define LABLE "#"
#define LINE_SIZE 1024
#define ARG_SIZE 32
#define IN 0 //输入重定向 从指定文件里读而不是从键盘文件读
#define OUT 1 //输出重定向
#define APPEND 2 //追加重定向


char pwd[LINE_SIZE];
char myenv[LINE_SIZE];
char* redirfilename = NULL;
int redir = -1;

const char* getusername(){
  return getenv("USER");
}

const char* gethostname_(){//有重名
  return getenv("HOSTNAME");
}

//const char* getpwd(){
///  return getenv("PWD");
//}

void getpwd(){
  getcwd(pwd,sizeof(pwd));
}

void Check_redir(char* cline){
    //遍历字符串
    char* pos = cline;
    while(*pos){
      if(*pos == '>'){//输出or追加
        if(*(pos + 1) == '>'){//追加
          *pos++ = '\0';
          *pos++ = '\0';//把这两个>>变成\0做隔断符

          while(isspace(*pos)) pos++;//找到\0后的第一个不是空格的字符
          redirfilename = pos;
          redir = APPEND;
          
          break;
        } 
        else{//输出
          *pos++ = '\0';
          while(isspace(*pos)) pos++;
          redirfilename = pos;
          redir = OUT;
          break;
        }
      }
      else if(*pos == '<'){//输入
        *pos++ = '\0';
        while(isspace(*pos)) pos++;
        redirfilename = pos;
        redir = IN;
        break;
      }

      pos++;
    }
}


void Interact(char* cline,int size){
  getpwd();//初始化

  printf(LEFT"%s@%s %s"RIGHT""LABLE" ",getusername(),gethostname_(),pwd);
  //scanf("%s",cline); 不行
  char* s = fgets(cline,size,stdin);
  assert(s!=NULL);

  (void)s;//假用 不想看到warning
  cline[strlen(cline) - 1] = '\0';

  Check_redir(cline);//检查是否有重定向符号，若有就分割
}

int Splitcommand(char* cline,char* _argv[]){
  int i = 1;
  _argv[0] = strtok(cline,DIVI);
  while(_argv[i++] = strtok(NULL,DIVI));
  
  return i - 1;
}

int Checkcommand(char* _argv[],int _argc){
    if(_argc == 2 && strcmp(_argv[0],"cd") == 0){
      chdir(_argv[1]);
      getpwd();
      sprintf(getenv("PWD"),"%s",pwd);
      return 1;
    } 
    else if(_argc == 2 && strcmp(_argv[0],"echo") == 0){
      if(strcmp(_argv[1],"$?") == 0){
        printf("%d\n",lastcode);
        lastcode = 0;
      }
      else if(*_argv[1] == '$'){
       char* prin = getenv(_argv[1] + 1);
       if(prin) printf("%s\n",prin);//把$跳过      
      } 
      else 
        printf("%s",_argv[1]);//正常字符串
      
      return 1;
    }
    else if(_argc == 2 && strcmp(_argv[0],"export") == 0 ){
      // myenv = _argv[1]; 你是弱智？C风格的字符串不让拷贝啊
      strcpy(myenv,_argv[1]);
      putenv(myenv);
      return 1;
    }

    if(strcmp(_argv[0],"ls") == 0){
      _argv[_argc++] = "--color";
      _argv[_argc] = NULL;
    }

    return 0;//不是内建命令返回0
}

void ExecuteNormally(char* _argv[]){
   pid_t id = fork();

   if(id < 0){
     perror("创建子进程失败\n");
     return ;
   }
   else if(id == 0){//child 创建子进程来进行程序进程替换
     if(redir == IN){//输入重定向
       int fd = open(redirfilename,O_RDONLY);
       if( fd <  0){
         perror("Open error\n");
         //printf("fd:%d\n",fd);
         return;
       }

       dup2(fd,0);
     }
     else if(redir == OUT){//输出重定向
       int fd = open(redirfilename,O_WRONLY|O_CREAT|O_TRUNC,0666);
       if( fd <  0){
         perror("Open error\n");
         return;
       }

       dup2(fd,1);
     }
     else if(redir == APPEND){
       int fd = open(redirfilename,O_WRONLY|O_CREAT|O_APPEND,0666);
       if( fd <  0){
         perror("Open error\n");
         return;
       }

       dup2(fd,1);
     }

     execvp(_argv[0],_argv);
     exit(11);//如果进程程序替换失败 子进程退出码设置为11
   }
   else{//parent
     int status;
     pid_t rid = waitpid(id,&status,0);

     if(rid == id){
       lastcode = WEXITSTATUS(status);//将对应最近的退出码给lastcode
     }
     else{
       perror("进程等待失败\n");
     }
   }
}

int main(){
  char commandline[LINE_SIZE];
  char* argv[ARG_SIZE];

  while(!quit){
    //初始化  清空上一次的一些值
    redirfilename = NULL;
    redir = -1;

    //命令行交互
    Interact(commandline,sizeof(commandline));
      
    //提取命令行对应的参数到argv中
    int argc =  Splitcommand(commandline,argv);
    if(argc == 0) continue;

    //检查命令是否是内建命令 对一些命令做特殊处理 比如 ls加上颜色
    // if(argc == 2 && strcmp(argv[0],"cd") == 0){
    //   chdir(argv[1]);
    //   if(strcmp(argv[0],".") == 0) continue;
    //   else if(strcmp(argv[0],"..") == 0){
    //     // 将上级路径写到环境变量PWD里面去
    //   }
    //   else{//正常的路径（绝对路径）
    //     sprintf(getenv("PWD"),"%s",argv[1]);
    //   }
    // }
   
    if(Checkcommand(argv,argc)) continue;

    //创建子进程执行正常命令
    ExecuteNormally(argv);
  }
  
  return 0;
}
