#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

//Declarations
void* CountWords(void* args);
FILE* GetChecksum(char* fileName);
FILE* CountOccurances(char* fileName, char* toFind);

struct wc_results {
	int lines;
	int words;
	int chars;
};

int main(int argc, char *argv[]) {

	//threads for processing the file
	pthread_t p1, p2, p3;
	
	//result values for thread creation
	int wcThreadRes;
	int gcThreadRes;
	int coThreadRes;

	struct wc_results *wcValues;
	
	//input values
	char* fileName = argv[1];
	char* searchTerm = argv[2];	

	//create a thread for each task
	wcThreadRes = pthread_create(&p1, NULL, &CountWords, fileName);
	assert(wcThreadRes == 0);

	//join the threads
	pthread_join(p1,(void*) &wcValues);

	//display the results
	printf("(WC: lines= %d words=%d chars=%d\n",
		wcValues -> lines,
		wcValues -> words,
		wcValues -> chars);
	printf("(MD5SUM): not implemented\n");
	printf("(WRDCNT): not implemented\n");
	
	return 1;	

}

/*
 * CountWords accepts a reference to three integers and a 
 * file name, calls wc to count the words, lines and chars 
 * in the file, and saves the result in the three integers
*/ 
void* CountWords(void* args) {
	//the file to save our command line results i
	char* fileName = (char*) args;
	//return value
	struct wc_results *toReturn = malloc(sizeof(struct wc_results));
	
	//the command to perform 
	char command[strlen(fileName) + 3];
	strcpy(command, "wc ");
	strcat(command, fileName);
        
	//the filestream capturing command result
	FILE* fp;
	fp = popen(command, "r");
	
	//handle file errors
	if (fp == NULL)
		printf("ERROR IN COUNTWORDS: popen failure\n");
	
	int lines;
	int words;
	int chars;
	
	//get results from filestream
	fscanf(fp, " %d %d %d", &lines,
				&words,
				&chars);

	toReturn->lines = lines;
	toReturn->words = words;
	toReturn->chars = chars;
	
	printf("just after gathering input\n");	

	//close filestream
	if(pclose(fp) == -1) 
		printf("ERROR IN COUNTWORDS: pclose failure\n");
	
	return (void*) toReturn;
}


FILE* GetChecksum(char* fileName) {

	return NULL;
}

