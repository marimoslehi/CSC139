#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For exit() and strcmp()

#define MAX_TASKS 100 // Maximum number of tasks

// Struct to represent a task
struct Task {
    int Process_Number;
    int arrival_time;
    int cpu_burst;
    int priority;
    int waiting_time; // Added waiting time here
};

// Function prototypes
void RoundRobin(struct Task tasks[], int num_tasks, int quantum);
void SJF(struct Task tasks[], int num_tasks);
void Priority_No_Preemption(struct Task tasks[], int num_tasks);
void Priority_With_Preemption(struct Task tasks[], int num_tasks); // Corrected spelling
void sort_by_cpu_burst(struct Task tasks[], int num_tasks);
//float calculate_average_waiting_time(struct Task tasks[], int num_tasks);
float print_and_calculate_average_waiting_time(struct Task tasks[], int num_tasks);
void initialize_waiting_times(struct Task tasks[], int num_tasks);

int main(int argc, char *argv[]) {
    FILE *file;
    struct Task tasks[MAX_TASKS];
    int num_tasks, task_id, arrival_time, cpu_burst, priority, time_quantum = 0;

    if (argc < 2) {
        printf("Usage: %s <inputfile>\n", argv[0]);
        exit(1);
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file %s\n", argv[1]);
        return 1;
    }

    char algoName[25];
    fscanf(file, "%s", algoName);

    if (strcmp(algoName, "RR") == 0) {
        fscanf(file, "%d", &time_quantum);
    }

    fscanf(file, "%d", &num_tasks);

    for (int i = 0; i < num_tasks; i++) {
        fscanf(file, "%d %d %d %d", &task_id, &arrival_time, &cpu_burst, &priority);
        tasks[i].Process_Number = task_id;
        tasks[i].arrival_time = arrival_time;
        tasks[i].cpu_burst = cpu_burst;
        tasks[i].priority = priority;
        tasks[i].waiting_time = 0; // Initialize waiting times
    }

    fclose(file);

    printf("Algorithm Name: %s\n", algoName);
    if (strcmp(algoName, "RR") == 0) {
        printf("Time Quantum: %d\n", time_quantum);
        RoundRobin(tasks, num_tasks, time_quantum);
    } else if (strcmp(algoName, "SJF") == 0) {
        SJF(tasks, num_tasks);
    } else if (strcmp(algoName, "PR_noPREMP") == 0) {
        Priority_No_Preemption(tasks, num_tasks);
    } else if (strcmp(algoName, "PR_withPREMP") == 0) {
        Priority_With_Preemption(tasks, num_tasks);
    } else {
        printf("Unsupported scheduling algorithm: %s\n", algoName);
    }

    return 0;
}

// Function to initialize waiting times to 0
void initialize_waiting_times(struct Task tasks[], int num_tasks) {
    for (int i = 0; i < num_tasks; i++) {
        tasks[i].waiting_time = 0;
    }
}

// Placeholder for RoundRobin implementation - need to fill in
void RoundRobin(struct Task tasks[], int num_tasks, int quantum) {
     int currentTime = 0, executedTime[num_tasks], completed = 0, queue[num_tasks*2], front = 0, rear = -1, i;
    memset(executedTime, 0, sizeof(executedTime));

    // Initialize queue with processes that arrive at time 0
    for (i = 0; i < num_tasks; i++) {
        if (tasks[i].arrival_time == 0) {
            queue[++rear] = i;
        }
    }

    while (completed < num_tasks) {
        if (front <= rear) { // Check if queue is not empty
            int index = queue[front++]; // Dequeue
            int execTime = min(quantum, tasks[index].cpu_burst - executedTime[index]);
            currentTime += execTime;
            executedTime[index] += execTime;

            // Check for new arrivals and add them to the queue
            for (i = 0; i < num_tasks; i++) {
                if (tasks[i].arrival_time > tasks[index].arrival_time && tasks[i].arrival_time <= currentTime) {
                    queue[++rear] = i;
                }
            }

            if (executedTime[index] < tasks[index].cpu_burst) {
                // If not completed, requeue the process
                queue[++rear] = index;
            } else {
                completed++;
                tasks[index].waiting_time = currentTime - tasks[index].cpu_burst - tasks[index].arrival_time;
            }
        } else {
            currentTime++; // If queue is empty, just advance time until a new process arrives
            for (i = 0; i < num_tasks; i++) {
                if (tasks[i].arrival_time <= currentTime && executedTime[i] < tasks[i].cpu_burst) {
                    queue[++rear] = i;
                    break;
                }
            }
        }
    }

    print_and_calculate_average_waiting_time(tasks, num_tasks);
}


