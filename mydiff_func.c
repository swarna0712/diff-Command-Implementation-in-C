#include "mydiff_func_header.h"  

/***************************************************
FUNCTION NAME : diff(fileName1, fileName2)

THIS FUNCTION TAKES TWO FILE NAMES AS INPUT.
IT OPENS BOTH THE FILES AND ALLOCATES TWO
BUFFERS TO STORE THE LINES READ FROM BOTH THE FILES.
THEN, IT CALLS COMPARE FILES TO MIMIC DIFF COMMAND.
FINALLY IT DISPLAYS THE NUMBER OF LINES THAT DIFFER
IN FILE1 WITH RESPECT TO FILE2.
****************************************************/

int diff(char **fileName1, char **fileName2)   
{
     char *lineFromFile1 = NULL, *lineFromFile2 = NULL;
     int retVal=SUCCESS;
     int differingLineCount=0;
     FILE *fptr1 = NULL, *fptr2 = NULL;

    retVal = allocateMemory(&lineFromFile1);   // Allocate memory to buffer to store the line read from FILE1.
    if (retVal == FAILURE) return(FAILURE);

    retVal = allocateMemory(&lineFromFile2);   // Allocate memory to buffer to store the line read from FILE2.
    if (retVal == FAILURE) return(FAILURE);

   fptr1 = fopen(*fileName1,"r");   // Open FILE1 in read mode.
   if(fptr1 == NULL)				// If error in opening file, report error and return.
   {
         printf("mydiff: %s : No such file or directory\n", *fileName1);
         free(lineFromFile1);       // free allocated memory.
         free(lineFromFile2);		// free allocated memory.
         return(FAILURE);
   }
   
   fptr2 = fopen(*fileName2,"r");       // Open FILE2 in read mode.
   if(fptr2 == NULL)           			// If error in opening file, report error and return.
   {
         printf("mydiff: %s : No such file or directory\n", *fileName2);
         free(lineFromFile1);			// free allocated memory.
         free(lineFromFile2);			// free allocated memory.
         return(FAILURE);
   }

   differingLineCount = compareFiles(fptr1,fptr2,lineFromFile1,lineFromFile2); // Call this function to mimic diff command.

   free(lineFromFile1);                 // free allocated memory.
   free(lineFromFile2);					// free allocated memory.
   fclose(fptr1);						// Close FILE1.
   fclose(fptr2);						// Close FILE2.
  
   displayResults(*fileName1, *fileName2, differingLineCount);   // Call this function to display the number of lines that differ.
   return(SUCCESS);
}


/*************************************************************************
FUNCTION NAME : compareFiles(filePtr1, filePtr2, file1Buffer, file2Buffer)

THIS FUNCTION COMPARES FILES LINE BY LINE. IT SKIPS THE MATCHING LINE.
ONCE MISMATCH IS FOUND, IT LOOKS FOR THE NEXT IMMEDIATELY AVAILABLE 
MATCHING LINE IN BOTH THE FILES. WITH THIS IT FORMS TWO MARKER BLOCKS
IN BOTH THE FILES. ONCE THE PROCESSING OF THIS BLOCK IS COMPLETE, THIS
PROCESS CONTINUES TO FORM THE NEXT MARKER BLOCK AND SO ON TILL EOF. 
FIRST, IT SEARCHES FOR THE MISMATCHED LINE IN FILE2 WITHIN THE MARKER
BLOCK OF FILE1. IF FOUND, IT DISPLAYS THE SET OF LINES TRAVERSED TILL
MISMATCH AS LINES TO BE DELETED FROM FILE1. 
IF MATCH NOT FOUND, IT SEARCHES FOR MISMATCH OF LINE IN FILE1 WITHIN THE
MARKER BLOCK OF FILE2. IF FOUND, IT DISPLAYS THE SET OF LINES TRAVERSED
TILL MISMATCH AS LINES TO BE ADDED TO FILE1.
IF MATCH IS NOT FOUND IN BOTH THE ABOVE CASES, IT DISPLAYS THE SET OF 
LINES WITHIN THE MARKER BLOCK OF FILE1 TO BE CHANGED TO THE SET OF
LINES WITHIN THE MARKER BLOCK OF FILE2.
IN ALL THE ABOVE CASES, WHEREVER MISMATCHED LINES ARE IDENTIFIED, 
THE COUNT FOR THE NUMBER OF LINES THAT DIFFER IS INCREMENTED AND
RETURNED TO diff FUNCTION. THE ABOVE SEARCH PROCESSES ARE REPEATED
TILL THE END OF MARKER BLOCK.
file1LineNumber - Number of lines traversed in FILE1.
file2LineNumber - Number of lines traversed in FILE2.
startRangeNum - The line number from where either add or delete or change 
				of lines occurs.
endRangeNum - The line number till where either add or delete or change 
			   of lines occurs.
numOfRemLinesRead - Number of remaining lines read from file when either
					of two files reaches EOF.
diffCount - Number of lines added or deleted or changed.
totalDifferingLineCount - Total number of lines that differ in FILE1 with
						  respect to FILE2.
file1Position - Current position of file pointer of FILE1.	
file2Position - Current position of file pointer of FILE2.	
extractInfoBuffer - This buffer is returned by lookUpNextMatch function to
					return the number of lines which forms the marker block
					of FILE1 and FILE2 and found or not found status.
searchInfoBuffer - This buffer is returned by searchLineInFile function to
				   return mainly the number of lines traversed, found or 
				   not found status.
				   
**************************************************************************/

