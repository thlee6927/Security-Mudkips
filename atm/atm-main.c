/* 
 * The main program for the ATM.
 *
 * You are free to change this as necessary.
 */

#include "atm.h"
#include <stdio.h>
#include <stdlib.h>

static const char prompt[] = "ATM: ";
extern char *currentUser = NULL;

int main()
{
    char user_input[1000];

    ATM *atm = atm_create();

    printf("%s", prompt);
    fflush(stdout);

    while (fgets(user_input, 10000,stdin) != NULL)
    {
        atm_process_command(atm, user_input);

        if(currentUser != NULL) {
            printf("ATM (%s): \n", currentUser);
        }
        else {
            printf("%s", prompt);
        }
        
        fflush(stdout);
    }
	return EXIT_SUCCESS;
}
