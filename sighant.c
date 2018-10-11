/*
 * sighant.c
 *
 *  Created on: 10 Oct 2018
 *      Author: bram
 */

/* Include own header */
#include "sighant.h"

/*Include default libraries */
#include <stdio.h>
#include <signal.h>
#include <errno.h>


/**
 * shell_signal_handler() - The handler which the signal will be passed along to
 *  by sigaction. If the signal is an interrupt, a function will be call which
 *  will kill all the child processes. Else the default action should occur.
 *
 *  @param signo The identifier of the signal.
 */
void shell_signal_handler(int signo){
	if(signo == SIGINT){
		kill_children();
	}

}

/**
 * setup_signal_handling() - Sets up sigaction so the interrupt signal will be
 * sent to the handler. The default action should occur for all the other
 * signals.
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
