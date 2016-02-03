/*
* cwade12c
* brutesh -- a bin to bruteforce shadow hashes up to 4 chars in length
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int countLines(char*, int);
void crack(char*);
int readLine(char*, int, int);
void writeLine(char*, const char*);


#define INPUT "pass.txt" //name of input file
#define OUTPUT "plainpass.txt" //name of output file

int const hash = 34; //hash length
int const SIZE = 1024; //buffer size

int main(void)
{
   int rfd, flags, i, count;
   char buffer[SIZE];
   char** arr;

   flags = O_RDONLY;
   rfd = open(INPUT, flags);

   count = countLines(buffer, rfd);

   lseek(rfd, 0, SEEK_SET); //rewind the file, http://linux.die.net/man/2/lseek

   arr = (char**) malloc(count * sizeof(char*));

   for(i = 0; i < count; i++)
   {
      arr[i] = (char*) malloc((hash+1) * sizeof(char));
   }

   i = 0;
   while(readLine(buffer, SIZE, rfd) >= hash) 
   {
      strcpy(arr[i], buffer);
      i++;
   }

   for(i = 0; i < count; i++)
   {
      crack(arr[i]);

      free(arr[i]);
      arr[i] = NULL;
   }

   free(arr);
   arr = NULL;

   close(rfd);

   return 1;
}

int countLines(char* buffer, int fd)
{
   int k;
   k = 0;

   while(readLine(buffer, SIZE, fd) >= hash) 
   {
      k++;
   }

   return k;
}

void crack(char* buffer) //only 4 maximum a-z supported
{
   char guess[] = { 'a', 'a', 'a', 'a', '\0' };

   for(guess[0] = 'a'; guess[0] < 'z' + 1; guess[0]++)
   {
      for(guess[1] = 'a'; guess[1] < 'z' + 1; guess[1]++)
      {
         for(guess[2] = 'a'; guess[2] < 'z' + 1; guess[2]++)
         {
            for(guess[3] = 'a'; guess[3] < 'z' + 1; guess[3]++)
            {
               if(strcmp(crypt(guess, buffer), buffer) == 0)
               {
                  writeLine(guess, OUTPUT);
                  //bust out the loops to save time
                  guess[3] = 'z';
                  guess[3] = guess[3] + 1;
                  guess[2] = 'z';
                  guess[2] = guess[2] + 1;
                  guess[1] = 'z';
                  guess[1] = guess[1] + 1;
                  guess[0] = 'z';
                  guess[0] = guess[0] + 1;
               }
            }
         }
      }
   }
}

void writeLine(char* line, const char* out)
{
   int wfd, flags, mode;

   flags = O_APPEND | O_CREAT | O_WRONLY;
   mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

   wfd = open(OUTPUT, flags, mode);

   write(wfd, line, strlen(line));
   write(wfd, "\n", 1);

   close(wfd);
}

int readLine(char* buffer, int bsize, int fd)
{
   int i, count;

   for(i = 0; i < bsize; i++)
   {
      count = read(fd, buffer, 1);

      if((buffer[0] == '\n') || count < 0)
      {
         buffer[0] = 0;
         close(count);
         return i;
      }

      buffer++;
   }
   close(count);

   return 1;
}
