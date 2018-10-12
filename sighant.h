/*
 * signhant.h Is the header file for the signalhandler of mish. Here the
 * sigaction struct is setup to catch an interrupt and pass it on to the
 * children of the shell. The pids of these children are assumed to be saved
 * in the current_shell_children list.
 *
 *  Created on: 10 Oct 2018
 *      Author: Bram Coenen (tfy15bcn)
 */

#ifndef SIGHANT_H_
#define SIGHANT_H_

#include "list.h"

/*Global variable for the list where the childrens pids should be saved.*/
struct list *current_shell_children;


/**
 * shell_signal_handler() - The handler which the signal will be passed along to
 *  by sigaction. If the signal is an interrupt, a function will be call which
 *  will kill all the child processes. Else the default action should occur.
 *
 *  @param signo The identifier of the signal.
 */
void shell_signal_handler(int signo);

/**
 * setup_signal_handling() - Sets up sigaction so the interrupt signal will be
 * sent to the handler. The default action should occur for all the other
 * signals.
 */
void setup_signal_handling(void);

/**
 * kill_children() - Steps through the entire list with the pids of the
 * childprocesses and sends an interrupt signal.
 */
void kill_children(void);

#endif /* SIGHANT_H_ */
