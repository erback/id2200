#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define WRITE 1
#define READ 2

int p1[2];
/*bool exitShell;*/



void prompt() {
  
  printf("\n %s >> ", "Woohoo");
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


int main(int argc, char **argv){
  pid_t  pid;
  size_t size = 100;
  char * input = "";
  char * memory = malloc(size + 1);
  char * args[2];
  char cwd[1024];
   
  
  while((strcmp(input, "exit")) != 0){
    prompt();

    /*Get the current directory adds it to the cwd*/

    getcwd(cwd, sizeof(cwd));

    /*Red the input from the command line*/

    input = (char *) malloc (size + 1);
    getline(&input, &size, stdin);
    
    /*Get the fist entry*/ 
    args[0] = strtok(input, "\n\t");
    args[1] = strtok(input, "\n\t");
    
    getcwd(cwd, sizeof(cwd));
    fprintf(stderr, "%s\n", args[1]);
    /* First check if user entered exit in the command promt*/
    if (strcmp(input, "exit") == 0){
      printf("Tack så mycket för denna gång.. Välkommen åter!\n");
    }

    /*Then check if the change directory command was entered*/
    else if (strcmp(input, "cd") == 0){
        
        
        fprintf(stderr, "hej%s\n", args[1]);



    }

    else {


      
      args[1] = cwd;
      args[2] = NULL;
      
      /*Save this for later use*/

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
        printf("pipe: %s\n", hmm);
      }
    }
  }
}