int compareFiles(FILE *fptr1,FILE *fptr2,char *lineFromFile1,char *lineFromFile2)
{
    int file1RetVal=0, file2RetVal=0; 
    int file1LineNumber=0, file2LineNumber=0;
    int startRangeNum=0, endRangeNum=0, numOfRemLinesRead = 0,file1Position=0, file2Position=0;
    int diffCount = 0, totalDifferingLineCount = 0;
   
   while(1)
   {
	   file1Position = ftell(fptr1);							// Get current position of file pointer of FILE1.
       file1RetVal = readLineFromFile(fptr1, lineFromFile1);   	// Read Line from FILE1
		if(file1RetVal == EOF)  								// Is FILE1 reached EOF
		{
			startRangeNum = file2LineNumber + 1 ;				
			numOfRemLinesRead = readRemainingLinesFromFile(fptr2,FILE_TWO);  // Retrieve remaining lines from FILE2 to tempFile2.
			if(numOfRemLinesRead == 0)    
				break;
			else if(numOfRemLinesRead < 0)
				return(FAILURE);
			endRangeNum = startRangeNum + numOfRemLinesRead - 1;
			diffCount = addLinesToFileOne(NO_TEMP_FILE, file1LineNumber,startRangeNum,endRangeNum); // Display that the retrieved lines to be added to FILE1.
			if(diffCount  == FAILURE) return(FAILURE);
			else
			{
				totalDifferingLineCount = totalDifferingLineCount + diffCount;
				break; 
			}
		}
		else 
		{
			file2Position = ftell(fptr2);							// Get current position of file pointer of FILE2.
			file2RetVal = readLineFromFile(fptr2, lineFromFile2);  	// Read Line from FILE2
			if (file2RetVal == EOF)				 					// Is FILE2 reached EOF
			{
				startRangeNum = file1LineNumber + 1 ;
                fseek(fptr1, file1Position, SEEK_SET);
				numOfRemLinesRead = readRemainingLinesFromFile(fptr1,FILE_ONE); // Retrieve remaining lines from FILE1 to tempFile1.
				if(numOfRemLinesRead == 0)    // FILE1 also reached EOF
					break;
				else if(numOfRemLinesRead < 0)
					return(FAILURE);

				endRangeNum = startRangeNum + numOfRemLinesRead ;
	        	diffCount = deleteLinesFromFileOne(file2LineNumber,startRangeNum,endRangeNum);  // Display that the retrieved lines to be deleted from FILE1.
				if(diffCount == FAILURE)
					return(FAILURE);
				else 
				{
					totalDifferingLineCount = totalDifferingLineCount + diffCount;
					break; 
				}
            }
		}
        file1LineNumber ++;
        file2LineNumber ++;

		if(strcmp(lineFromFile1,lineFromFile2) !=0)   // If two lines are not identical, i.e. mismatch found
		{
			int file1PositionToBeCompared=0, file2PositionToBeCompared=0, file1PositionToStart=0, file2PositionToStart=0 ;
			int file1EndRangeNum=0, file2EndRangeNum=0;
			struct info searchInfo, extractInfo;
		
			initializeInfoBuffer(&searchInfo);      // Initialize the serachInfoBuffer. 
			initializeInfoBuffer(&extractInfo);		// Initialize the extractInfoBuffer.
		                
	        startRangeNum = file1LineNumber;

			fseek(fptr1, file1Position, SEEK_SET);
			fseek(fptr2, file2Position, SEEK_SET);	
		
			extractInfo=lookUpNextMatch(fptr1, fptr2);    // Call this function to form marker block for FILE1 and FILE2.
			
			if(extractInfo.retStatus == FAILURE) 
				break;
		
			file2PositionToBeCompared=ftell(fptr2);
			file1PositionToBeCompared=ftell(fptr1);	

			if((extractInfo.found==1) && (extractInfo.file1NumOfLinesRead == 0))  // if the number of lines in marker block of FILE1 is zero.
			{
				startRangeNum = file2LineNumber;
				if(extractInfo.file2NumOfLinesRead>1)
					endRangeNum =file2LineNumber+ extractInfo.file2NumOfLinesRead-1;
				else if(extractInfo.file2NumOfLinesRead==1)
					endRangeNum =0;
				else
					endRangeNum =file2LineNumber+ extractInfo.file2NumOfLinesRead;
				diffCount = addLinesToFileOne(TEMP_FILE, (file1LineNumber -1),startRangeNum,endRangeNum); // Displays the lines within the marker block of FILE2 as lines to be added to FILE1.
				fseek(fptr1, file1PositionToBeCompared, SEEK_SET);
                fseek(fptr2, file2PositionToBeCompared, SEEK_SET);
				file2LineNumber+= extractInfo.file2NumOfLinesRead;
				if(diffCount  == FAILURE) 
					return(FAILURE);
				else
					totalDifferingLineCount = totalDifferingLineCount +diffCount;
			}
			else
			{		
				fseek(fptr1, file1Position, SEEK_SET);
				fseek(fptr2, file2Position, SEEK_SET);		
	                 
				searchInfo = searchLineInFile(fptr1, lineFromFile2, extractInfo.file1NumOfLinesRead,NULL);      // Search of mismatched line of FILE2 within the marker block of FILE1.
				if(searchInfo.retStatus == FAILURE) 
					break;

				file1PositionToStart=ftell(fptr1);
				file2RetVal = readLineFromFile(fptr2, lineFromFile2);	
				file2PositionToStart=ftell(fptr2);
				                
				if((searchInfo.found == 1))															// If found
				{
					endRangeNum = startRangeNum + searchInfo.linesTraversedTillFound ;
					diffCount = deleteLinesFromFileOne(file2LineNumber-1,startRangeNum,endRangeNum);  // Display the set of traversed ines as to be deleted from FILE1.
					file1LineNumber += searchInfo.linesTraversedTillFound;
					if(diffCount == FAILURE) return(FAILURE); 
					totalDifferingLineCount = totalDifferingLineCount + diffCount;
					if((extractInfo.file2NumOfLinesRead==1))
					{
						file2RetVal = readLineFromFile(fptr2, lineFromFile2);
						file2RetVal = readLineFromFile(fptr2, lineFromFile2);
						searchInfo = searchLineInFile(fptr1, lineFromFile2, (extractInfo.file1NumOfLinesRead-1),NULL);  // Search of mismatched line of FILE2 within the marker block of FILE1.
						if(searchInfo.retStatus == FAILURE) 
							break;
					
						if((searchInfo.found == 1))
						{
							file1PositionToStart=ftell(fptr1);	
							file2PositionToStart=ftell(fptr2);
							startRangeNum=file1LineNumber+1;
							endRangeNum = startRangeNum + searchInfo.linesTraversedTillFound ;
							diffCount = deleteLinesFromFileOne(file2LineNumber,startRangeNum,endRangeNum);  // Display the set of traversed ines as to be deleted from FILE1.
							file1LineNumber+= searchInfo.linesTraversedTillFound + 1;
							file2LineNumber++;
							if(diffCount == FAILURE) 
								return(FAILURE); 
							else
								totalDifferingLineCount = totalDifferingLineCount + diffCount;
						}
						fseek(fptr1, file1PositionToStart, SEEK_SET);
				        fseek(fptr2, file2PositionToStart, SEEK_SET);
					}
					fseek(fptr1, file1PositionToStart, SEEK_SET);
					fseek(fptr2, file2PositionToStart, SEEK_SET);	
				}
				else 
				{
					fseek(fptr1,file1Position,SEEK_SET);	
					fseek(fptr2,file2Position,SEEK_SET);	

					startRangeNum = file2LineNumber;
					searchInfo = searchLineInFile(fptr2, lineFromFile1,extractInfo.file2NumOfLinesRead, lineFromFile2);     // Search of mismatched line of FILE1 within the marker block of FILE2.
					if(searchInfo.retStatus == FAILURE) 
						break;

					endRangeNum = startRangeNum + searchInfo.linesTraversedTillFound ;
					file1PositionToStart=ftell(fptr1);	
					file2PositionToStart=ftell(fptr2);

					if((searchInfo.found == 1))   // If mismatched line of FILE1 is found within marker bloack of FILE2.
					{
						diffCount = addLinesToFileOne(NO_TEMP_FILE, (file1LineNumber -1),startRangeNum,endRangeNum);   // Display the set of traversed ines as to be added to FILE1.
						file2LineNumber+= searchInfo.linesTraversedTillFound ;
						fseek(fptr1, file1PositionToStart, SEEK_SET);
						fseek(fptr2, file2PositionToStart, SEEK_SET);
						if(diffCount  == FAILURE) 
							return(FAILURE);
						else
							totalDifferingLineCount = totalDifferingLineCount + diffCount;
					}
					else  						// If mismatched line of FILE1 is NOT found within marker bloack of FILE2.
					{
						if((extractInfo.file1NumOfLinesRead==1) || (extractInfo.file1NumOfLinesRead== -1) )			
							file1EndRangeNum=0;
						else
							file1EndRangeNum=file1LineNumber+extractInfo.file1NumOfLinesRead-1;
						
						if((extractInfo.file2NumOfLinesRead==1) || (extractInfo.file2NumOfLinesRead==0))
							file2EndRangeNum=0;	
						else  if(extractInfo.file2NumOfLinesRead==2)
							file2EndRangeNum=file2LineNumber+extractInfo.file2NumOfLinesRead-1;
						else	
							file2EndRangeNum=file2LineNumber+extractInfo.file2NumOfLinesRead;
						
						// Display of set of lines from FILE1 marker block to be changed to the set of lines from FILE2 marker block.
						diffCount = changeLinesInFile(file1LineNumber, file2LineNumber, file1EndRangeNum, file2EndRangeNum);
						
						file1LineNumber+= extractInfo.file1NumOfLinesRead ;
						file2LineNumber+= extractInfo.file2NumOfLinesRead ;
						fseek(fptr1, file1PositionToBeCompared, SEEK_SET);
 			            fseek(fptr2, file2PositionToBeCompared, SEEK_SET);
						if(diffCount  == FAILURE) 
							return(FAILURE);
						else
							totalDifferingLineCount = totalDifferingLineCount + diffCount;
					}			
				}
			}
		
		}
	}
    return(totalDifferingLineCount);
}

