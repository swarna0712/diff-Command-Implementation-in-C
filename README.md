# diff-Command-Implementation-in-C

" mydiff " is a program to mimic working of the linux command " diff " that compares files line by line.
It accepts multiple file names as input in the command line and compares first file with rest of the
files one by one.
It tells you the instructions on how to change the first file to make it match the file being compared 
currently in the list of inputs.
Apart from the output generally displayed by the " diff " command, this program outputs the number
of lines differed in the first file with respect to the file being compared in the list along with the file names.

The following are the constraints under which the program works properly:

1. It only works for text files, does not support directories and any other file types.
2. Ignores any blank line present between the lines in the text file.
3. Maximum line size is defined as 150 bytes. This can be increased beyond 150 bytes 
    by changing the value of LINE_SIZE in the preprocessor section, as the buffer used
    to read the line from the file is allocated memory dynamically.
