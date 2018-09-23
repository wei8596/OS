#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  //(intptr_t)
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

//=====define=====
#define N 5				//# of philosophers
#define LEFT (i+N-1)%N	//# of i's left neighbor
#define RIGHT (i+1)%N	//# of i's right neighbor
#define THINKING 0		//philosopher is thinking
#define HUNGRY	 1		//philosopher is trying to get forks
#define EATING	 2		//philosopher is eating

//macros to encapsulate the POSIX semaphore functions
#define semaphore_create(s, v) sem_init(&s, 0, v)  //initialize
#define down(s) sem_wait(s)  //lock
#define up(s) sem_post(s)  //unlock
typedef sem_t semaphore;

//=====function=====
void *philosopher(int i);  //i: philosopher number, from 0 to N-1
void take_forks(int i);
void put_forks(int i);
void test(int i);

void think(int i);
void eat(int i);
void print_state(void);  //show each philosopher's state

//=====global=====
int c = 0;			//for counting
int state[N];		//array to keep track of everyone's state
semaphore mutex;	//mutual exclusion for critical regions
semaphore s[N];		//one semaphore per philosopher

int main(void) {
	pthread_t tid[N];
	int k;
	//initialize
	for(k = 0; k < N; ++k) {
		semaphore_create(s[k], 1);
	}
	semaphore_create(mutex, 1);

	//create a thread for each philosopher
	for(k = 0; k < N; ++k) {
		if(pthread_create(&tid[k], NULL, (void*)philosopher, (void*)(intptr_t)k) != 0) {
			perror("pthread_create() error");
			exit(1);
		}
	}

	for(k = 0; k < N; ++k) {
		pthread_join(tid[k], NULL);  //waits for the thread specified by tid[k] to terminate
	}

	return 0;
}

void *philosopher(int i) {
	while(1) {  //repeat forever
		if(c < 4) {
			++c;
		}
		else {
			print_state();  //print the state
		}

		think(i);		//philosopher is thinking
		take_forks(i);	//acquire 2 forks or block
		eat(i);			//yum-yum, spaghetti
		put_forks(i);	//put both forks back on table
	}
	
	return (void*)0;
}

void take_forks(int i) {
	down(&mutex);		//enter critical region
	state[i] = HUNGRY;	//record fact that philosopher i is hungry
	test(i);			//try to acquire 2 forks
	up(&mutex);			//exit critical region
	down(&s[i]);		//block if forks were not acquired
}

void put_forks(int i) {
	down(&mutex);			//enter critical region
	state[i] = THINKING;	//philosopher has finished eating
	test(LEFT);				//see if left neighbor can now eat
	test(RIGHT);			//see if right neighbor can now eat
	up(&mutex);				//exit critical region
}

void test(int i) {
	if(state[i]==HUNGRY && state[LEFT]!=EATING && state[RIGHT]!=EATING) {
		state[i] = EATING;
		up(&s[i]);
	}
}

void think(int i) {
	state[i] = THINKING;
	int thinking_time = (rand() % 5) + 3;
	sleep(thinking_time);  //thinking
}

void eat(int i) {
	int eating_time = (rand() % 5) + 3;
	sleep(eating_time);
}

void print_state(void) {  //show each philosopher's state
	int k;
	for(k = 0; k < N; ++k) {
		if(state[k] == THINKING) {
			printf("THINKING  ");
		}
		else if(state[k] == HUNGRY) {
			printf("HUNGRY    ");
		}
		else if(state[k] == EATING) {
			printf("EATING    ");
		}
	}
	printf("\n");
}
