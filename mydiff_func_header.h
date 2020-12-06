/****  STANDARD INCLUDE FILES SECTION ******/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/******** CONSTANT DEFINITION SECTION ********/
#define SUCCESS 1
#define FAILURE 0
#define FILE_ONE  1
#define FILE_TWO 2
#define NO_TEMP_FILE 0
#define TEMP_FILE 1
#define LINE_SIZE 150

/****** NEW DATA TYPE DEFINITION SECTION *******/
struct info
{
      int file1NumOfLinesRead;
      int file2NumOfLinesRead;
      int linesTraversedTillFound;
      unsigned int found;
      unsigned int retStatus;
};

/******* FUNCTION PROTOTYPE SECTION **********/
int readLineFromFile(FILE *, char *);
int compareFiles(FILE *, FILE *, char *, char *);
void initializeInfoBuffer(struct info *infoBuffer);
int allocateMemory(char **);
int readRemainingLinesFromFile(FILE *, int);
int addLinesToFileOne(int, int, int, int);
int deleteLinesFromFileOne(int, int, int);
struct info searchLineInFile(FILE *, char*, int, char *);
struct info lookUpNextMatch(FILE *, FILE *);
int changeLinesInFile(int , int , int , int);
void displayResults(char *, char *, int);
int diff(char **, char **);