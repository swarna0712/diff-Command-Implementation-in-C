#include "mydiff_func_header.h"        

/**************************************************************
FUNCTION NAME: main(list of file names input by user)

THIS PROGRAM MIMICS THE LINUX DIFF COMMAND WITHOUT ANY OPTIONS.
IN ADDITION TO COMPARING JUST TWO FILES, THIS PPROGRAM ACCEPTS 
MORE THAN ONE FILE TO BE COMPARED WITH FIRST FILE.
IT RUNS THROUGH INPUT FILES, COMPARES TWO FILES LINE BY LINE, 
THAT IS, FIRST FILE WITH REST OF THE FILES ONE BY ONE.
IT DISPLAYS THE CHANGES THAT HAVE TO BE MADE IN FIRST FILE WITH
RESPECT TO THE OTHER FILE AS WELL AS THE NUMBER OF LINES THAT 
DIFFER BETWEEN TWO FILES.
	
HOW TO RUN THE PROGRAM:
mydiff FILE1 FILE2 ... FILEn
****************************************************************/

int main(int argc,char *argv[])
{
    char *fileName1, *fileName2;
    int numOfFilesToBeCompared = 0, fileCount=0;
  
  fileName1 = argv[1];  // Get the first file name from the command line.
  
  // Check if minimum two file names are entered in command line.
  
   if( argc < 3 )      // If less than 2 file names are entered, report error.
   {
        printf("mydiff: missing operand after \'%s\'\n", fileName1);
        printf("\nHelp: mydiff <filename1> <filename2>\n");
        return 0;
   }
  
   numOfFilesToBeCompared = argc - 2 ;      // Get the number of files to be compared with fileName1.

  for(fileCount = 0; fileCount < numOfFilesToBeCompared; fileCount++)
  {
    fileName2 = argv[fileCount+2];         // Get the name of the file to be compared with first file from the command line.
	
   	if(( strcmp(fileName1,fileName2) == 0))   // If both file names are same report warning.
  	{
        		printf("\nmydiff:SKIPPED COMPARISON OF \"%s\" AND \"%s\" AS FILE NAMES ARE SAME.\n ", fileName1, fileName2);
        		continue;	
  	}
	printf("\n*******************************************************************************************\n");
	printf("\ndiff: DIFFERENCES BETWEEN \"%s\" AND \"%s\" \n\n", fileName1,fileName2);
	diff(&fileName1, &fileName2);
  }
  printf("\n*******************************************************************************************\n");

}