/***************************************************
FUNCTION NAME : allocateMemory(Buffer)

THIS FUNCTION DYNAMICALLY ALLOCATES MEMORY OF SIZE
"LINESIZE" TO THE BUFFER AND RETURNS THE POINTER TO 
THE ALLOCATED MEMORY. 
****************************************************/

int allocateMemory(char **lineFromFile)
{
 
   *lineFromFile = (char*)malloc(LINE_SIZE);

   if(*lineFromFile == NULL)
   {
         printf("mydiff: Error in allocating memory");
         return(FAILURE);
   }
   return(SUCCESS);
}

/***************************************************
FUNCTION NAME : readLineFromFile(filePtr,Buffer)

THIS FUNCTION READS LINE FROM THE FILE INTO THE 
BUFFER AND RETURNS THE BUFFER. 
****************************************************/

int readLineFromFile(FILE *fptr, char *lineFromFile)
{
     int status = 0;

     status = fscanf(fptr, "%[^\n]\n",lineFromFile);  
     return(status);
}

/***************************************************
FUNCTION NAME : initializeInfoBuffer(infoBuffer)

THIS FUNCTION INITIALIZES ALL THE FIELDS IN THE
STRUCTURE TO INITIAL VALUE. 
****************************************************/

void initializeInfoBuffer(struct info *infoBuffer)
{
	infoBuffer->linesTraversedTillFound =0;
	infoBuffer->found=0;
    infoBuffer->file1NumOfLinesRead =0;
	infoBuffer->file2NumOfLinesRead =0;
    infoBuffer->retStatus=SUCCESS;
}

