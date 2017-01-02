#include<stdio.h>
#include<pthread.h>

/* Prototypes for the two functions. */
void* thread1();
void* thread2();

int main (int argc, char *argv[])
{
  /* Threads declaration. */
  pthread_t i1;
  pthread_t i2;

  /* thread creation */
  pthread_create(&i1, NULL, (void*)&thread2, NULL);
  pthread_create(&i2, NULL, (void*)&thread1, NULL);

  pthread_join(i1, NULL);
  pthread_join(i2, NULL); 

  /* main waits for the two threads to finish */
  printf("This is the main process.\n");
}

  /* Thread 1 implementation. */
void* thread1() {
  sleep(5);
  printf("This is thread 1 running.\n");
}
  /* Thread 2 implementation. */
void* thread2() {
  sleep(5);
  printf("This is thread 2 running.\n");
}

