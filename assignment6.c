#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
/*definititions*/
#define PHILNUM 5
#define EATTIME 10
int randomGaussian(int mean, int stddev);
void* dinnerTime();

/*global variables*/
int chopsticks[PHILNUM];
int id;
pthread_mutex_t idMutex;
pthread_mutex_t chopsticksMutex[PHILNUM];
/*main function*/
int main(int argc, char *argv[]) {
	int pNum = 0;
	srand(time(NULL));
	pthread_mutex_init(&idMutex, NULL);
	pthread_t philosophers[PHILNUM];
	for(int i = 0; i < PHILNUM; i++) { //initalizes the mutex
		int foo = pthread_mutex_init(&chopsticksMutex[i], NULL);
		if(foo != 0) {
			perror("");
			exit(1);
		}
	}
	for(int i = 0; i < PHILNUM; i++) { //creates the threads
		int foo = pthread_create(&philosophers[i], NULL, &dinnerTime, NULL);
                if(foo != 0) {
                        perror("");
                        exit(1);
                }
	}
	for(int i = 0; i < PHILNUM; i++) { //waits for threads to finish
		int foo = pthread_join(philosophers[i], NULL);
                if(foo != 0) {
                        perror("");
                        exit(1);
                }
	}
	for(int i = 0; i < PHILNUM; i++) { //destroys the mutexes
		int foo = pthread_mutex_destroy(&chopsticksMutex[i]);
                if(foo != 0) {
                        perror("");
                        exit(1);
                }

	}
	return 0;
}

void* dinnerTime() {
	/* totEating tracks the time they eat */
	int totEating = 0;
	/*Initializes the ID and chopstick values for this thread*/
	pthread_mutex_lock(&idMutex);
	int x = id;
	int chopOne = id;
	int chopTwo = (id + 1) % PHILNUM;
	id++;
	pthread_mutex_unlock(&idMutex);
	printf("Philosopher %d is thinking...\n",x);
	fflush(stdout);
	/*Actually handles eating loop*/
	while(totEating < EATTIME) { //Ensures the threads eat for the required time
		if(pthread_mutex_lock(&chopsticksMutex[chopOne])==0) {
			if(pthread_mutex_trylock(&chopsticksMutex[chopTwo])==0) {
				int dt = randomGaussian(9,3);
				printf("Philosopher %d is eating for %d seconds\n",x,dt);
				fflush(stdout);
				sleep(dt);
				totEating += dt;
				printf("Philosopher %d is putting down their chopsticks\n",x);
				fflush(stdout);
				pthread_mutex_unlock(&chopsticksMutex[chopTwo]);
				printf("Philosopher %d is thinking....\n",x);
				fflush(stdout);
			}
			pthread_mutex_unlock(&chopsticksMutex[chopOne]);
		}
	}
	printf("Philosopher %d has finished their meal.\n Total Eating time: %ds\n",x,totEating);
	fflush(stdout);
}

/*randomGaussian function provided by professor*/
int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}