/***************************************************
FUNCTION NAME : lookUpNextMatch(filePtr1, filePtr2)

THIS FUNCTION IS CALLED WHENEVER MISMATCH IS FOUND 
BETWEEN TWO FILES. IT READS LINES ONE BY ONE FROM
FILE2 AND SEARCHES FOR THE MATCH IN FILE1. IF FOUND
THE MARKER BLOCK IS FORMED. ELSE, THE NEXT LINE FROM
FILE2 IS SEARCHED IN THE SAME PROCESS. IF EITHER OF
THE FILES REACH EOF AND IF NO MARKER BLOCK IS FORMED
YET, THEN THE ENTER REMAINING LINES OF BOTH THE FILES
TILL EOF IS FORMED AS MARKER BLOCK.
****************************************************/

struct info lookUpNextMatch(FILE *fptr1, FILE *fptr2)
{
	struct info extractInfo;
	char *lineFromFile1 = NULL, *lineFromFile2 = NULL;
 	int file1RetVal=0, file2RetVal=0, file1Position=0, file2Position=0, file2PositionToBeSet=0 ;
	int retVal = SUCCESS;
	FILE *tempFptr = NULL, *tempFptr2 = NULL;

	extractInfo.retStatus=SUCCESS;
	
	tempFptr=fopen("tempFile.txt","w");          // Open tempFile to store the read line, if the marker block of FILE1 has zero lines.
		
