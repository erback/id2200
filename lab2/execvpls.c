#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

#define WRITE 1
#define READ 2

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

/*void  tokenizeInput(char *input){
  i = 0;
  char * tokenList [5];
  while(input != NULL) {
    printf("Testing %s\n", input);

    tokenList[i] = strtok(NULL, " ");
    i ++;
  }

  return tokenList;

}*/

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



int main(int argc, char **argv){
  pid_t  pid;
  size_t size = 256;
  size_t i;
  char * input = "";
  char * memory = malloc(size + 1);
  char * args[16];
  char  cwd[1024];
  bool runBg;


  while((strcmp(input, "exit")) != 0){

    memset(args, 0, 16);

    /*Get the current directory adds it to the cwd*/

    getcwd(cwd, sizeof(cwd));


    /*Printing the command promt*/
    prompt(cwd);


    /*Read the input from the command line*/

    input = (char *) malloc (size + 1);
    getline(&input, &size, stdin);

    /*Get first command element*/
    args[0] = strtok(input, " \n\t");




    if ( NULL == args[0] ) /* If no value was entered print the commandpromt again*/
      continue;


    /* Check if user wants to exit the promt*/
    else if (strcmp(args[0], "exit") == 0){
      printf("Tack så mycket för denna gång.. Välkommen åter!\n");
      break;
    }


    /*Then check if the change directory command was entered*/
    else if (0 == strcmp(args[0], "cd")){

          /*get the next argument*/
          args[1] = strtok(NULL, " \n\t");

          if (NULL == args[1]){
            if (changeDir(getenv("HOME")) == false)
              errHandler("Could not change directory");
            }

          else{
            if (false == changeDir(args[1]))
              errHandler("Could not change directory");
          }
          continue;
        }


    else{
        bool and = false;
        /* Parse the rest of the arguments*/
        i = 1;
        for(1; i ++; i<16){
          args[i] = strtok(NULL, " \n\t");
          if (args[i] == NULL){
            if (args[1] != NULL && ){
              bool and = &( args[i-1][strlen(args[i-1])-1] ) ? : true;
            }
            break;
          }
        }



        /* Determine if it should be run as background or foregorund process, default is runBg = false*/



        if (and){
          printf("%s\n", "hej");
        }


        /*Create piṕe to execute command line actions and arguments*/
          if(pipe(p1)== -1){
              perror("pipe1");
            }

          pid = fork();
          if (0 > pid){
            printf("ERROR");
          }
          else if(0 == pid){

            if (execvp(args[0], args) < 0){
              printf("That is not a valid command... Try another one such as ls or cd");
                exit(1);
                }
        char bytut[] = "bytut";
        close(p1[0]);
        write(p1[1], bytut, strlen(bytut) + 1);
        close(p1[1]);
      }
      else{
        char hmm[100];
        close(p1[1]);
        read(p1[0], hmm, sizeof(hmm));
        close(p1[0]);
      }

    }

  }
}











