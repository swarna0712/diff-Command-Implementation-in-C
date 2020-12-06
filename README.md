# diff-Command-Implementation-in-C

" mydiff " is a program to mimic working of the linux command " diff " that compares files line by line.
It accepts multiple file names as input in the command line and compares first file with rest of the
files one by one.
It tells you the instructions on how to change the first file to make it match the file being compared 
currently in the list of inputs.
Apart from the output generally displayed by the " diff " command, this program outputs the number
of lines differed in the first file with respect to the file being compared in the list along with the file names.