    if(tempFptr == NULL)
   	{
   		printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
		extractInfo.retStatus = FAILURE;
   		return(extractInfo);
	}
 
	tempFptr2=fopen("tempFile2.txt","w");       // Open tempFile2 to store the lines read from FILE2 till marker block.
	
		
    if(tempFptr2 == NULL)
   	{
   		printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
		extractInfo.retStatus = FAILURE;
		fclose(tempFptr);
   		return(extractInfo);
	}

	retVal = allocateMemory(&lineFromFile1);    	
 	if(retVal == FAILURE)
   	{
		extractInfo.retStatus=FAILURE;
 		fclose(tempFptr);
		fclose(tempFptr2);
		return(extractInfo);
   	}

	retVal = allocateMemory(&lineFromFile2);    	
 	if(retVal == FAILURE)
    {
		extractInfo.retStatus=FAILURE;
 		fclose(tempFptr);
		fclose(tempFptr2);
		free(lineFromFile1);
    	return(extractInfo);
   	}

	initializeInfoBuffer(&extractInfo);
	
	file1Position = ftell(fptr1);
	file2Position = ftell(fptr2);
	file2RetVal = readLineFromFile(fptr2, lineFromFile2);   
	file2PositionToBeSet=ftell(fptr2);     

	while(1)
	{
		
		file2RetVal = readLineFromFile(fptr2, lineFromFile2);       // Read line from FILE2.
		if (file2RetVal == EOF)
		{	
		    file2PositionToBeSet=ftell(fptr2); 
		    fprintf(tempFptr2,"%s\n", lineFromFile2);	
			if(extractInfo.file2NumOfLinesRead == 0)                // If marker block not formed and FILE2 reached EOF, then marker block for both the files is till EOF.
		    {
				extractInfo.file1NumOfLinesRead = EOF;		
				extractInfo.file2NumOfLinesRead = EOF;		
				fseek(fptr1,0,SEEK_END);
			}
		    extractInfo.file2NumOfLinesRead++;
            break;
		}
		extractInfo.file1NumOfLinesRead=0;
		while(!extractInfo.found)
		{
			file1RetVal = readLineFromFile(fptr1, lineFromFile1);     // Read line from FILE1.
       		if (file1RetVal == EOF)  
			{
				if(extractInfo.file1NumOfLinesRead == 0)			  // If marker block not formed and FILE1 reached EOF, then marker block for both the files is till EOF.
			    {
					extractInfo.file1NumOfLinesRead = EOF;		
					extractInfo.file2NumOfLinesRead = EOF;
					fseek(fptr2,0,SEEK_END);		
			    }
			    break;
			}
			if((strcmp(lineFromFile1,lineFromFile2)==0) && (extractInfo.file1NumOfLinesRead == 0))  // If matching line is found and marker block of file1 has zero lines.
			{
				file2PositionToBeSet=ftell(fptr2);
				extractInfo.found=1;
				fseek(fptr2, file2Position, SEEK_SET);
				file2RetVal = readLineFromFile(fptr2, lineFromFile2);
				fprintf(tempFptr,"%s\n", lineFromFile2);	
				extractInfo.file2NumOfLinesRead++;
				fseek(fptr2, file2PositionToBeSet , SEEK_SET);
				break;
			}	
			if((strcmp(lineFromFile1,lineFromFile2)==0))
			{
				extractInfo.found=1;
				extractInfo.file2NumOfLinesRead++;
				break;
			}
			extractInfo.file1NumOfLinesRead++;		
		}
		
		if(extractInfo.found==0)    // If lines are not identical store the line of FILE2 in tempFile2 and continue the process.
		{
			fprintf(tempFptr2,"%s\n", lineFromFile2);	
			extractInfo.file2NumOfLinesRead++;
		    fseek(fptr1, file1Position, SEEK_SET);
		}
		else
			break;
		
	}

	if((file2RetVal==EOF) && (extractInfo.found==0))      // If marker block not formed and FILE2 reached EOF.
	{
		fseek(fptr2, file2PositionToBeSet, SEEK_SET);
		fseek(fptr1, 0, SEEK_END);
	}
	
	free(lineFromFile1);     
	free(lineFromFile2);
	fclose(tempFptr);
	fclose(tempFptr2);
	return(extractInfo);
	
}

