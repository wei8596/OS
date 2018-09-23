/**
* reference: mmap man page example program source
*/
#include <stdio.h>
#include <stdlib.h>  //exit()
#include <sys/types.h>  //open()
#include <sys/stat.h>  //open()
#include <fcntl.h>  //open()
#include <unistd.h>  //lseek(), sleep()
#include <sys/mman.h>  //mmap(), munmap()
#include <time.h>  //time()
#include <string.h>  //memcpy()

//-----define-----
#define SIZE_message 25  //max time message length
#define MAX_MESSAGE_NUMBER 10
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)  //handle error

//-----time message struct-----
typedef struct {
	char time[SIZE_message];
}time_message;

//-----function-----
void message(void);

//-----global-----
char write_data[SIZE_message];  //write message for copy
int start;  //the starting time

//map a text file as shared mem:
int main(int argc, char** argv) {
	int fd,i;
	time_message *t_map;
	int sleep_time;

	//open file
	fd = open(argv[1], O_CREAT|O_RDWR|O_TRUNC, 00777);
	if(fd < 0)
		handle_error("open");
	/*
	*NAME
	*	lseek - reposition read/write file offset
	*SYNOPSIS
	*	off_t lseek(int fd, off_t offset, int whence);
	*DESCRIPTION
	*	SEEK_SET
	*		The file offset is set to offset bytes.
	*/
	lseek(fd, sizeof(time_message)*5-1, SEEK_SET);
	write(fd, "", 1);

	//void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
	//creates a new mapping in the virtual address space of the calling process
	//returns a pointer to the mapped area
	t_map = (time_message*) mmap(NULL, sizeof(time_message)*MAX_MESSAGE_NUMBER, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(t_map == MAP_FAILED)
		handle_error("mmap");
	close(fd);

	start = time(NULL);  //record the starting time
	//write mmap
	for(i = 0; i < MAX_MESSAGE_NUMBER; ++i) {
		message();  //calculate message
		printf("Writing:\t%s", write_data);
		memcpy(t_map[i].time, &write_data, SIZE_message);  //copy data in the memory to ptr_map's offset (write)
		sleep_time = rand()%2 + 1;  //random sec for waiting
		sleep(sleep_time);
	}
	sleep(10);
	munmap(t_map, sizeof(time_message)*MAX_MESSAGE_NUMBER);  //unmap the mmap
	printf("memory map Write end\n");

	return 0;
}

void message(void) {
	int end, diff;
	char *hour, *min, *sec;
	int h, m, s;
	char time_str[SIZE_message];
	char temp[SIZE_message] = "";
	
	end = time(NULL);
	diff = end - start;
	
	//calculate time hour:min:sec
	
	//split string
	strcpy(time_str, __TIME__);
	hour = strtok(time_str, ":");
	min = strtok(NULL, ":");
	sec = strtok(NULL, "");
	
	//string to integer
	h = atoi(hour);
	m = atoi(min);
	s = atoi(sec);
	s += diff;
	
	//check carry
	if(s >= 60) {
		m += (s/60);
		s %= 60;
		if(m >= 60) {
			h += (m/60);
			m %= 60;
		}
	}
	
	sprintf(temp, "%2.2d:%2.2d:%2.2d\n", h, m, s);
	strcpy(write_data, temp);
}

