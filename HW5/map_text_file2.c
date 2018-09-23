#include <stdio.h>
#include <stdlib.h>  //exit()
#include <sys/types.h>  //open()
#include <sys/stat.h>  //open()
#include <fcntl.h>  //open()
#include <unistd.h>  //sleep()
#include <sys/mman.h>  //mmap(), munmap()
#include <time.h>  //time()

//-----define-----
#define SIZE_message 25  //max time message length
#define MAX_MESSAGE_NUMBER 10
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)  //handle error

//-----time message struct-----
typedef struct {
	char time[SIZE_message];
}time_message;

//map a text file as shared mem:
int main(int argc, char** argv) {
	int fd,i;
	time_message *t_map;
	int sleep_time;

	fd = open(argv[1], O_CREAT|O_RDWR, 00777);
	if(fd < 0)
		handle_error("open");

	t_map = (time_message*)mmap(NULL, sizeof(time_message)*MAX_MESSAGE_NUMBER, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	for(i = 0; i < MAX_MESSAGE_NUMBER; ++i) {
		sleep_time = rand()%2 + 1;  //random sec for waiting
		sleep(sleep_time);
		printf("Reading:\t%s", t_map[i].time);  //output file message
	}
	munmap(t_map, sizeof(time_message)*MAX_MESSAGE_NUMBER);
	printf("memory map Read end\n");

	return 0;
}