/**********************************************************************************************
FUNCTION NAME : searchLineInFile(filePtr1, lineToBeSearched, markerBlockNumberOfLines, buffer)

THIS FUNCTION IS CALLED WHENEVER MISMATCHED LINE FROM ONE FILE IS TO BE SEARCHED WITHIN THE
MARKER BLOCK OF THE OTHER FILE. IT COMPARES THE LINE TO BE SEARCHED WITH THE LINES OF THE OTHER
FILE WITHIN THE MARKER BLOCK AND STORES THE NON IDENTICAL LINES INTO THE RESPECTIVE TEMPFILE.
IF FOUND IT RETURNS THE searchInfo STRUCTURE.
***********************************************************************************************/

struct info searchLineInFile(FILE *fptr, char *lineToBeSearched, int searchTillLine, char *lineFromFile2)
{
    int file1RetVal=0;
    int counter=1, retVal = SUCCESS;  
    char *lineFromFile = NULL;
    struct info searchInfo;
    FILE *tempFptr = NULL;
    
    if (lineFromFile2 == NULL)
    	tempFptr=fopen("tempFile1.txt","w");
	else if((searchTillLine==0) && lineFromFile2!=NULL)
		tempFptr=fopen("tempFile2.txt","a");
	else if((searchTillLine!=0) && lineFromFile2!=NULL)
		tempFptr=fopen("tempFile2.txt","w");
		
    if(tempFptr == NULL)
	{
		printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
		searchInfo.retStatus = FAILURE;
		return(searchInfo);
	}

	retVal = allocateMemory(&lineFromFile);
    if(retVal == FAILURE)
	{
        fclose(tempFptr);
        searchInfo.retStatus = FAILURE;
        return(searchInfo);
	}

	initializeInfoBuffer(&searchInfo);

	if( searchTillLine != EOF)          // If the value of searchTillLine is greater than zero, then search till that line number. 
	{  
      	while(counter<=searchTillLine)
       	{
       		file1RetVal = readLineFromFile(fptr, lineFromFile);   // Read Line from FILE1.
       		if (file1RetVal == EOF) 
				break ;	
           	if (strcmp(lineToBeSearched, lineFromFile) != 0)          // If lines are non identical then store the lines into tempFile.
           	{
           		fprintf(tempFptr,"%s\n", lineFromFile);
            	searchInfo.linesTraversedTillFound++;
           	}
           	else
           	{
				searchInfo.found = 1;
				searchInfo.retStatus=SUCCESS;
				break;
          	}
         	counter++;
       	
		}
	}
	else
	{
		while(file1RetVal != EOF)
       	{
        	file1RetVal = readLineFromFile(fptr, lineFromFile);   // Read Line from FILE1 or FILE2
            if (file1RetVal == EOF) 
				break ;	
           	if (strcmp(lineToBeSearched, lineFromFile) != 0)
           	{
           		fprintf(tempFptr,"%s\n", lineFromFile);
            	searchInfo.linesTraversedTillFound++;
           	}
          	else
          	{
				searchInfo.found = 1;
				searchInfo.retStatus=SUCCESS;
				break;
          	}
         	counter++;
       	
		}
    } 
	
    fclose(tempFptr);		             
    free(lineFromFile);
    return(searchInfo);
}

/**********************************************************************************************
FUNCTION NAME : addLinesToFileOne(tempFileFlag, file1LineNumber,startRangeNum, endRangeNum)

THIS FUNCTION IS CALLED WHENEVER SET OF LINES ARE IDENTIFIED TO BE ADDED TO FILE1.
THE SET OF LINES ARE ALREADY STORED IN EITHER IN tempFile OR tempFile2 BEFORE
THIS FUNCTION IS CALLED. IT DISPLAYS THE RANGE OF LINE NUMBERS AND THE CONTENT OF 
tempFile OR tempFile2.
***********************************************************************************************/

