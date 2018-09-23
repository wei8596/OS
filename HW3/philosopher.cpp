#include <iostream>
#include <cstdlib>
#include <stdint.h>  //(intptr_t)
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

using namespace std;
//=====define=====
#define N 5				//# of philosophers
#define LEFT (i+N-1)%N	//# of i's left neighbor
#define RIGHT (i+1)%N	//# of i's right neighbor
#define THINKING 0		//philosopher is thinking
#define HUNGRY	 1		//philosopher is trying to get forks
#define EATING	 2		//philosopher is eating

//=====class=====
class monitor{
public:
	void ini(void);			//initialize
	void take_forks(int i);	//i: philosopher number, from 0 to N-1
	void put_forks(int i);
	void test(int i);
	void think(int i);
	void eat(int i);
	void print_state(void); //show each philosopher's state

private:
	int state[N];			//array to keep track of everyone's state
	pthread_mutex_t mutex;	//mutual exclusion for critical regions
	pthread_cond_t s[N];
};

//=====function=====
void *philosopher(int i);  //i: philosopher number, from 0 to N-1

//=====global=====
monitor m;
int c = 0;  //for counting

int main(void) {
	pthread_t tid[N];
	int k;

	//initialize
	m.ini();
	//create a thread for each philosopher
	for(k = 0; k < N; ++k) {
		if(pthread_create(&tid[k], NULL, (void* (*)(void*))philosopher, (void*)(intptr_t)k) != 0) {
			cerr << "pthread_create() error";
			exit(1);
		}
	}

	for(k = 0; k < N; ++k) {
		pthread_join(tid[k], NULL);  //waits for the thread specified by tid[k] to terminate
	}

	return 0;
}

//initialize
void monitor::ini() {
	pthread_mutex_init(&mutex, NULL);
    for(int i = 0; i < N; ++i) {
	    pthread_cond_init(&s[i], NULL);
	}
}

void monitor::take_forks(int i) {
	pthread_mutex_lock(&mutex);		//enter critical region
	state[i] = HUNGRY;				//record fact that philosopher i is hungry
	test(i);						//try to acquire 2 forks
	pthread_mutex_unlock(&mutex);	//exit critical region
}

void monitor::put_forks(int i) {
	pthread_mutex_lock(&mutex);		//enter critical region
	state[i] = THINKING;			//philosopher has finished eating
	pthread_cond_signal(&s[LEFT]);  //叫左邊叫醒等待吃的人
    pthread_cond_signal(&s[RIGHT]); //叫右邊叫醒等待吃的人
	pthread_mutex_unlock(&mutex);	//exit critical region
}

void monitor::test(int i) {  //等於兩個迴圈,當條件不符時,就會進入pthread_cond_wait繼續等待
	while(!(state[i]==HUNGRY && state[LEFT]!=EATING && state[RIGHT]!=EATING)) {
		pthread_cond_wait(&s[i], &mutex);  //&mutex當左邊及右邊同時叫醒時,就依序左再右,可防止同時運時造成delock
	}
	state[i] = EATING;
}

void monitor::think(int i) {
	state[i] = THINKING;
	int thinking_time = (rand() % 5) + 3;
	sleep(thinking_time);  //thinking
}

void monitor::eat(int i) {
	int eating_time = (rand() % 5) + 3;
	sleep(eating_time);  //eating
}

void monitor::print_state(void) {  //show each philosopher's state
	for(int k = 0; k < N; ++k) {
		if(state[k] == THINKING) {
			cout << "THINKING  ";
		}
		else if(state[k] == HUNGRY) {
			cout << "HUNGRY    ";
		}
		else if(state[k] == EATING) {
			cout << "EATING    ";
		}
	}
	cout << endl;
}

void *philosopher(int i) {
	while(1) {  //repeat forever
		if(c < 4) {
			++c;
		}
		else {
			m.print_state();	//print the state
		}

		m.think(i);				//philosopher is thinking
		m.take_forks(i);		//acquire 2 forks or block
		m.eat(i);				//yum-yum, spaghetti
		m.put_forks(i);			//put both forks back on table
	}
	pthread_exit(NULL);
}
