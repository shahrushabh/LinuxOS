//Assumption: The shell is built case sensitive, i.e. exit != EXIT.
//Make file is included. In case its is insufficient to run,
//Compilation: gcc -c project1.c
//	       gcc -o project1 project1.c
//Running:     ./project1
//Optional:    ./project1 -p "Prompt"

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>


char command[1024];			// The complete command received from shell.
const char delim[2] = " ";		// Delimeter to parse command for processsing.
char *parsedCommand = NULL;		// Parsed commnand.


void interpretCommand(char *c);			// Interprets the command reveiced from shell.
void otherCommands(char *nbCommand);		// Run non built-in commands.

// Main Function.
int main(int argc, char **argv) {
	char *prompt = "308sh> ";
	char *parsedCommand = NULL;
	if((argc > 1) && (strcmp("-p",argv[1]) == 0 )) {
		prompt = argv[2];
	}
	
	//Accet Commands from user.
	while(1) {
		printf("%s",prompt);
		fgets(command,1024,stdin);
	// Replace the new line character with null character for string processing.
		if(command[0] != '\n') {
			command[strlen(command)-1] = '\0';
			parsedCommand = strtok(command,delim);
			interpretCommand(parsedCommand);
		}
	}

	return 0;
}


// Interprets the user command received from shell.
void interpretCommand(char *c) {
	if(strcmp("exit",c) == 0) {
		exit(0);
	}
	else if(strcmp("pid",c) == 0) {
		printf("Process ID is: %d\n",getpid());
	}
	else if(strcmp("ppid",c) == 0) {
		printf("Parent Process ID is: %d\n",getppid());
	}
	else if(strcmp("pwd",c) == 0) {
		char path[64];
		getcwd(path,63);
		printf("%s\n",path);
	}
	else if(strcmp("cd",c) == 0) {
		parsedCommand = strtok(NULL,delim);
		chdir(parsedCommand);
	// Present user an error message for non existing file/directory. 
		if(errno == ENOENT) {
			perror("Error");
			printf("Error Code: %d\n",errno);
			errno = 0;
		}
	}
	else if(strcmp("get",c) == 0) {
		parsedCommand = strtok(NULL,delim);
		printf("%s\n",getenv(parsedCommand));
	}
	else if(strcmp("set",c) == 0) {
		char *value = strtok(NULL,delim);
		if(strcmp("",parsedCommand) == 0) {
			printf("Too few arguments to set\n");
			return;
		}
		else {
			if(setenv(parsedCommand,value,0) != 0) {
				perror("Error");
			}
			return;
		}
	}
	else {
	// Go to non built-in commands.
		otherCommands(c);
	}
}


// Check PATH variables for additional commads to be executed.
void otherCommands(char *nbCommand) {
	pid_t p;
	p = fork();
	int status;
        char*params[5];
        int i=1;
        do{
	        params[i] = strtok(NULL,delim);
        	i++;
        } while(params[i-1] != NULL);
        params[0] = nbCommand;

	if(p < 0) {
                perror("Error");
		printf("Error code: %d",errno);
		return;
	}
	// Wait for child process to complete.
	else if(p != 0) {
		waitpid(-1,&status,0);
	}
	else if(p == 0){
		printf("Child process is: %s\nProcess ID is %d\n",nbCommand,getpid());
		execvp(nbCommand, params);
		perror("Execv");
	}
}

