#include <mpi.h>
#include <stdio.h>

#include <mpi_monitor.h>

int main(int argc, char** argv) {
  timeout_config_t tconfig = {0};
  tconfig.ti.tv_nsec = 500000000;
  monitor_init(&tconfig);

  int number;
  int i;
  for(i = 0; i < 10; i++) {
    monitor_signal_iter(i);
  }

  monitor_signal_complete();
  monitor_finalize();
}