int addLinesToFileOne(int tempFileToBeRead, int file1LineNumber, int startRangeNum, int endRangeNum)
{
    int temp=0, numOfLinesToBeAdded = 0, retVal = SUCCESS;
    char *lineFromTemp = NULL;
    FILE *tempFptr = NULL, *tempFptr2 = NULL;
    int diffCount = 0;

	
    tempFptr=fopen("tempFile.txt","r");				// Open tempFile
    if(tempFptr == NULL)
    {
  	printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
   	return(FAILURE);
    }    

	tempFptr2=fopen("tempFile2.txt","r");		    // Open tempFile2
		
	if(tempFptr2 == NULL)
	{
		printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
		return(FAILURE);
	}

	retVal = allocateMemory(&lineFromTemp);
	if(retVal == FAILURE)
	{
        fclose(tempFptr);
        fclose(tempFptr2);
        return(FAILURE);
	}

	numOfLinesToBeAdded = endRangeNum - startRangeNum;
	
	if (numOfLinesToBeAdded >= 1)
       printf("%da%d,%d\n", file1LineNumber, startRangeNum, endRangeNum);		// Print the range of line number of FILE1 and FILE2.
	else
       printf("%da%d\n", file1LineNumber, startRangeNum);						// Print the line number of FILE1 and FILE2.

    if (tempFileToBeRead == TEMP_FILE)
    {
      	temp = readLineFromFile(tempFptr, lineFromTemp);	// Read Line from tempFile.
      	printf(">%s\n",lineFromTemp);						// Print the line read from tempFile.
		diffCount++;										// Compute the number of lines that are to be added to FILE1.
    }
  
	while(1)
	{
		temp = readLineFromFile(tempFptr2, lineFromTemp);   // Read Line from tempFile2.
		if (temp == EOF) 
			break;
		printf(">%s\n",lineFromTemp);					    // Display the lines read from tempFile2.	
		diffCount++;										// Compute the number of lines that are to be added to FILE1.
	}

	fclose(tempFptr);
	fclose(tempFptr2);
	free(lineFromTemp);
	return(diffCount);
}

/**********************************************************************************************
FUNCTION NAME : deleteLinesFromFileOne(file1LineNumber,startRangeNum, endRangeNum)

THIS FUNCTION IS CALLED WHENEVER SET OF LINES ARE IDENTIFIED TO BE DELETED FROM FILE1.
THE SET OF LINES ARE ALREADY STORED IN tempFile1 BEFORE THIS FUNCTION IS CALLED. 
IT DISPLAYS THE RANGE OF LINE NUMBERS AND THE CONTENT OF tempFile1.
***********************************************************************************************/

int deleteLinesFromFileOne(int file2LineNumber, int startRangeNum, int endRangeNum)
{
    int temp=0, numOfLinesToBeDeleted = 0, retVal = SUCCESS;
    char *lineFromTemp = NULL;
    FILE *tempFptr = NULL;
    int diffCount = 0;    

    tempFptr=fopen("tempFile1.txt","r");			// Open tempFile1.
		
    if(tempFptr == NULL)
	{
		printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
		return(FAILURE);
	}

    retVal = allocateMemory(&lineFromTemp);
    if(retVal == FAILURE)
	{
        fclose(tempFptr);
        return(FAILURE);
	}
	
	numOfLinesToBeDeleted = endRangeNum-startRangeNum;

	if(numOfLinesToBeDeleted > 1)
		printf("%d,%dd%d\n", startRangeNum, (endRangeNum-1),file2LineNumber);	// Print the range of line number of FILE1 and FILE2.		
	else
		printf("%dd%d\n", startRangeNum, file2LineNumber);						// Print the line number of FILE1 and FILE2.

    while(1)
	{
		temp = readLineFromFile(tempFptr, lineFromTemp);   // Read Line from tempFile1.
		if(temp == EOF) 
			break;
		printf("<%s\n",lineFromTemp);					   // Display the lines read from tempFile1.
		diffCount++;									   // Compute the number of lines that are to be deleted from FILE1.
	}
	fclose(tempFptr);
	free(lineFromTemp);
	return(diffCount);
}

/**********************************************************************************************
FUNCTION NAME:changeLinesInFile(file1LineNumber,file2LineNumber, file1EndRangeNum,file2EndRangeNum)

THIS FUNCTION IS CALLED WHENEVER SET OF LINES OF FILE1 ARE IDENTIFIED AS TO BE CHANGED
TO SET OF LINES OF FILE2. THE SET OF LINES OF FILE1 ARE ALREADY STORED IN tempFile1 AND
SET OF LINES OF FILE2 ARE ALREADY STORED IN tempFile2 BEFORE THIS FUNCTION IS CALLED. 
IT DISPLAYS THE RANGE OF LINE NUMBERS AND THE CONTENT OF tempFile1, FOLLOWED BY tempFile2.
***********************************************************************************************/

