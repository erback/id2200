#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define WRITE 1
#define READ 0

#define DEBUG 1

int p1[2], p2[2], p3[2];

void err(char* msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

void closePipe(int pipeEnd[2]){

  if (close(pipeEnd[READ]) == -1)
    err("error when closing pipe, read");

  if (close(pipeEnd[WRITE]) == -1)
    err("error when closing pipe, write");
}

int main(int argc, char** argv, char** envp){

  int pid1, pid2, pid3;


  /*Creating pipes*/
  if (pipe(p1) == -1){
    err("pipe1");
  }
  if (pipe(p2) == -1){
    err("pipe2");
  }

  /*
  if (pipe(p3) == -1){
    err("pipe3");
  }
  */

  /*First fork*/
  pid1 = fork();
  if (pid1 == -1){
    err("fork1");
  }

  /**FIRST CHILD*************PRINTENV****************/
  else if (pid1 == 0){

    if (dup2(p1[WRITE],STDOUT_FILENO)== -1){
      err("dup2 miss");
    }

    closePipe(p2);
    closePipe(p1);

    if (execlp("printenv", "printenv", NULL) == -1){
      err("execlp printenv does not like you!");
    }
  }

  /**PARENT*******************************************/
  else {

    pid2 = fork();
    if (pid2 == -1){
      err("fork2");
    }

    /**SECOND*CHILD***********SORT*******************/
    else if (pid2 == 0){

      if (dup2(p1[READ],STDIN_FILENO) == -1){
    err( "dup2 p1 read" );
      }
      if (dup2(p2[WRITE], STDOUT_FILENO) == -1){
    err("dup2 p2 write");
      }

      closePipe(p2);
      closePipe(p1);

      if (execlp("sort", "sort", NULL) == -1){
    err("execlp sort does not like you!");
      }
     }

    /**PARENT*****************************************/
    else {

      pid3=fork();
      if (pid3 == -1){
    err("fork3");
      }
      /**THIRD CILD***************LESS****************/
      else if (pid3 == 0){

    if (dup2(p2[READ], STDIN_FILENO) == -1){
      err("err dup2 read");
    }

    closePipe(p2);
    closePipe(p1);

    char *args4exec[] = {"more", NULL};

    if (execvp(args4exec[0], args4exec) == -1){
      err("err in exelp pager");
    }
      }
    }
  }
  closePipe(p1);
  closePipe(p2);
  waitpid(pid3, NULL, 0);
  return 0;
}
