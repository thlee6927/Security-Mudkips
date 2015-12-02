#include "ports.h"
#include "bank/bank.h"
#include "atm/atm.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <dirent.h> 

static unsigned char key[32];
static unsigned char iv[16];

int main(int argc, char**argv) {
	struct timeval blah;
	int i, n, j, index, exists = 1;
	char *second = argv[1];
	char *path2, *fName, *next, *bankPath, *atmPath;
	FILE *bankFile, *atmFile;
	DIR *d;
	struct dirent *dir;

	if(argc == 1) {
		printf("Usage: init <filename>\n");
		return 62; 
	} //user fails to provide exactly one argument

	gettimeofday(&blah);
	srand(blah.tv_sec);

	for(i = 0; i < 32; i++) {
		key[i] = rand() % 256;
	} //filling the key with random nums

	for(n = 0; n < 16; n++) {
		iv[n] = rand() % 256;
	} //filling the iv with random nums

	for(j = 0; j < strlen(second); j++) {
		if(second[j] == '/') {
			index = i;
		}
	} //finds index of last '/' so I can freaking figure out the path2 and the filename

	strncpy(path2, second, index); //gets path2

	fName = strtok(second, "/");
	next = strtok(NULL, "/");

	while(next != NULL) {
		fName = next;
		next = strtok(NULL, "/");
	} //gets filename

	bankPath = (char *) malloc(1 + strlen(second) + strlen(".bank"));
	atmPath = (char *) malloc(1 + strlen(second) + strlen(".atm"));

	strcat(bankPath, ".bank");
	strcat(atmPath, ".atm");

	d = opendir(path2);

	if(d == NULL) {
		printf("Error creating initialization files\n");
		return 64;
	} //directory doesn't exist
	else {
		while((dir = readdir(d)) != NULL) {
			if((strcmp(dir->d_name, bankPath) == 0) || (strcmp(dir->d_name, atmPath) == 0)) {
				exists = 0;
				break;
			}
		} //checks if the files already exist

		if(exists == 0) {
			printf("Error: one of the files already exists\n");

			return 63;
		} //must not create either file nor overwrite the existing ones
		else {
			bankFile = fopen(bankPath, "w");
			atmFile = fopen(atmPath, "w");

			fprintf(bankFile, "%s", key);
			fprintf(bankFile, "%s", iv);
			fprintf(atmFile, "%s", key);
			fprintf(atmFile, "%s", iv);

			printf("Successfully initialized bank state\n");

			fclose(bankFile);
			fclose(atmFile);

			return 0;
		}
	}

	printf("Error creating initialization files\n"); //if the program fails for any other reason
		
	return 64;
}