#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_PROCESSES 5
#define EP 0.000001

typedef struct {
    char name;  // 进程名
    float arrival_time;  // 到达时间
    float service_time;  // 服务时间
    float remaining_time;  // 剩余时间
    float completion_time;  // 完成时间
    float turnaround_time;  // 周转时间
    float weighted_turnaround_time;  // 带权周转时间
} Process;

// 抢占的短作业优先算法
void calculate_SJF(Process processes[], int n) {
    int completed = 0, min_index = -1;  // 已完成进程数, 最优先进程
    float current_time = 0;  // 当前时间
    int is_completed[MAX_PROCESSES];
    // 初始化
    for (int i = 0; i < MAX_PROCESSES; i++) {
        is_completed[i] = 0;
    }

    while (completed < n) {
        // 寻找已到达的进程中的最优先进程
        if (min_index != -1 && fabs(processes[min_index].remaining_time - 0) < EP) {
            min_index = -1;
            for (int i = 0; i < n; i++) {
                if (processes[i].arrival_time <= current_time &&  // 该进程已到来
                    !is_completed[i] &&  // 该进程未完成
                    (min_index == -1 || processes[i].remaining_time < processes[min_index].remaining_time)) {  // 当前未确定最优先进程或该进程优先级更高
                    min_index = i;  // 确定该进程为最优先进程
                }
            }   
        }

        // 没有找到当前最优先进程, 说明当前没有进程到来, 寻找一个最先到达的进程
        if (min_index == -1) {
            float min_arrival_time = processes[0].arrival_time;
            min_index = 0;
            for (int i = 0; i < n; i++) {
                if (processes[i].arrival_time < min_arrival_time && !is_completed[i]) {
                    min_arrival_time = processes[i].arrival_time;
                    min_index = i;
                }
            }
            current_time = min_arrival_time;
        }

        // 下一个时间节点(当前进程处理完或者被抢占)
        float next_time = current_time + processes[min_index].remaining_time;
        int prior_index = min_index;

        for (int i = 0; i < n; i++) {
            // 如果有进程在当前进程处理完成之前到达
            if (processes[i].arrival_time < next_time && processes[i].arrival_time > current_time && !is_completed[i]  // 已到达, 未完成
            ) {
                // 并且剩余时间更短(优先级更高)
                if (processes[i].remaining_time < next_time - processes[i].arrival_time) {
                    if (prior_index == min_index) prior_index = i;
                    else if (processes[i].remaining_time < processes[prior_index].remaining_time) {
                        prior_index = i;
                    }
                }
            }
        }
        // 如果不需要抢占, 完成当前进程, 计算周转时间和带权周转时间
        if (prior_index == min_index) {  
            current_time = next_time;
            processes[min_index].completion_time = current_time;
            processes[min_index].turnaround_time = processes[min_index].completion_time - processes[min_index].arrival_time;
            processes[min_index].weighted_turnaround_time = (float)processes[min_index].turnaround_time / processes[min_index].service_time;
            processes[min_index].remaining_time = 0;
            is_completed[min_index] = 1;
            completed++;
        }
        // 抢占
        else {  
            current_time = processes[prior_index].arrival_time;
            processes[min_index].remaining_time = next_time - current_time;  // 记录被强占进程的剩余时间
            min_index = prior_index;
        }
    }
}

// 时间片轮转算法
void calculate_RR(Process processes[], int n, int time_quantum) {
    int completed = 0, current_time = 0;
    int is_completed[MAX_PROCESSES];
    // 初始化
    for (int i = 0; i < MAX_PROCESSES; i++) {
        is_completed[i] = 0;
    }

    while (completed < n) {
        int found_process = 0;

        for (int i = 0; i < n; i++) {
            // 如果该进程已到达且没有完成
            if (processes[i].arrival_time <= current_time && !is_completed[i]) {
                found_process = 1;
                if (processes[i].remaining_time > 0) {
                    if (processes[i].remaining_time > time_quantum) {
                        current_time += time_quantum;
                        processes[i].remaining_time -= time_quantum;
                    } else {
                        current_time += processes[i].remaining_time;
                        processes[i].completion_time = current_time;
                        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                        processes[i].weighted_turnaround_time = (float)processes[i].turnaround_time / processes[i].service_time;
                        processes[i].remaining_time = 0;
                        is_completed[i] = 1;
                        completed++;
                        break;
                    }
                }
            }
        }

        // 若当前时间没有任何进程到达，直接推进到下一个到达时间
        if (!found_process) {
            int next_arrival_time = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (!is_completed[i] && processes[i].arrival_time > current_time) {
                    next_arrival_time = fmin(next_arrival_time, processes[i].arrival_time);
                }
            }
            // 如果找到了下一个到达时间，则将当前时间推移到下一个到达时间
            if (next_arrival_time != INT_MAX) {
                current_time = next_arrival_time;
            }
        }
    }
}

void print_results(Process processes[], int n) {
    float total_turnaround_time = 0.0, total_weighted_turnaround_time = 0.0;
    
    printf("进程  完成时间  周转时间  带权周转时间\n");
    for (int i = 0; i < n; i++) {
        printf("%c     %.2f     %.2f     %.2f\n", 
                processes[i].name, 
                processes[i].completion_time, 
                processes[i].turnaround_time, 
                processes[i].weighted_turnaround_time);
        
        total_turnaround_time += processes[i].turnaround_time;
        total_weighted_turnaround_time += processes[i].weighted_turnaround_time;
    }

    printf("平均周转时间: %.2f\n", total_turnaround_time / n);
    printf("平均带权周转时间: %.2f\n", total_weighted_turnaround_time / n);
}

int main() {
    Process processes[MAX_PROCESSES] = {
        {'A', 0, 3, 3, 0, 0, 0.0},
        {'B', 2, 6, 6, 0, 0, 0.0},
        {'C', 4, 4, 4, 0, 0, 0.0},
        {'D', 6, 5, 5, 0, 0, 0.0},
        {'E', 8, 2, 2, 0, 0, 0.0}
    };

    // 运行 SJF 调度
    printf("抢占的短作业优先 (SJF) 调度:\n");
    calculate_SJF(processes, MAX_PROCESSES);
    print_results(processes, MAX_PROCESSES);
    
    // 重新初始化进程剩余时间
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].remaining_time = processes[i].service_time;
        processes[i].completion_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].weighted_turnaround_time = 0.0;
    }

    // 运行 RR 调度
    printf("\n时间片轮转 (RR) 调度:\n");
    calculate_RR(processes, MAX_PROCESSES, 1);
    print_results(processes, MAX_PROCESSES);

    return 0;
}