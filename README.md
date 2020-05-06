## Simple-Memory-Manager
Simple Memory Manager with insert, remove and optimize operations.
Memory is divided into 8 blocks of equal size.
At the end program will estimate free space of each block.

Input example:
1.line is n  - number of commands. If number of commads is 0, then finish program.
2.line is size of memory
Next n lines is command insert, remove or optimize, commands are in format:
 - insert fileName fileSize
 - remove fileName
 - optimize

3
8KB
insert f1 7KB
insert f2 3MB
remove f1
6
8Mb
insert f1 4MB
insert f2 1MB
insert f3 512KB
remove f1
remove f1
insert f1 5MB
0

Example output:
Error is printed if we tried to insert file bigger than free space.
If there wasn't error, estimation of free space in each block is printed in next format:
[C][C][C][C][C][C][C][C]

C can have 3 values:
 - ' ' if free space is between 75% and 100% of block size,
 - '-' if free space is between 25% and 75% of block size,
 - '#' if free space is between 0% and 25% of block size.
 
ERROR: Disk is full! 
[#][#][#][#][#][#][-][ ] 
