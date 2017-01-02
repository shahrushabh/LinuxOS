/* t2.c
   synchronize threads through mutex and conditional variable 
   To compile use: gcc -o t2 t2.c -lpthread 
*/ 

#include <stdio.h>
#include <pthread.h>

void 	hello();    // define two routines called by threads    
void 	world();         	
void    again();

/* global variable shared by threads */
pthread_mutex_t 	mutex;  		// mutex
pthread_cond_t 		done_hello,done_world; 		// conditional variable
int 			h_done = 0, w_done = 0;	// testing variable

int main (int argc, char *argv[]){
    pthread_t 	tid_hello, tid_world, tid_again;	// Thread ids. 
    /*  initialization on mutex and cond variable  */ 
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&done_hello, NULL);
    pthread_cond_init(&done_world, NULL); 
    
    pthread_create(&tid_hello, NULL, (void*)&hello, NULL); //thread creation
    pthread_create(&tid_world, NULL, (void*)&world, NULL); //thread creation
    pthread_create(&tid_again, NULL, (void*)&again, NULL); //thread creation. 

    /* main waits for the two threads to finish */
    pthread_join(tid_hello, NULL);  
    pthread_join(tid_world, NULL);
    pthread_join(tid_again, NULL);

    printf("\n");
    return 0;
}

/* The hello function implementation. */
void hello() {
    pthread_mutex_lock(&mutex);
    printf("Hello ");
    fflush(stdout); 	// flush buffer to allow instant print out
    h_done = 1;
    pthread_cond_signal(&done_hello);	// signal world() thread
    pthread_mutex_unlock(&mutex);	// unlocks mutex to allow world to print
    return ;
}

/* World function implementation. */
void world() {
    pthread_mutex_lock(&mutex);

    /* world thread waits until done == 1. */
    while(h_done == 0) 
	pthread_cond_wait(&done_hello, &mutex);

    printf("World ");
    fflush(stdout);
    w_done = 1;
    pthread_cond_signal(&done_world);
    pthread_mutex_unlock(&mutex); // unlocks mutex
    return ;
}

/* The Again function Implementation. */
void again() {
    pthread_mutex_lock(&mutex);

    /* again threads will wait untill world threads is done. */
    while(w_done == 0) {
	pthread_cond_wait(&done_world, &mutex);
    }
    printf("Again!");
    fflush(stdout);
    pthread_mutex_unlock(&mutex);  // Unlock the mutex.
    return;
}
