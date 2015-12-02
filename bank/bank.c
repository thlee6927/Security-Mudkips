#include "bank.h"
#include "ports.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/evp.h>

Bank* bank_create()
{
    Bank *bank = (Bank*) malloc(sizeof(Bank));
    if(bank == NULL)
    {
        perror("Could not allocate Bank");
        exit(1);
    }

    // Set up the network state
    bank->sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&bank->rtr_addr,sizeof(bank->rtr_addr));
    bank->rtr_addr.sin_family = AF_INET;
    bank->rtr_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    bank->rtr_addr.sin_port=htons(ROUTER_PORT);

    bzero(&bank->bank_addr, sizeof(bank->bank_addr));
    bank->bank_addr.sin_family = AF_INET;
    bank->bank_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    bank->bank_addr.sin_port = htons(BANK_PORT);
    bind(bank->sockfd,(struct sockaddr *)&bank->bank_addr,sizeof(bank->bank_addr));

    // Set up the protocol state
    // TODO set up more, as needed

    OpenSSL_add_all_digests();
    bank->clientHead = NULL;

    return bank;
}

void bank_free(Bank *bank)
{
    if(bank != NULL)
    {
        close(bank->sockfd);
        free(bank);
    }
}

ssize_t bank_send(Bank *bank, char *data, size_t data_len)
{
    // Returns the number of bytes sent; negative on error
    return sendto(bank->sockfd, data, data_len, 0,
                  (struct sockaddr*) &bank->rtr_addr, sizeof(bank->rtr_addr));
}

ssize_t bank_recv(Bank *bank, char *data, size_t max_data_len)
{
    // Returns the number of bytes received; negative on error
    return recvfrom(bank->sockfd, data, max_data_len, 0, NULL, NULL);
}

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

Node *get_client(Node *head, char *nom){
    if(head == NULL)
        return NULL;
    if(strcmp(head->name, nom) == 0)
        return head;
    return get_client(head->next, nom);
}

int num_clients(Node *head){
    if(head == NULL)
        return 0;
    return 1 + num_clients(head->next);
}

int compHash(uint8_t *hash1, uint8_t *hash2){
    int i = 0;
    for(i = 0; i < 32; i++){
        if(*(hash1 + i) != *(hash2 + i))
            return 0;
    }
    return 1;
}

void bank_process_local_command(Bank *bank, char *command, size_t len)
{
    // TODO: Implement the bank's local commands
    char *line = strtok (command," ");

    if(line != NULL){
        if(strcmp(line, "create-user") == 0){
            char *name = strtok(NULL, " ");
            char *pinStr = strtok(NULL, " ");
            char *balanceStr = strtok(NULL, " ");
            char *check = strtok(NULL, " ");
            if(name == NULL || pinStr == NULL || balanceStr == NULL || check != NULL)
                printf("Usage: create-user <user-name> <pin> <balance>\n");
            else{
                if(strlen(name) > 250 || strlen(pinStr) != 4 || isChar(name, strlen(name)) == 0 || isNum(pinStr, 4) == 0 || isNum(balanceStr, strlen(balanceStr)) == 0){
                    printf("Usage: create-user <user-name> <pin> <balance>\n");
                }
                else{
                    unsigned int balance = strtol(balanceStr, NULL, 10);
                    char asdf[99];
                    sprintf(asdf, "%d",balance);
                    if(strlen(asdf) != strlen(balanceStr))
                        printf("Usage: create-user <user-name> <pin> <balance>\n");
                    else{
                        Node *user = get_client(clientHead, name);
                        if(user == NULL){
                            Node *put = malloc(sizeof(Node));
                            File *file;
                            char *filename;

                            sprintf(filename, "%s.card", name);
                            file = fopen(filename, "w+");

                            if(file != NULL){
                                EVP_MD_CTX *ctx = EVP_MD_CTX_create();
                                uint8_t thehash[32];
                                int hashlen;
                                uint32_t namelen = strlen(name);
                                unsigned char *data = malloc(4 + namelen + 32);
                                put->name = malloc(strlen(name) + 1);
                                put->PIN = malloc(4);
                                strcpy(put->name, name, strlen(name));
                                memcpy(put->PIN, pinStr, 4);
                                put->balance = balance;
                                put->next = bank->clientHead;
                                if(bank->clientHead != NULL)
                                    bank->clientHead->prev = put;
                                put->prev = NULL;
                                bank->clientHead = put;

                                EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);

                                EVP_DigestUpdate(ctx, pinStr, 4);
                                EVP_DigestFinal_ex(ctx, thehash, &hashlen);

                                memcpy(data, &namelen, 4);
                                memcpy(data + 4, name, namelen);
                                memcpy(data + 4 + namelen, thehash, 32);

                                fwrite(data, 1, namelen + 36, file);
                                fclose(file);

                                printf("Created user %s\n", name);
                                EVP_MD_CTX_destroy(ctx);
                            }
                            else
                                printf("Error creating card file for user %s\n", name);

                        }
                        else{
                            printf("Error: user %s already exists\n", name);
                        }
                    }
                }
            }
        }
        else{
            if(strcmp(line, "deposit") == 0){
                char *name = strtok(NULL, " ");
                char *amtStr = strtok(NULL, " ");
                char *check = strtok(NULL, " ");
                if(name == NULL || amtStr == NULL || check != NULL)
                    printf("Usage: deposit <user-name> <amt>\n");
                else{
                    if(strlen(name) > 250 || isChar(name, strlen(name)) == 0 || isNum(amtStr, strlen(amtStr)) == 0){
                        printf("Usage: deposit <user-name> <amt>\n");
                    }
                    else{
                        unsigned int amt = strtol(amtStr, NULL, 10);
                        char asdf[99];
                        sprintf(asdf, "%d",amt);
                        if(strlen(asdf) != strlen(amtStr))
                            printf("Usage: deposit <user-name> <amt>\n");
                        else{  
                            Node *user = get_client(bank->clientHead, name);
                            if(user == NULL){
                                printf("No such user\n");
                            }
                            else{
                                if(user->balance + amt < user->balance)
                                    printf("Too rich for this program\n");
                                else{
                                    user->balance = user->balance + amt;
                                    printf("$%i added to %s's account\n", amt, name);
                                }
                            }
                        }
                    }
                }

            }
            else{
                if(strcmp(line, "balance") == 0){
                    char *name = strtok(NULL, " ");
                    char *check = strtok(NULL, " ");
                    if(name == NULL || check != NULL)
                        printf("Usage: balance <user-name>\n");
                    else{
                        if(strlen(name) > 250 || isChar(name, strlen(name)) == 0){
                            printf("Usage: balance <user-name>\n");
                        }
                        else{
                            Node *user = get_client(bank->clientHead, name);
                            if(user == NULL){
                                printf("No such user\n");
                            }
                            else{
                                printf("$%i\n", user->balance);
                            }
                        }
                    }

                }
                else{
                    printf("Invalid command\n");
                }
            }
        }

    }
    else{
        printf("Invalid command\n");
    }

}

