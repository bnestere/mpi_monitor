#include <mpi.h>
#include <stdio.h>
#include <time.h>

#include "mpi_monitor.h"

int monitor_finalize() {
  MPI_Finalize();
}

#define BILLION 1000000000
void timespec_add(struct timespec *result, struct timespec *t1, struct timespec *t2)
{
  unsigned long sec = t2->tv_sec + t1->tv_sec;
  unsigned long nsec = t2->tv_nsec + t1->tv_nsec;

  if (nsec >= BILLION) {
    nsec -= BILLION;
    sec++;
  }

  result->tv_sec = sec;
  result->tv_nsec = nsec;

//  return struct timespec{ .tv_sec = sec, .tv_nsec = nsec };
}

/* 
 * Calculate the overall timeout described by a timeout_config_t
 * Parameters:
 *   save: The timespec to save the overall timeout in
 *   tconfig: The timeout config
 */
void calculate_timeout(struct timespec *save, timeout_config_t *tconfig) {
  timespec_add(save, &tconfig->thm, &tconfig->ti);
  timespec_add(save, save, &tconfig->tr);
  timespec_add(save, save, &tconfig->tw);

  //save->tv_sec = tconfig->thm.tv_sec + tconfig->ti.tv_sec 
  //  + tconfig->tr.tv_sec + tconfig->tw.tv_sec;
  //save->tv_nsec = tconfig->thm.tv_nsec + tconfig->ti.tv_nsec 
  //  + tconfig->tr.tv_nsec + tconfig->tw.tv_nsec;
}

int before(struct timespec *t1, struct timespec *t2) {
  if(t1->tv_sec == t2->tv_sec) {
    return t1->tv_nsec < t2->tv_nsec;
  } else {
    return t1->tv_sec < t2->tv_sec;
  }
}

void print_time(struct timespec *ts, const char *prefix) {
  printf("%s: %lld.%.9ld\n", prefix, ts->tv_sec, ts->tv_nsec); 
}

int monitor_init(timeout_config_t *timeout_config) {
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  //Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  printf("monitor_init()::Initialized, world_size = %d, world_rank = %d\n", world_size, world_rank);

  MPI_Status status;
  MPI_Request request;
  struct timespec ts;
  ts.tv_sec = 1;
  ts.tv_nsec = 0;

  struct timespec full_timeout, current_time, next_timeout;
  calculate_timeout(&full_timeout, timeout_config);

  if(world_rank == 1) {
    int number;
    int last_number;
    printf("monitor_init()::Waiting for program initialization...\n");
    nanosleep(&timeout_config->program_init, NULL);
    printf("monitor_init()::MONITOR - Starting wait loop\n");
    while(number > -1) {
      last_number = number;
      printf("monitor_init()::MONITOR - Waiting for number..\n");
      //MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Irecv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
      printf("monitor_init()::Waiting for message...\n");

      clock_gettime(CLOCK_MONOTONIC_RAW, &current_time);
      ///print_time(&current_time, "current");
      timespec_add(&next_timeout, &full_timeout, &current_time);

      print_time(&full_timeout, "fulltime");
      print_time(&current_time, "current");
      print_time(&next_timeout, "nexttime");

      // Assume timeout is true to start and reset once the heartbeat is received
      int timed_out = 1; 
      while(before(&current_time, &next_timeout)) {
        int flag = 0;
        MPI_Test(&request, &flag, &status);
        if(flag) {
          printf("monitor_init()::Received message...\n");
          timed_out = 0;
          break;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &current_time);
      }
      if(timed_out == 1) {
        printf("monitor_init()::ERROR, Timeout hit\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
        monitor_finalize();
        exit(-1);
      }
      

      printf("monitor_init()::MONITOR - MPI Monitor received number %d from process 0\n", number);
    }
    printf("monitor_init()::MONITOR - MPI Monitor received exit signal %d\nLast iteration %d\n", number, last_number);
    monitor_finalize();
    exit(0);
  }

  return world_rank;
}


int monitor_signal_iter(int iteration_no) {
  printf("monitor_signal_iter()::Sending number %d\n", iteration_no);
  MPI_Send(&iteration_no, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
}

int monitor_signal_complete() {
  int number = -1;
  printf("monitor_signal_complete()::Sending signal complete %d\n", number);
  MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
}
