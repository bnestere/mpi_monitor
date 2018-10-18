#ifndef __MPI_MONITOR__
#define __MPI_MONITOR__

#ifdef __cplusplus
#define EXTC extern "C"
#else
#define EXTC
#endif

/*
 * Initialize the monitor and consume the monitor process, allowing the rest of 
 * the processes to continue
 */
EXTC int monitor_init(void);

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
