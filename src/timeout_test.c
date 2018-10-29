/*
 * This file tests the timeout functionality of the heartbeat monitor.
 *
 * It should fail on iteration 5.
 */
#include <mpi.h>
#include <stdio.h>
#include <time.h>

#include <mpi_monitor.h>

int main(int argc, char** argv) {
  timeout_config_t tconfig = {0};
  tconfig.thm.tv_nsec = 30000000;
  tconfig.ti.tv_nsec = 100000000;
  tconfig.tw.tv_nsec = 100000000;
  tconfig.tr.tv_nsec = 50000000;
  monitor_init(&tconfig);

  int number;
  int i;

  struct timespec ts ={0};
  //ts.tv_sec = 2;
  ts.tv_nsec = 50000000;

  for(i = 0; i < 10; i++) {
    monitor_signal_iter(i);
    ts.tv_nsec += 50000000;
    nanosleep(&ts, NULL);
  }
  monitor_signal_complete();
  monitor_finalize();
}
