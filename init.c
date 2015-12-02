#include "ports.h"
#include "bank.h"
#include "atm.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char**argv) {
	if(one or both files already exist) {
		printf("Error: one of the files already exists\n");

		return 63;
	} //must not create either file nor overwrite the existing ones

	FILE *bankFile, *atmFile;

	String fName; //specified in cmd line

	bankFile = fopen(fName.bank,"w");
	atmFile = fopen(fName.atm, "w");

	//put stuff in the new files... and make them secure but what? uhhhhhh


	if(exactly one arg missing in cmd) {
		printf("Usage: init " + fName\n");
		return 62; 
	}

	if(program fails for any reason) {
		printf("Error creating initialization files\n");
		return 64;
	} //like directory not found

	fclose(bankFile);
	fclose(atmFile);

	printf("Successfully initialized bank state\n");

	return 0;
}