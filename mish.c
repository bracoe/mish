/*
 * mish.c
 *
 *  Created on: 5 Oct 2018
 *      Author: bram
 */

/* Own inculdes */
#include "parser.h"
#include "execute.h"
#include "list.h"

/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pwd.h>


/* Defines */
#define PRINT_PROMPT fprintf(stderr, "mish%% "); fflush(stderr);

/*Global variable */
struct list *current_mish_children;

/*Function prototypes.*/
int pipe_and_fork_commands(command *command_array, int number_of_commands);
int execute_external_command(command cmd);
void internal_cd(char *dir);
void internal_echo(char **message, int words);
int check_for_internal_commands(command *command_array, int number_of_commands);
void run_internal_commands(command *command_array, int number_of_commands);
void wait_for_children(int num_of_children);
int redirect_extrenal_command(command cmd);
char *get_home_directory();
void setup_signal_handling();
void remove_child_from_list_of_current_children(pid_t complete_child);
void kill_children();

//TODO: handle signal interrupt

void mish_signal_handler(int signo){
	if(signo == SIGINT){
		//printf("Got interrupt sig handle");
		kill_children();
	}

}

void kill_children(){
	list_pos first_pos = list_get_first_position(current_mish_children);
	list_pos current_pos = \
	list_get_previous_position(list_get_last_position(current_mish_children), \
			current_mish_children);

	while(current_pos != first_pos){
		pid_t child_pid = *(pid_t *)list_get_value(current_pos);
		//printf("Child pid: %d", child_pid);
		int ret = kill(child_pid, SIGINT);
		if(ret < 0){
			perror("Killed child process");
		}
		current_pos = list_get_previous_position(current_pos, \
												current_mish_children);
	}
}

/**
 * The main function of the program contains an eternal loop to process the
 * commands given to the mish terminal. This loop can only be terminated using
 * the * signal.
 *
 * @param The amount of commands given to the program.
 * @param An array of the given strings.
 * @return 0, but should never happen.
 */
int main(int argc, char const *argv[]) {

	current_mish_children = list_new();
	setup_signal_handling();

    char input_line[MAXLINELEN+1];
    command command_array[MAXCOMMANDS];

    while(1){ //Main terminal loop only quit due to signal.
        PRINT_PROMPT;

        if(fgets(input_line, MAXLINELEN, stdin) == NULL){
        	if(errno == EINTR){
        		printf("\n");
        		errno = 0; // Clean errno
        		continue;
        	}
        	else if(errno != 0){
        		perror("Reading input");
        		continue;
        	}
        	else{
        		break;
        	}
        }
        int number_of_commands = parse(input_line,command_array);

        if(check_for_internal_commands(command_array, number_of_commands) > 0){
            //printf("Run internal commands!\n");
            run_internal_commands(command_array, number_of_commands);
        }
        else{ //External commands
            //printf("Starting external command commands!\n");
            int ret = pipe_and_fork_commands(command_array, number_of_commands);
            if(ret < 0){
                //TODO: handle it
                break;
            }
            wait_for_children(number_of_commands);
        }
    }
    list_kill(current_mish_children); // should be empty
    return 0;
}

/**
 * wait_for_children() - Makes the mish process wait until the children finish
 * their command which should be executed.
 * @param num_of_children The number of children should be waited on.
 */
void wait_for_children(int num_of_children){
    int num_of_completed_children = 0;
    int status = 0;
    pid_t complete_child;
    while(num_of_completed_children < num_of_children){
        if((complete_child = wait(&status)) < 0){
        	if(errno != EINTR){
        		perror("Wait for child error");
				fprintf(stderr, "Child was %d\n", complete_child);
        	}
        }
        remove_child_from_list_of_current_children(complete_child);
        num_of_completed_children++;
    }
}

void remove_child_from_list_of_current_children(pid_t complete_child){
	list_pos first_pos = list_get_first_position(current_mish_children);
	list_pos current_pos = \
	list_get_previous_position(list_get_last_position(current_mish_children), \
								current_mish_children);

	while(current_pos != first_pos){
		pid_t *pid_in_list = (pid_t*)list_get_value(current_pos);
		if(complete_child == *pid_in_list){
			list_remove_element(current_pos, current_mish_children);
			free(pid_in_list);
			break;
		}
		current_pos = list_get_previous_position(current_pos, \
													current_mish_children);
	}

}

void setup_signal_handling(){
	struct sigaction new_action, old_action;

	  /* Set up the structure to specify the new action. */
	  new_action.sa_handler = mish_signal_handler;
	  sigemptyset (&new_action.sa_mask);
	  new_action.sa_flags = 0;

	  sigaction (SIGINT, NULL, &old_action);
	  if (old_action.sa_handler != SIG_IGN)
	    sigaction (SIGINT, &new_action, NULL);
}

/**
 *
 */
int check_for_internal_commands(command *command_array, int number_of_commands){
    int internal_commands = 0;
    for(int i = 0; i < number_of_commands; i++){
        if((strcmp(command_array[i].argv[0], "cd") == 0) ||
            (strcmp(command_array[i].argv[0], "echo") == 0)){
            internal_commands++;
        }
        else if(internal_commands > 0){
            fprintf(stderr, "Got an internal and external command together!\n");
            return -1; //Something is wrong.
        }
    }
    return internal_commands;
}

/**
 *
 */
