#ifndef __MPI_MONITOR__
#define __MPI_MONITOR__

#ifdef __cplusplus
#define EXTC extern "C"
#else
#define EXTC
#endif

#include <time.h>

/*
 * Defines the expected timeouts for the heartbeat monitor
 * Field Descriptions:
 *  thm - Time for a message to reach the heartbeat monitor
 *  ti - The interval between sending heartbeats
 *  tr - Timeout for the heartbeat monitor to realize it hasn't received
 *      a message from a node
 *  tw - The wait time for the heartbeat monitor to declare a node is dead
 */
typedef struct _timeout_config {
  struct timespec thm;
  struct timespec ti;
  struct timespec tr;
  struct timespec tw;
} timeout_config_t;

/*
 * Initialize the monitor and consume the monitor process, allowing the rest of 
 * the processes to continue
 */
EXTC int monitor_init(timeout_config_t *);

/*
 * Finalize and perform clean up for the monitor
 */
EXTC int monitor_finalize(void);

/*
 * Signal the monitor that we have progressed to the iteration iteration_no
 */
EXTC int monitor_signal_iter(int ieration_no);

/*
 * Signal the monitor that we have finished all computation and are ready to exit the process
 */
EXTC int monitor_signal_complete(void);

#undef EXTC

#endif
