#include <stdio.h>
int B[3] = [1, 2, 3];
int x = B[0] + 8;
int y = B[0];
B[0] = y;
x = B[0];
f = x + y;
