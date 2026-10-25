#include <stdio.h>
#include "../include/metrics.h"

void metrics_print_global(int total_ticks, int num_completed_tasks, tcb_t** tasks) {
    double throughput = 0.0;
    double avg_wait = 0.0;
    int total_wait_time = 0;

    for (int i = 0; i < num_completed_tasks; i++) {
        if(tasks[i] != NULL && tasks[i]->execState == FINISHED){
            total_wait_time += tasks[i]->waitTime;
        }
    }

    if (total_ticks > 0) {
        throughput = (double)num_completed_tasks / total_ticks;
    }
    
    if (num_completed_tasks > 0) {
        avg_wait = (double)total_wait_time / num_completed_tasks;
    }

    printf("\n====== GLOBAL PERFORMANCE METRICS ====================\n");
    printf("  - Total Simulation Time : %d clock ticks.\n", total_ticks);
    printf("  - System Throughput     : %.2f sentinels/tick.\n", throughput);
    printf("  - Average Waiting Time  : %.2f ticks.\n", avg_wait);
    printf("======================================================\n");
}