void run_internal_commands(command *command_array, int number_of_commands){
    for(int i = 0; i < number_of_commands; i++){
        if(strcmp(command_array[i].argv[0], "cd") == 0){
            internal_cd(command_array[i].argv[1]);
        }
        else{
            internal_echo(command_array[i].argv, command_array[i].argc);
        }
    }
}

/**
 * internal_cd() - Changes the current working directory of the mish terminal.
 * If no directory is given, then the current working directory is change to the
 * home directory of the current process owner. Else if a directory is given,
 * then the working directory is set to the given directory.
 *
 * @param dir The full directory to change to.
 */
void internal_cd(char *dir){
    if(dir == NULL){ //Change dir to homedir if no argument given
        dir = get_home_directory();
        if(dir == NULL){
            fprintf(stderr, "Could not get home directory...\n");
            return;
        }
    }

    int ret = chdir(dir);
    if(ret < 0){
        perror("Internal cd");
    }
}

/**
 * get_home_directory() - Gets the home directory of the current process owner.
 *
 * @return The current home directory.
 */
char *get_home_directory(){
    struct passwd *pw = getpwuid(getuid());
    if(pw == NULL){
        perror("Get home dir");
    }
    char *dir = pw->pw_dir;
    return dir;
}

/**
 * internal_echo() - prints the given massage to the standard output of the
 * mish terminal. The first word in the array is skipped.
 *
 * @param message A string array containing the message which should be printed.
 * @param words The amount of words in the array which should be printed.
 */
void internal_echo(char **message, int words){

    //Print all words except for the last one with a blankspace
    for(int i = 1; i < words-1; i++){
        int ret = printf("%s ",message[i]);
        if(ret < 0){
            perror("Internal echo:");
        }
    }

    //Print last word without blanksapce
    int ret = printf("%s\n",message[words-1]);
    if(ret < 0){
        perror("Internal echo");
    }
}

/**
 *
 */
int pipe_and_fork_commands(command *command_array, int number_of_commands){

    int in_pipe[2];
    int out_pipe[2];

    //Create pipe
    for(int i = 0; i < number_of_commands; i++){
        //Fork the command(s)
    	if(i < number_of_commands-1){
    		int ret = pipe(out_pipe);
			if (ret == -1) {
				perror("Pipe");
				return -1;
			}
    	}

        pid_t pid = fork();
        if ( pid < 0 ) {
            perror("fork");
            exit(1);

        } else if ( pid == 0 ) { //Child process
            //printf("Child: externals redirected!\n");
        	int ret = 0;
        	if(i != 0){
				//printf("Child: changing stdin!\n");
				ret = dupPipe(in_pipe, READ_END, STDIN_FILENO);
				if(ret < 0){
					return -1;
				}
				ret = close(in_pipe[WRITE_END]);
				if(ret < 0){
					perror("Closing pipe write end");
				}
			}
        	if(i != number_of_commands-1){
				//printf("Child: changing stdout!\n");
				ret = dupPipe(out_pipe, WRITE_END, STDOUT_FILENO);
				if(ret < 0){
					return -1;
				}
				ret = close(out_pipe[READ_END]);
				if(ret < 0){
					perror("Closing pipe read end");
				}
			}

            execute_external_command(command_array[i]);
            return -1; //Should never happen

        } else {
            // Parentprocess
        	if(i != 0){
        		int ret = close(in_pipe[READ_END]);
				if(ret < 0){
					perror("Closing pipe");
				}
				ret = close(in_pipe[WRITE_END]);
				if(ret < 0){
					perror("Closing pipe");
				}
			}
        	in_pipe[0] = out_pipe[0];
        	in_pipe[1] = out_pipe[1];

        	int *child_pid = malloc(sizeof(int));
        	*child_pid = pid;
        	list_append(child_pid, current_mish_children);
        }

    }
    return number_of_commands;
}

/**
 *
 */
int execute_external_command(command cmd){
	//printf("Child: pipes connected!\n");
	if(redirect_extrenal_command(cmd) < 0){
		fprintf(stderr, "Could not redirect for %s\n", cmd.argv[0]);
		return -1;
	}
    //printf("Child: Running process: %s", cmd.argv[0]);
    int ret = execvp(cmd.argv[0],cmd.argv);
    if(ret < 0){
        perror(cmd.argv[0]);
    }
    return 0;
}

/**
 * redirect_extrenal_command() - Redirects the input or output of the the
 * command. The if the input or output of the command struct, is not NULL, then
 * it will be redirected.
 *
 * @param cmd the external command which should be redirected.
 * @return 0 on success or -1 on failure.
 */
int redirect_extrenal_command(command cmd){
    //Check if input/output has to be redirected
    if(cmd.infile != NULL){
        //printf("Child: inputfile: %s\n", cmd.infile);
        int ret = redirect(cmd.infile, O_RDONLY, STDIN_FILENO);
        if(ret < 0){
            fprintf(stderr, "Child could not redirect infile!\n");
            return -1;
        }
    }
    if(cmd.outfile != NULL){
        //printf("Child: outputfile: %s\n", cmd.outfile);
        int ret = redirect(cmd.outfile, O_WRONLY | O_CREAT, STDOUT_FILENO);
        if(ret < 0){
            fprintf(stderr, "Child could not redirect outfile!\n");
            return -1;
        }
    }
    return 0;
}
