/*
 * Atiya Kailany
 * December 3, 2019
 * File System Measurements
 *
 */

//import the needed libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>


// a set of define values used in the functions
#define BLOCK_SIZE 4096
#define _512_ 512000
#define _256_ 256000
#define _4096_ 4096000
#define _1MG_ 1000000
#define _1GB_ 1000000000
#define conversion 3591965958




//explicitly insintiating write(), read() and fsync()
extern ssize_t write (int __fd, const void *__buf, size_t __n);
ssize_t read(int fd, void *buf, size_t count);
int fsync(int fildes);

//allocating 2 bytes for rdtsc()
static __inline__ unsigned long long rdtsc(void) {

  unsigned long long int x;
  __asm__ volatile (" .byte 0x0f, 0x31" : "=A" (x));
  return x;
}

//global variables for file path and computing time
int size, i, fd;
char * filename = "/var/tmp/akailany/Large1";

unsigned long long start, end;
double timeSpent;


void findCacheSize() {

  //allocate memory
  char *buff = malloc(BLOCK_SIZE * sizeof(char));

  //open file to read
  fd = open(filename, O_RDONLY);

    // Repeated reads of a group of blocks
    // Each group of blocks is approximatley 1 GB
    // Loop 20 times until cache is full and spike occurs
    for(i=0; i<20; i++) {
      start = rdtsc(); //record start time
      size = read(fd, buff, _1GB_);
      end = rdtsc(); //stop recording

      //convert rdtsc to microseconds and assign into timeSpent
      timeSpent = (((double)(end - start) / conversion) * 1000000);

      //print iteration number and its corresponding time
      printf("Iteration %d", i+1);
      printf(" Cache Size: %f\n", timeSpent);
    }//for

  //close file when done
  close(fd);

}//findCacheSize()



void readBlockSize() {
  //allocate memory
  char *buff = malloc(BLOCK_SIZE * sizeof(char));

  //open file to read
  fd = open(filename, O_RDONLY);

  //read 256 bytes for 100 iterations to observe where the spikes occur
  //the distance between every 2 spikes is how big the block is
  for(i=0; i<100; i++) {
    start = rdtsc(); //record start time
    size = read(fd, buff, _256_);
    end = rdtsc(); //stop recording

    //convert rdtsc to microseconds and assign into timeSpent
    timeSpent = (((double)(end - start) / conversion) * 1000000);

    //print iteration number and its corresponding time
    printf("Iteration %d", i+1);
    printf(" Block Size: %f\n", timeSpent);
  }//for

  //close file when done
  close(fd);


  //Using stat library to find correct answer and verify our experiment
    struct stat fi;
    stat("/", &fi);
    printf("Correct Block Size: %d\n", fi.st_blksize);

}//readBlockSize()


void prefetchedData() {
  //allocate memory
  char *buff = malloc(BLOCK_SIZE * sizeof(char));

  //open file to read
  fd = open(filename, O_RDONLY);

  //This reads one block (4096) at a time, and the time is
  //recorded for each read.
  for(i=0; i<50; i++) {
    start = rdtsc(); //record start time
    size = read(fd, buff, _4096_);
    end = rdtsc(); //stop recording

    //convert rdtsc to microseconds and assign into timeSpent
    timeSpent = (((double)(end - start) / conversion) * 1000000);

    //print iteration number and its corresponding time
    printf("Iteration %d", i+1);
    printf(" Prefetched Data Size: %f\n", timeSpent);
  }//for

  //close file when done
  close(fd);


}//prefetchedData()


void extentTime() {

  //allocate memory
  char *buff = malloc(BLOCK_SIZE * sizeof(char));

  //open file to read
  fd = open(filename, O_RDONLY);

  //write 1 megabyte at a time and then sync it
  //time is recorded for each sync, each sync is
    // 1 MG; loops for 100 iterations (100MG)
  for(i=0; i<100; i++) {
    size = write(fd, buff, _1MG_);
    start = rdtsc(); //record start time
    size = fsync(fd);
    end = rdtsc(); //stop recording

    //convert rdtsc to microseconds and assign into timeSpent
    timeSpent = (((double)(end - start) / conversion) * 1000000);

    //print iteration number and its corresponding time
    printf("Iteration %d", i+1);
    printf(" Extent Size: %f\n", timeSpent);
  }//for

  //close file when done
  close(fd);
}


int main() {

//call the above functions, the iteration and
//corresponding time for each will be
//printed on the terminal
findCacheSize();
readBlockSize();
prefetchedData();
extentTime();

}//end main
