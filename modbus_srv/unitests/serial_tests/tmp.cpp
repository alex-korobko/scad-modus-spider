#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (char *argv[],int argc)
{
time_t time_tt;
/*
char *buffer, ex;
j=0;
printf ("i=%d",(int) 10/115200*1.5*5);
return 1;
*/
printf ("\nSize of time_t %d", sizeof(time_tt));
time_tt=time(NULL);

printf ("time_t %d", time_tt);

return EXIT_SUCCESS;
}
