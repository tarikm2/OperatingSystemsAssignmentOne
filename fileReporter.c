#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

//Declarations
void* CountWords(void*);
void* GetChecksum(void*);
void* CountOccurances(void*);

struct wc_results {
	int lines;
	int words;
	int chars;
};

struct occ_args {
	char* toSearch;
	char* fileName;
};

int main(int argc, char *argv[]) {

	//threads for processing the file
	pthread_t p1, p2, p3;
	
	//result values for thread creation
	int threadRes;

	struct wc_results *wcValues;
	char* mdsumValues = malloc(sizeof(char) * 32);
	int* searchOccurances;
	struct occ_args occArgs;		

	//input values
	char* fileName = argv[1];
	char* searchTerm = argv[2];	
	
	occArgs.toSearch = searchTerm;
	occArgs.fileName = fileName;

	//create a thread for each task
	threadRes = pthread_create(&p1, NULL, &CountWords, fileName);
	assert(threadRes == 0);
	
	threadRes = pthread_create(&p2, NULL, &GetChecksum, fileName);
	assert(threadRes == 0);

	threadRes = pthread_create(&p3, NULL, &CountOccurances, (void*) &occArgs);
	assert(threadRes == 0);
	
	//join the threads
	pthread_join(p1,(void*) &wcValues);
	pthread_join(p2, (void*) &mdsumValues);
	pthread_join(p3, (void*) &searchOccurances);

	//display the results
	printf("(WC): lines= %d words=%d chars=%d\n",
		wcValues -> lines,
		wcValues -> words,
		wcValues -> chars);

	printf("(MD5SUM): %s\n", mdsumValues);
	printf("(WRDCNT): COUNT OF \"%s\"=%d\n", searchTerm, *searchOccurances);
	
	
	return 1;	
}

/*
 * CountWords accepts a reference to a
 * file name, calls wc to count the words, lines and chars 
 * in the file, and returns the rusult in the wc_results sruct.
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

	//close filestream
	if(pclose(fp) == -1) 
		printf("ERROR IN COUNTWORDS: pclose failure\n");
	
	return (void*) toReturn;
}

/*
 * GetChecksum accpts a file name, calls md5sdum 
 * to generate a checksum, and then returns this 
 * checksum.
 */
void* GetChecksum(void* args) {
	//the file name to generate checksum for
	char* fileName = (char*) args;
	char* toReturn = malloc(sizeof(char) * 32);	

	//generate the command to call
	char command[strlen(fileName + 7)];
	strcpy(command, "md5sum ");
	strcat(command, fileName);
	
	//call the command 	
	FILE* fp;
	fp = popen(command, "r");	
	assert(fp != NULL);
	
	//get the result of md5sum command into toReturn
	fscanf(fp, "%s", toReturn); 
	
	//close the file
	assert(fclose(fp) != -1);

	return (void*) toReturn;
}

void* CountOccurances(void* args) {
	struct occ_args *arguments =  (struct occ_args*) args;
	int* toReturn = malloc(sizeof(int));

	char command[strlen(arguments->toSearch)
		   + strlen(arguments->fileName) + 9];

	strcpy(command, "grep -c \"");
	strcat(command, arguments->toSearch);
	strcat(command, "\" ");
	strcat(command, arguments->fileName);
	
	FILE* fp;
	fp = popen(command, "r");
	assert(fp != NULL);
	
	fscanf(fp, "%d", toReturn);

	assert(fclose(fp) != -1);

	return (void*) toReturn;
}