void bank_process_remote_command(Bank *bank, char *command, size_t len)
{
    // TODO: Implement the bank side of the ATM-bank protocol
    uint8_t opcode = *((uint8_t *) command);

    if(opcode == 1){
        int namelen;
        char *name;
        Node *user = NULL;
        uint8_t *sendback = malloc(2);

        namelen = *((uint32_t *)(command+1));
        namelen = ntohl(namelen);

        name = malloc(namelen + 1);

        memcpy(name, command+5, namelen);

        name[namelen] = 0;

        user = get_client(bank->clientHead, name);

        sendback[0] = 1;

        if(user == NULL){
            sendback[1] = 0;
        }
        else
            sendback[1] = 1;

        bank_send(bank, sendback, 2);
        free(sendback);
    }
    else{
        uint8_t *mesHash = command + len-32;
        int messlen = len-33
        char errM = {0,0};
        if(messlen > 5){
            int namelen;
            char *name;
            uint8_t *data = malloc(messlen);
            Node *user = NULL;
            memcpy(data, command+1, messlen);

            namelen = *((uint32_t *)data);
            namelen = ntohl(namelen);

            name = malloc(namelen + 1);

            memcpy(name, data+4, namelen);

            name[namelen] = 0;

            user = get_client(bank->clientHead, name);

            if(user != NULL){
                EVP_MD_CTX *hashctx = EVP_MD_CTX_create();
                uint8_t thehash[32];
                int hashlen;

                EVP_DigestInit_ex(hashctx, EVP_sha256(), NULL);

                EVP_DigestUpdate(hashctx, user->PIN,4);
                EVP_DigestUpdate(hashctx, data, messlen);
                EVP_DigestFinal_ex(hashctx, thehash, &hashlen);
                EVP_MD_CTX_destroy(hashctx);

                if(compHash(thehash, mesHash) == 1){
                    if(opcode == 2){
                        uint8_t *sendback = malloc(2);
                        uint32_t with = *((uint32_t *)(data+namelen+4));

                        with = ntohl(with);

                        if(with < user->balance){
                            user->balance = user->balance - with;
                            sendback[1] = 1;
                        }
                        else
                            sendback[1] = 0;

                        bank_send(bank, sendback, 2);
                        free(sendback);
                    }
                    else{
                        if(opcode == 3){
                            uint8_t *sendback = malloc(5);
                            uint32_t bal = htonl(user->balance);
                            sendback[0] = 3;

                            memcpy(sendback + 1, &bal, 4);

                            bank_send(bank, sendback, 5);
                            free(sendback);
                        }
                        else{
                            bank_send(bank, errM, 2);
                        }
                    }
                }
                else{
                    bank_send(bank, errM, 2);
                }
            }
            else{
                bank_send(bank, errM, 2);
            }


            free(data);
        }
        else{
            bank_send(bank, errM, 2);
        }
    }
}
