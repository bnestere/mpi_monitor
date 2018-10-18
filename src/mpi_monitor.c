#include <mpi.h>
#include <stdio.h>

#include "mpi_monitor.h"

int monitor_finalize() {
  MPI_Finalize();
}

int monitor_init() {
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

  if(world_rank == 1) {
    printf("monitor_init()::MONITOR - Starting wait loop\n");
    int number;
    int last_number;
    while(number > -1) {
      last_number = number;
      printf("monitor_init()::MONITOR - Waiting for number..\n");
      MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
