#include "atm.h"
#include "ports.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

extern char *currentUser = NULL; //to see if there's already a user logged in

ATM* atm_create()
{
    ATM *atm = (ATM*) malloc(sizeof(ATM));
    if(atm == NULL)
    {
        perror("Could not allocate ATM");
        exit(1);
    }

    // Set up the network state
    atm->sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&atm->rtr_addr,sizeof(atm->rtr_addr));
    atm->rtr_addr.sin_family = AF_INET;
    atm->rtr_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    atm->rtr_addr.sin_port=htons(ROUTER_PORT);

    bzero(&atm->atm_addr, sizeof(atm->atm_addr));
    atm->atm_addr.sin_family = AF_INET;
    atm->atm_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    atm->atm_addr.sin_port = htons(ATM_PORT);
    bind(atm->sockfd,(struct sockaddr *)&atm->atm_addr,sizeof(atm->atm_addr));

    // Set up the protocol state
    // TODO set up more, as needed

    return atm;
}

void atm_free(ATM *atm)
{
    if(atm != NULL)
    {
        close(atm->sockfd);
        free(atm);
    }
}

ssize_t atm_send(ATM *atm, char *data, size_t data_len)
{
    // Returns the number of bytes sent; negative on error
    return sendto(atm->sockfd, data, data_len, 0,
                  (struct sockaddr*) &atm->rtr_addr, sizeof(atm->rtr_addr));
}

ssize_t atm_recv(ATM *atm, char *data, size_t max_data_len)
{
    // Returns the number of bytes received; negative on error
    return recvfrom(atm->sockfd, data, max_data_len, 0, NULL, NULL);
}

void atm_process_command(ATM *atm, char *command)
{
    // TODO: Implement the ATM's side of the ATM-bank protocol

	/*
	 * The following is a toy example that simply sends the
	 * user's command to the bank, receives a message from the
	 * bank, and then prints it to stdout.
	 */

	/*
    char recvline[10000];
    int n;

    atm_send(atm, command, strlen(command));
    n = atm_recv(atm,recvline,10000);
    recvline[n]=0;
    fputs(recvline,stdout);
	*/

    char *token = strtok(command, " ");
    char *invalid;

    if((strcmp("begin-session", token)) == 0) {
        char *username = strtok(NULL, " "); //gets user name (aka next token)

        invalid = strtok(NULL, " "); //checks to make sure no extra words afterwards

        if((strlen(username) > 250) || (isChar(username, strlen(username)) == 0) || (invalid != NULL)) {
            printf("Usage: begin-session <user-name>\n");
        } //invalid inputs
        else if(strlen(currentUser) != 0) {
            printf("A user is already logged in\n");
        } //already user logged in
        else if() {

            //IF NO SUCH USERNAME EXISTS WITH THE BANK

        }
        else {
            char *fileName = username + ".card"; //formats to the file name
            char *pinInput = NULL;

            if (fopen(fileName, "r") != NULL) {



                //Need to read in hash to PIN in order to verify if pinInput is correct




                printf("PIN? ");

                fgets(pinInput, 10000, stdin);

                if(strlen(pinInput) != 4) || (isNum(pinInput, strlen(pinInput)) == 0) || (strcmp(pinInput, ????????))) {
                    printf("Not authorized\n");
                    printf("ATM: ");
                }
                else {
                    printf("Authorized\n");
                    printf("ATM (" + username + "): ");

                    currentUser = username; //set current user
                }
            }
            else {
                printf("Unable to access " + username + "'s card\n");
            }
        }        
    }
    else if((strcmp ("withdraw", token)) == 0) {
        char *amount = strtok(NULL, " ");

        invalid = strtok(NULL, " ");

        if(strlen(currentUser) == 0) {
            printf("No user logged in\n");
        }
        else {
            if((isNum(amount, strlen(amount)) == 0) || (invalid != NULL) || (atoi(amount) > INT_MAX)) {
                printf("Usage: withdraw <amt>\n");
            } //invalid inputs
            else {



                //NEED TO TALK TO ROUTER/BANK TO GET BALANCE OF USER



                if(amount > balance from above) {
                    printf("Insufficient funds\n");
                }
                else {
                    printf("$" + amount + " dispensed\n");


                    //REDUCE BALANCE ACCORDINGLY IN BANK

                } //sufficient funds in bank
            } //valid inputs
        } //user logged in

    }
    else if((strcmp ("balance", token)) == 0) {
        invalid = strtok(NULL, " ");

        if(strlen(currentUser) == 0) {
            printf("No user logged in\n");
        }
        else {
            if(invalid != NULL) {
                printf("Usage: balance\n");
            }
            else {


                //GET BALANCE FROM BANK

                printf("$" + balance + "\n");
            }
        }
    }
    else if((strcmp ("end-session", token)) == 0){
        invalid = strtok(NULL, " ");

        if(strlen(currentUser) == 0) {
            printf("No user logged in\n");
        }
        else {
            if(invalid != NULL) {
                printf("Invalid command\n");
            }
            else {
                currentUser = NULL;

                //TERMINATE CURRENT SESSION???????

                printf("User logged out\n");
            }
        }
    }
    else {
        printf("Invalid command\n");
    }

}

/*Checks if a string contains only [a-zA-Z+]*/
int isChar(char *str, int len){
    int i = 0;
    for(i = 0; i < len; i++){
        char c = str[i];
        if((97 <= c && 122 >= c) || (65 <= c && 90 >= c))
            continue;
        else
            return 0;
    }
    return 1;
}

/*Checks if a string only contains [0-9]*/
int isNum(char *str, int len){
    int i = 0;
    for(i = 0; i < len; i++){
        char c = str[i];
        if(48 <= c && 57 >= c)
            continue;
        else
            return 0;
    }
    return 1;
}