// Placeholder for SJF implementation - need to fill in
void SJF(struct Task tasks[], int num_tasks) {
    // First, sort tasks by CPU burst, then by arrival time
    for (int i = 0; i < num_tasks - 1; i++) {
        for (int j = 0; j < num_tasks - i - 1; j++) {
            if (tasks[j].cpu_burst > tasks[j + 1].cpu_burst ||
                (tasks[j].cpu_burst == tasks[j + 1].cpu_burst && tasks[j].arrival_time > tasks[j + 1].arrival_time)) {
                struct Task temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
    }

    int currentTime = 0;
    for (int i = 0; i < num_tasks; i++) {
        if (currentTime < tasks[i].arrival_time) {
            currentTime = tasks[i].arrival_time; // Adjust current time for gaps
        }
        tasks[i].waiting_time = currentTime - tasks[i].arrival_time;
        currentTime += tasks[i].cpu_burst; // Process execution
    }

    print_and_calculate_average_waiting_time(tasks, num_tasks);
}

// Placeholder for sorting tasks by CPU burst - need to fill in for SJF
void sort_by_cpu_burst(struct Task tasks[], int num_tasks) {
    for (int i = 0; i < num_tasks - 1; i++) {
        for (int j = 0; j < num_tasks - i - 1; j++) {
            if (tasks[j].cpu_burst > tasks[j + 1].cpu_burst) {
                struct Task temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
    }
}

// Placeholder for Priority_No_Preemption implementation - need to fill in
void Priority_No_Preemption(struct Task tasks[], int num_tasks) {
    // Sort tasks by priority (and by arrival time as secondary criterion)
    for (int i = 0; i < num_tasks - 1; i++) {
        for (int j = 0; j < num_tasks - i - 1; j++) {
            if (tasks[j].priority > tasks[j + 1].priority ||
                (tasks[j].priority == tasks[j + 1].priority && tasks[j].arrival_time > tasks[j + 1].arrival_time)) {
                struct Task temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
    }

    // Initialize current time and simulate the scheduling process
    int currentTime = 0;
    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].arrival_time > currentTime) {
            currentTime = tasks[i].arrival_time;
        }
        tasks[i].waiting_time = currentTime - tasks[i].arrival_time;
        currentTime += tasks[i].cpu_burst;
    }

    // Print results
    printf("Priority Scheduling without Preemption Results:\n");
    print_and_calculate_average_waiting_time(tasks, num_tasks);
}

void Priority_With_Preemption(struct Task tasks[], int num_tasks) {
    // Initialize variables for simulation
    int currentTime = 0, i = 0, completed = 0;
    int remainingTasks = num_tasks;
    struct Task *currentTask = NULL;

    // Array to keep track of remaining burst time for each task
    int remainingBurstTime[MAX_TASKS];
    for (int i = 0; i < num_tasks; i++) {
        remainingBurstTime[i] = tasks[i].cpu_burst;
    }

    while (completed < num_tasks) {
        // Check for task arrivals and preempt if necessary
        for (int j = 0; j < num_tasks; j++) {
            if (tasks[j].arrival_time <= currentTime && remainingBurstTime[j] > 0) {
                if (currentTask == NULL || tasks[j].priority < currentTask->priority) {
                    if (currentTask != NULL) {
                        // Preempt current task
                        printf("%d    %d\n", currentTime, tasks[j].Process_Number);
                    }
                    currentTask = &tasks[j];
                }
            }
        }

        if (currentTask != NULL) {
            remainingBurstTime[currentTask - tasks]--; // Decrement remaining time for current task
            if (remainingBurstTime[currentTask - tasks] == 0) {
                // Task completed
                currentTask->waiting_time = currentTime + 1 - currentTask->cpu_burst - currentTask->arrival_time;
                currentTask = NULL;
                completed++;
            }
        }
        currentTime++; // Move simulation time forward
    }

    // Calculate and print average waiting time
    printf("Average Waiting Time: %.2f\n", print_and_calculate_average_waiting_time(tasks, num_tasks));
}

// Define or correct the implementation of print_and_calculate_average_waiting_time
float print_and_calculate_average_waiting_time(struct Task tasks[], int num_tasks) {
    int total_waiting_time = 0;
    for (int i = 0; i < num_tasks; i++) {
        // Print the waiting time for each task
        printf("Process %d waits for %d time units.\n", tasks[i].Process_Number, tasks[i].waiting_time);
        // Sum up the waiting time of all tasks
        total_waiting_time += tasks[i].waiting_time;
    }
    // Calculate the average waiting time
    float avg_waiting_time = (float)total_waiting_time / num_tasks;
    // Print the average waiting time
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    return avg_waiting_time;
}

/*// Function to calculate and print average waiting time
float calculate_average_waiting_time(struct Task tasks[], int num_tasks) {
    int total_waiting_time = 0;
    for (int i = 0; i < num_tasks; i++) {
        total_waiting_time += tasks[i].waiting_time;
    }
    float avg_waiting_time = (float)total_waiting_time / num_tasks;
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    return avg_waiting_time;
}*/