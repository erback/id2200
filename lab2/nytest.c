#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>


#define MAXINPUT 71
#define MAXARG 6
#define RUNNING 1
#define	CLOSING 0

void prompt(char *cwd);
void changeDirectory(char *arguments);
bool checkProcessType(char *arguments);
void executeCommand(bool bgProcess, char **arguments);
pid_t startFork();
void regSigHandler(int sigCode, void (*handler)(int sig));
void sigHandler(int sigCode);


int main(){
	int programStatus = RUNNING;
	char cwd[1024];

	while(programStatus){
		char input[MAXINPUT];
	  	char **arguments = (char **) calloc (MAXARG,  MAXINPUT);
	  	int length = 0;

	  	getcwd(cwd, sizeof(cwd));
		prompt(cwd);
		fgets(input, MAXINPUT, stdin);
	    char *args = strtok(input, " \n\n");


	    while( args != NULL){
	      arguments[length] = args;
	      args = strtok(NULL, " ");
	      length++;
	    }

	    if (NULL == arguments[0]){
	    	printf("Du måste skriva in något...\n");
	    }
	    else if (strcmp(arguments[0], "&") == 0){
	    	printf("Du får inte bara skicka ett &...");
	    }
	    else if (strcmp(arguments[0], "exit") == 0){
	    	printf("Stänger ner kommandotolken..\n");
	    	programStatus = CLOSING;
	    }
	    else if (strcmp(arguments[0], "cd") == 0){
            changeDirectory(arguments[1]);
          	free(arguments);
	    }
	    else{
	    	bool bgProcess = checkProcessType(arguments[length-1]);
	    	if (bgProcess){
	    		if (strcmp(arguments[length-1], "&") == 0){
	    			arguments[length-1] = NULL;
	    		}
	    		else{
	    			strtok(arguments[length-1], "&");
	    		}
	    	}
	    	executeCommand(bgProcess, arguments);
	    	free(arguments);
	    }
	}
}

void prompt(char* cwd) {

  printf("\n %s >> ", cwd);
  fflush(stdout);
}

void changeDirectory(char * arguments){
	if (NULL == arguments){
  		char *homeDir = getenv("HOME");
    	chdir(homeDir);
    }

  	else{
  		strtok(arguments, "\n");
    	if(0 != chdir(arguments)){
    		printf("Angiven plats finns inte");
    	}
  	}
}

bool checkProcessType(char *argument){
	strtok(argument, "\n");
	int argLen = strlen(argument);
	if (strcmp(&argument[argLen -1], "&") == 0 || strcmp(argument, "&") == 0){
		return true;
	}
	else{
		return false;
	}
}

void executeCommand(bool bgProcess, char **arguments){
	clock_t start, end;
	double elapsed;
	start = clock();
	pid_t childProcess = startFork();
	if (0 == childProcess){
		printf("Inne i child");
		regSigHandler(SIGINT, sigHandler);
		execvp(arguments[0], arguments);
		perror("Nu va det något galet med det du skrev in...");
		exit(1);
	}
	if (bgProcess){
		printf("Bakgrundsprocess med process id: %i\n", childProcess);
	}
	else{

		printf("Förgrundsprocess med process id: %i\n", getpid());
		end = clock();
		elapsed = ((double) (end-start)) / CLOCKS_PER_SEC;
        fprintf(stderr, "Time elapsed for process: %f s\n", elapsed);

	}
}

pid_t startFork(){
	pid_t child_pid = fork();
	if(child_pid < 0){
		printf("Fel vid skapande av process");
		exit(1);
	}
	return child_pid;
}


void regSigHandler(int sigCode, void (*handler)(int sig)){
	int retValue;
	struct sigaction sigParams;
	sigParams.sa_handler = handler;
	sigemptyset( &sigParams.sa_mask );
	sigParams.sa_flags = 0;
	retValue = sigaction(sigCode, &sigParams, (void *) 0);

	if (-1 == retValue){
		perror("sigation failure.. dåligt");
		exit(1);
	}
}

void sigHandler(int sigCode){
	printf("hej");

	if (SIGINT == sigCode){
		printf("%d",sigCode);
	}

}
