#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

const int max_elves = 9;
const int max_reindeers = 9;
int elves = 0;
int reindeers = 0;
sem_t mutex;
sem_t reindeer_mutex;
sem_t elf_mutex;
sem_t santa_mutex;

void *SantaClaus(void*);
void *Reindeer(void*);
void *Elf(void*);

int main(){
    sem_init(&mutex, 0, 1);
    sem_init(&reindeer_mutex, 0, 0);
    sem_init(&elf_mutex, 0, 1);
    sem_init(&santa_mutex, 0, 0);
    
    pthread_t t_santa;
    pthread_create(&t_santa, NULL, SantaClaus, NULL);
    
    pthread_t reindeer[max_reindeers];
    pthread_t elve[max_elves];
    int i;
    for(i=0; i<max_reindeers; i++){
        int reindeer_id = i+1;
        int elf_id = i+1;
        if(pthread_create(reindeer+i, NULL, Reindeer, &reindeer_id) != 0){
            perror("Failed to create reindeer thread");
            return 1;
        }
        if(pthread_create(elve+i, NULL, Elf, &elf_id) != 0){
            perror("Failed to create elf thread");
            return 1;
        }
    }
    pthread_join(t_santa, NULL);
    
    return 0;
}

void *SantaClaus(void *arg){
    printf("Santa Claus: ZZzzzz\n");
    while(1){
        sem_wait(&santa_mutex);
        sem_wait(&mutex);
        
        if(reindeers == max_reindeers){
            printf("Santa Claus preparing sleighs\n");
			for (int r = 0; r < max_reindeers; r++)
				sem_post(&reindeer_mutex);
            sleep(10);
            printf("Time to make kids happy Hohoho!!\n");
            reindeers = 0;
        }else if(elves == 3){
            printf("Hey my elves! What is the problem?\n");
        }
        sem_post(&mutex);
    }
}

void *Reindeer(void *arg){
    int *temp = (int *)arg;
    int reindeer_id = *temp;
    printf("Reindeer %d arrived to the hall! \n", reindeer_id);
    sleep(3);
    while(1){
        sem_wait(&mutex);
		reindeers++;
		if (reindeers == max_reindeers)
			sem_post(&santa_mutex);
		sem_post(&mutex);
		sem_wait(&reindeer_mutex);
		printf("Reindeer %d getting hitched! \n", reindeer_id);
		sleep(20);
    }
    return NULL;
}

void *Elf(void *arg){
    int *temp = (int *)arg;
    int elf_id = *temp;
    printf("Elf %d is working. \n", elf_id);
    while(1){
        if(rand() % 3 == 0){
            sem_wait(&elf_mutex);
            sem_wait(&mutex);
             elves++;
            if (elves == 3)
				sem_post(&santa_mutex);
			else
				sem_post(&elf_mutex);
			sem_post(&mutex);
            printf("Elf %d arrived to the hall for asking help! \n", elf_id);
            sleep(10);

            sem_wait(&mutex);
			elves--;
			if (elves == 0)
				sem_post(&elf_mutex);
			sem_post(&mutex);
        }
        sleep(3 + rand() % 7);
    }
    return NULL;
}
