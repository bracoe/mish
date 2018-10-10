/*
 * signhant.h
 *
 *  Created on: 10 Oct 2018
 *      Author: bram
 */

#ifndef SIGHANT_H_
#define SIGHANT_H_

#include "list.h"

/*Global variable */
struct list *current_shell_children;

void kill_children(void);
void shell_signal_handler(int signo);
void setup_signal_handling(void);


#endif /* SIGHANT_H_ */
