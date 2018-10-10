/*
 * sighant.c
 *
 *  Created on: 10 Oct 2018
 *      Author: bram
 */

#include "sighant.h"

#include <stdio.h>
#include <signal.h>
#include <errno.h>
/**
 *
 */
void shell_signal_handler(int signo){
	if(signo == SIGINT){
		kill_children();
	}

}

/**
 *
 */
void setup_signal_handling(void){
	struct sigaction new_action, old_action;

	  /* Set up the structure to specify the new action. */
	  new_action.sa_handler = shell_signal_handler;
	  sigemptyset (&new_action.sa_mask);
	  new_action.sa_flags = 0;

	  sigaction (SIGINT, NULL, &old_action);
	  if (old_action.sa_handler != SIG_IGN)
	    sigaction (SIGINT, &new_action, NULL);
}

/**
 *
 */
void kill_children(void){
	list_pos first_pos = list_get_first_position(current_shell_children);
	list_pos current_pos = \
	list_get_previous_position(list_get_last_position(current_shell_children), \
			current_shell_children);

	while(current_pos != first_pos){
		pid_t child_pid = *(pid_t *)list_get_value(current_pos);
		//printf("Child pid: %d", child_pid);
		int ret = kill(child_pid, SIGINT);
		if(ret < 0){
			perror("Killed child process");
		}
		current_pos = list_get_previous_position(current_pos, \
												current_shell_children);
	}
}
