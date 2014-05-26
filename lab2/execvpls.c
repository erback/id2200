
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#define _GNU_SOURCE
#define WRITE 1
#define READ 2
#define BUFFERSIZE 71
#define ARGVSIZE 6

int p1[2];
/*bool exitShell;*/



void prompt(char* cwd) {

  printf("\n %s >> ", cwd);
  fflush(stdout);
}
void closePipe(int pipeEnd[2]){
  if (close(pipeEnd[WRITE]) == -1){
      perror("error when closing pipe, read");
  }
  if (close(pipeEnd[READ]) == -1){
      perror("error when closing pipe, write");
  }
}



void errHandler(char * msg){
  perror(msg);
}

bool changeDir(char * path){


  if (path == NULL)
    return false;

  else if (chdir(path) == 0){
    return true;
  }
  else
    return false;

}

pid_t startFork(){

  pid_t pid = fork();
  if (pid == -1)
    errHandler("Fork error");
  return pid;
}



void cleanZoombie(int signal) {
    wait(&signal);
    printf("Zoombie process %i was cleaned up", signal);
}

bool checkBG(char * args[]){

}



void run(char ** args){

        bool runBg = false;

        clock_t start, end;
        pid_t  pid;
        double elapsed;



        int i=0;

      while(args[i] != NULL){
          i++;
          printf("%i\n", i);
        }

      if (strcmp(&(args[i-1][strlen(args[i-1])-1]), "&") == 0)
        runBg = true;
      else
        runBg =false;

      if(runBg)
          printf("%s\n", "bg is runnign");


      if (runBg){
          if(pipe(p1)== -1)
              perror("Pipe creation error");
        }

        //Starting the timer
            start = clock();

            //printf("%s\n", start);

        /*Create pipe to execute command line actions and arguments*/


          pid = startFork();


          if(0 == pid){
            if (runBg){
                if (dup2(p1[STDIN_FILENO],STDIN_FILENO)== -1)
                    errHandler("dup2 error");
                  closePipe(p1);
            }

            if (execvp(args[0], args) < 0)
                errHandler("That is not a valid command..");


          }

          if (runBg){

            closePipe(p1);
          }

          else{

            waitpid(pid, NULL, 0);

            end = clock();
            elapsed = ((double) (end-start)) / CLOCKS_PER_SEC;
            fprintf(stderr, "Time elapsed for process: %f s\n", elapsed);
            printf("%i\n", pid);

}

}



int main(int argc, char **argv){
  bool programStatus = true;
  char  cwd[1024];
  char input[BUFFERSIZE];
  char **arguments = (char **) calloc (ARGVSIZE,  BUFFERSIZE);
  //signal(SIGCHLD,cleanZoombie);


  while(programStatus){

    //memset(args, 0, 16);


    /*Get the current directory adds it to the cwd*/

    getcwd(cwd, sizeof(cwd));


    /*Printing the command promt*/
    prompt(cwd);


    /*Read the input from the command line*/


    fgets(input, BUFFERSIZE, stdin);
    char *args = strtok(input, " \n");
    int length = 0;
    while( args != NULL){ /* Read until NULL */
      arguments[length] = args; /* Point to the input */
      args = strtok(NULL, " "); /* Move on */
      length++;
    }

    if ( NULL == arguments[0] ) /* If no value was entered print the commandpromt again*/
      continue;


    /* Check if user wants to exit the promt*/
    else if (strcmp(arguments[0], "exit") == 0){
      printf("Tack så mycket för denna gång.. Välkommen åter!\n");
      programStatus = false;
    }


    /*Then check if the change directory command was entered*/
    else if (0 == strcmp(arguments[0], "cd")){

          /*get the next argument*/
          arguments[1] = strtok(NULL, " \n\t");

          if (NULL == arguments[1]){
            if (changeDir(getenv("HOME")) == false)
              errHandler("Could not change directory");
            }

          else{
            if (false == changeDir(arguments[1]))
              errHandler("Could not change directory");
          }
          continue;
        }


    else{

      printf("%s\n", "hej");
      run(arguments);


      }

  }

}