int changeLinesInFile(int file1LineNumber, int file2LineNumber, int file1EndRangeNum, int file2EndRangeNum)
{
  	char *lineFromTemp = NULL;
	int temp=0, retVal = SUCCESS; 
	FILE *tempFptr1 = NULL, *tempFptr2 = NULL;
	int diffCount = 0;

    tempFptr1=fopen("tempFile1.txt","r");			// Open tempFile1.
		
    if(tempFptr1 == NULL)
   	{
   		printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
   	 	return(FAILURE);
   	}

	tempFptr2=fopen("tempFile2.txt","r");			// Open tempFile2.
		
    if(tempFptr2 == NULL)
   	{
   		printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
 		fclose(tempFptr1);
   	 	return(FAILURE);
   	}

	retVal = allocateMemory(&lineFromTemp );
	if (retVal == FAILURE)
   	{
    	fclose(tempFptr1);
		fclose(tempFptr2);
    	return(FAILURE);
   	}
		
	if((file1EndRangeNum==0) && ( file2EndRangeNum==0))
		printf("%dc%d\n", file1LineNumber, file2LineNumber);						// Print the line number of FILE1 and FILE2.
	else if( file1EndRangeNum==0)
		printf("%dc%d,%d\n", file1LineNumber, file2LineNumber,  file2EndRangeNum);	// Print the range of line number of FILE1 and FILE2.
	else if( file2EndRangeNum==0)
		printf("%d,%dc%d\n", file1LineNumber,  file1EndRangeNum, file2LineNumber);	// Print the range of line number of FILE1 and FILE2.
	else
		printf("%d,%dc%d,%d\n", file1LineNumber,  file1EndRangeNum, file2LineNumber, file2EndRangeNum);	// Print the range of line number of FILE1 and FILE2.

	while(1)
   	{
		temp = readLineFromFile(tempFptr1, lineFromTemp);   // Read Line from tempFile1.
		if(temp == EOF)
			break;
		printf("<%s\n",lineFromTemp);						// Display the lines read from tempFile1.
		diffCount++;										// Compute the number of lines that are to be changed in FILE1.
   	}
	
	printf("- - -\n");										// This separates contents of tempFile1 and tempFile2.

	while(1)
   	{
		temp = readLineFromFile(tempFptr2, lineFromTemp);   // Read Line from tempFile2.
		if(temp == EOF) 
			break;
		printf(">%s\n",lineFromTemp);						// Display the lines read from tempFile1.
   	}

	fclose(tempFptr1);
	fclose(tempFptr2);
	free(lineFromTemp);
	return(diffCount);
}

/************************************************************************************************************
FUNCTION NAME : readRemainingLinesFromFile(filePtr1 or filePtr2, flag that tells which file has to be read )

THIS IS A COMMON FUNCTION WHICH IS CALLED WHEN EITHER FILE1 OR FILE2 REACHES EOF. IF ONE OF THE FILE REACHES 
EOF, IT READS THE REMAINING LINES PRESENT IN THE OTHER FILE AND STORES IT INTO THE RESPECTIVE tempFile.
*************************************************************************************************************/

int readRemainingLinesFromFile(FILE *fptr, int fileOneOrTwo)
{
    int fileRetVal=0, numOfRemLinesRead =0, retVal = SUCCESS;
    char *remLineFromFile=NULL;
    FILE *tempFptr = NULL;
    
	if(fileOneOrTwo == FILE_ONE) 
	   tempFptr=fopen("tempFile1.txt","w");			// Open tempFile1.
	else
	   tempFptr=fopen("tempFile2.txt","w");			// Open tempFile2.
		
    if(tempFptr == NULL)
	{
		printf("mydiff: Error in opening tempFile \"tempFile.txt\"\n");
		return(FAILURE);
	}

	retVal = allocateMemory(&remLineFromFile);
	if(retVal == FAILURE)
	{
        fclose(tempFptr);
        return(FAILURE);
	}

	while(1)
	{
            fileRetVal = readLineFromFile(fptr, remLineFromFile);   // Read remaining line from FILE1 or FILE2.
            if (fileRetVal == EOF)
				break ;	
            fprintf(tempFptr,"%s\n", remLineFromFile);				// Print the line read from FILE1 or FILE2.
			numOfRemLinesRead ++;									// Increment the count of remaining lines read from FILE1 or FILE2.
    } 

    fclose(tempFptr);		             
    free(remLineFromFile);
    return(numOfRemLinesRead );
}

/**********************************************************************************************
FUNCTION NAME : displayResults(fileName1, fileName2, total number of lines that differ in FILE1)

THIS FUNCTION IS CALLED AFTER ALL OTHER COMPUTATION IS DONE. IT DISPLAYS THE TOTAL NUMBER OF
LINES THAT DIFFER IN FILE1 WITH RESPECT TO FILE2 ALONG WITH THE RESPECTIVE FILE NAMES.
***********************************************************************************************/

void displayResults(char *fileName1, char *fileName2, int differingLineCount)
{
  if (differingLineCount== 0)
	printf("\nmydiff: NO DIFFERENCES FOUND\n");
  else
 	printf("\nmydiff: NUMBER OF LINES THAT ARE DIFFERNET IN \"%s\" WITH RESPECT TO \"%s\" IS %d\n",fileName1, fileName2,differingLineCount);  
}