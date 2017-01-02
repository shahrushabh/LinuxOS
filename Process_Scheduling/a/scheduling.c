/*******************************************************************************
*
* CprE 308 Scheduling Lab
*
* scheduling.c
*******************************************************************************/

#define NUM_PROCESSES 20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NONINIT -1
#define START 0
#define FINISHED 1

struct process
{
  /* Values initialized for each process */
  int arrivaltime;  /* Time process arrives and wishes to start */
  int runtime;      /* Time process requires to complete job */
  int priority;     /* Priority of the process */

  /* Values algorithm may use to track processes */
  int starttime;
  int endtime;
  int flag;
  int remainingtime;
};

/* Forward declarations of Scheduling algorithms */
void first_come_first_served(struct process *proc);
void shortest_remaining_time(struct process *proc);
void round_robin(struct process *proc);
void round_robin_priority(struct process *proc);

int main()
{
  int i;
  struct process proc[NUM_PROCESSES],      /* List of processes */
                 proc_copy[NUM_PROCESSES]; /* Backup copy of processes */

  /* Seed random number generator */
  /*srand(time(0));*/  /* Use this seed to test different scenarios */
  srand(0xC0FFEE);     /* Used for test to be printed out */

  /* Initialize process structures */
  for(i=0; i<NUM_PROCESSES; i++)
  {
    proc[i].arrivaltime = rand()%100;
    proc[i].runtime = (rand()%30)+10;
    proc[i].priority = rand()%3;
    proc[i].starttime = 0;
    proc[i].endtime = 0;
    proc[i].flag = 0;
    proc[i].remainingtime = 0;
  }

  /* Show process values */
  printf("Process\tarrival\truntime\tpriority\n");
  for(i=0; i<NUM_PROCESSES; i++)
    printf("%d\t%d\t%d\t%d\n", i, proc[i].arrivaltime, proc[i].runtime,
           proc[i].priority);

  /* Run scheduling algorithms */
  printf("\n\nFirst come first served\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  first_come_first_served(proc_copy);

  printf("\n\nShortest remaining time\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  shortest_remaining_time(proc_copy);

  printf("\n\nRound Robin\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin(proc_copy);
/*
  printf("\n\nRound Robin with priority\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin_priority(proc_copy);
*/

  return 0;
}

void first_come_first_served(struct process *proc)
{

  /* Implement scheduling algorithm here */

  int proc_arrival = 32766;
  int process_num;
  int totaltime = 1;
  int avgtime;
  for(int i=0; i<NUM_PROCESSES; i++) {
    int j = 0;
    while(j < NUM_PROCESSES) {
      if((proc[j].arrivaltime < proc_arrival) && (proc[j].flag == 0)){
        proc_arrival = proc[j].arrivaltime;
        process_num = j;
      }
      j++;
    }
    printf("Process %d started at time %d.\n", process_num, totaltime);
    totaltime += proc[process_num].runtime;
    printf("Process %d finished at time %d.\n", process_num, totaltime);
    proc[process_num].flag = 1;
    proc[process_num].endtime = totaltime;
    proc_arrival = 32767;
  }
  for(int i=0; i<NUM_PROCESSES; i++) {
    avgtime += (proc[i].endtime - proc[i].arrivaltime);
  }
  printf("Average time from arrival to finish is %d.\n", (avgtime/NUM_PROCESSES));
}

void shortest_remaining_time(struct process *proc)
{
  /* Implement scheduling algorithm here */

  int timecounter = 0;
  int avgtime = 0;
  int totaltime = 0;
  int j=0;
  int shortesttime = -32767;
  for(int i=0; i < NUM_PROCESSES; i++) {
    while(j < NUM_PROCESSES) {
      if((shortesttime < 0) && (proc[j].arrivaltime <= timecounter) && (proc[j].flag == 0)) {
        shortesttime = j;
      }
      else if((shortesttime >= 0) && (proc[j].runtime < proc[shortesttime].runtime) && 
             (proc[j].flag == 0) && (proc[j].arrivaltime <= timecounter)) {
        shortesttime = j;
      }
      j++;
    }

    if(shortesttime < 0) {
      timecounter++;
      i--;
    }
    else {
      proc[shortesttime].starttime = timecounter;
      proc[shortesttime].endtime = timecounter + proc[shortesttime].runtime;
      timecounter += proc[shortesttime].runtime;
      totaltime += proc[shortesttime].endtime - proc[shortesttime].starttime;
      proc[shortesttime].flag = 1;
      printf("Process %d started at time %d.\n", shortesttime, proc[shortesttime].starttime);
      printf("Process %d finished at time %d.\n", shortesttime, proc[shortesttime].endtime);
    }
    shortesttime = -32767;
    j=0;
  }
  for(int i=0; i<NUM_PROCESSES; i++) {
    avgtime += (proc[i].endtime - proc[i].arrivaltime);
  }
  printf("Average time from arrival to finish is %d.\n", (avgtime/NUM_PROCESSES));
}

void round_robin(struct process *proc)
{
  /* Implement scheduling algorithm here */

  /*
   * Code: (-1) = NONINIT, (0) = START, (1) = FINISHED.
   */

  for(int i=0; i<NUM_PROCESSES; i++) {
    proc[i].flag = NONINIT;
  }
  int timecounter = 0;
  int totaltime = 0;
  int done = 0;
  int runningprocess = 0;
  int init = 0;

  while(done != 1) {
    if((proc[runningprocess].arrivaltime <= timecounter) && (proc[runningprocess].flag != FINISHED)) {
      if((proc[runningprocess].remainingtime == 0) && (proc[runningprocess].flag != NONINIT)) {
        proc[runningprocess].endtime = timecounter;
        proc[runningprocess].flag = FINISHED;
        printf("Process %d started at time %d.\n", runningprocess, proc[runningprocess].starttime);
        printf("Process %d finished at time %d.\n", runningprocess, proc[runningprocess].endtime);
      }
      else {
        if(proc[runningprocess].flag == START) {
          proc[runningprocess].remainingtime -= 1;
        }
        else if(proc[runningprocess].flag == NONINIT) {
          proc[runningprocess].starttime = timecounter;
          proc[runningprocess].flag = START;
          proc[runningprocess].remainingtime = proc[runningprocess].runtime - 1;
        }
      }
      timecounter += 1;
      runningprocess += 1;
      if(runningprocess == NUM_PROCESSES) {
        runningprocess = 0;
      }
      init = runningprocess;
    }
    else {
      if(runningprocess == NUM_PROCESSES) {
        runningprocess = 0;
      }
      else if(runningprocess == init) {
        timecounter += 1;
        runningprocess += 1;
      }
      else {
        runningprocess += 1; 
      }
    }
    for(int i=0; i<NUM_PROCESSES; i++) {
      if(proc[i].flag != FINISHED) {
        break;
      } else {
        done = 1;
      }
    }
  }

  for(int i=0; i<NUM_PROCESSES; i++) {
    totaltime += proc[i].endtime - proc[i].arrivaltime;
  }
  printf("Average time from arrival to finish is %d.\n", totaltime/NUM_PROCESSES);
}

void round_robin_priority(struct process *proc)
{
  /* Implement scheduling algorithm here */
}