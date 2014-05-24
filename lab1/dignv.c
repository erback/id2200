#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define WRITE 1
#define READ 0




void errHandler(char*);
void closePipe(int []);
pid_t startFork(void);
void makePipe(int []);

int p1[2], p2[2], p3[2];


int main(int argc, char** argv){

  pid_t pid1, pid2 = NULL , pid3, pid4;

  makePipe(p1);

  /**PARENT*******************************************/
  
  pid1 = startFork();

  /**FIRST CHILD*************PRINTENV****************/
  if (pid1 == 0){

    if (dup2(p1[WRITE],STDOUT_FILENO)== -1){
      errHandler("dup2 p1 miss");
    }

    closePipe(p1);
    

    execlp("printenv", "printenv", NULL); 
    
    errHandler("Error when executing printenv");
    
  }


  /**Second child*************GRIP****************/

  /*if (1<argc){ 
      if (dup2(p1[READ],STDOUT_FILENO)== -1)
              errHandler("dup2 miss");

      waitpid(pid1, NULL, 0);
      makePipe(p2);

      pid2 = startFork();

      if(pid2 == 0){

          if (dup2(p2[WRITE],STDOUT_FILENO)== -1)
              errHandler("dup2 miss");

          closePipe(p1);
          closePipe(p2);
          execvp("grep", argv);
          errHandler("Grep did not work");
      }

  }*/


  /**Third child*************SORT****************/

   makePipe(p2);

  if (dup2(p1[READ],STDOUT_FILENO)== -1)
              errHandler("dup2 p1 read miss");
  /*if (NULL != pid2)
    waitpid(pid2, NULL, 0);*/

          
  waitpid(pid1, NULL, 0);
  


  pid3 = startFork();


  if (pid3 == 0){

      if (dup2(p2[WRITE],STDOUT_FILENO)== -1)
              errHandler("dup2 p2 write miss");

      closePipe(p1);
      closePipe(p2);

      execlp("sort", "sort", NULL);

      errHandler("Sort failed");

  }



/**PARENT*******************************************/

 
  if (dup2(p2[READ],STDOUT_FILENO)== -1)
              errHandler("dup2 p2 read miss");

    waitpid(pid3, NULL, 0);


    char *args4exec[] = {"less", NULL};

    execvp(args4exec[0], args4exec);
      errHandler("Less did not work");



 closePipe(p2);
 closePipe(p1);
  
  return 0;
}


void makePipe(int thePipe[]){
  if (pipe(thePipe) == -1)
    errHandler("pipe1");
}

pid_t startFork(){

  pid_t pid = fork();
  if (pid == -1)
    errHandler("Fork error");
  return pid;
}

void closePipe(int pipeEnd[2]){

  if (close(pipeEnd[READ]) == -1)
    errHandler("error when closing pipe, read");

  if (close(pipeEnd[WRITE]) == -1)
    errHandler("error when closing pipe, write");
}

void errHandler(char* msg){
  perror(msg);
  exit(EXIT_FAILURE);
}