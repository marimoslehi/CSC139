#include <stdio.h>
#include <string.h>

#define MAX_TASKS 100 // Maximum number of tasks

// Struct to represent a task
struct Task {
    int Process_Number;
    int arrival_time;
    int cpu_burst;
    int priority;
};

// Function prototypes
void RoundRobin(struct Task tasks[], int num_tasks);
void SJF(struct Task tasks[], int num_tasks);
void Priorty_No_Preemption(struct Task tasks[], int num_tasks);
void Priorty_With_Preemption(struct Task tasks[], int num_tasks);
void sort_by_cpu_burst(struct Task tasks[], int num_tasks);
float calculate_average_waiting_time(struct Task tasks[], int num_tasks);

int main() {
    FILE *file;
    char filename[] = "input.txt";
    struct Task tasks[MAX_TASKS]; // Array to store tasks
    int num_tasks, task_id, arrival_time, cpu_burst, priority,time_quantom;

    // Open the file for reading
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return 1;
    }

    // Using fscanf to read from the file and %s stops when it encounters a white space or tab or end of line character and it will be 25 chars long 
    // to ensure that it fits the algo name
    char algoName[25];
    fscanf(file, "%s", algoName);



    // Using fscanf to read a number from the file and stops when it encounters a new line,space and tab. So that will read a number that will 
    // correspond to the number of tasks and store it in num_task. &num_tasks is the location in memory that fscanf will use to save that value into
    fscanf(file, "%d", &num_tasks);

    // For loop to store all the values(ints) in one line to the corresping variable then store those inside a struct. fscanf will read 4 ints and store
    // them in the correct variable by passing in the variable location in memory.
    for (int i = 0; i < num_tasks; i++) {
        fscanf(file, "%d %d %d %d", &task_id, &arrival_time, &cpu_burst, &priority);
        tasks[i].Process_Number = task_id;
        tasks[i].arrival_time = arrival_time;
        tasks[i].cpu_burst = cpu_burst;
        tasks[i].priority = priority;
    }

    // Close file since were dont accessing it
    fclose(file);

    // Prints the Algo name to console 
    printf("Algorithm Name: %s\n", algoName);
    printf("Algorithm Name: %s\n", time_quantom);

    // Check the scheduling algorithm name and call the appropriate methods
    if (strcmp(algoName, "RR") == 0) {
        // Call RR method
        RoundRobin(tasks, num_tasks);
    } else if (strcmp(algoName, "SJF") == 0) {
        // Call SJF method
        SJF(tasks, num_tasks);
    } else if (strcmp(algoName, "PR_noPREMP") == 0) {
        // Call PR_noPREMP method
        Priorty_No_Preemption(tasks, num_tasks);
    } else if (strcmp(algoName, "PR_withPREMP") == 0) {
        // Call PR_withPREMP method
        Priorty_With_Preemption(tasks, num_tasks);
    } else {
        printf("Unsupported scheduling algorithm: %s\n", algoName);
    }

    printf("Process Number  Arrival Time  CPU Burst  Priority\n");
    for (int i = 0; i < num_tasks; i++) {
        printf("%d\t\t%d\t\t%d\t\t%d\n", tasks[i].Process_Number, tasks[i].arrival_time, tasks[i].cpu_burst, tasks[i].priority);
        }


    return 0;
}

// Define method implementations
void RoundRobin(struct Task tasks[], int num_tasks) {
    // Method implementation for Round Robin
    printf("Round Robin method called\n");
}

void SJF(struct Task tasks[], int num_tasks) {
    // Method implementation for Shortest Job First
    sort_by_cpu_burst(tasks, num_tasks);
    printf("Shortest Job First method called\n");
    float avg_waiting_time = calculate_average_waiting_time(tasks, num_tasks);
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
}

void Priorty_No_Preemption(struct Task tasks[], int num_tasks) {
    // Method implementation for Priority Scheduling without Preemption
    printf("Priority Scheduling without Preemption method called\n");
}

void Priorty_With_Preemption(struct Task tasks[], int num_tasks) {
    // Method implementation for Priority Scheduling with Preemption
    printf("Priority Scheduling with Preemption method called\n");
}

// Sorting tasks based on CPU burst (SJF)
void sort_by_cpu_burst(struct Task tasks[], int num_tasks) {
    struct Task temp;
    for (int i = 0; i < num_tasks - 1; i++) {
        for (int j = i + 1; j < num_tasks; j++) {
            if (tasks[i].cpu_burst > tasks[j].cpu_burst) {
                temp = tasks[i];
                tasks[i] = tasks[j];
                tasks[j] = temp;
            }
        }
    }
}

// Calculate average waiting time for SJF
float calculate_average_waiting_time(struct Task tasks[], int num_tasks) {
    int total_waiting_time = 0;
    int current_time = 0;
    
    // Calculate waiting time for each task
    for (int i = 0; i < num_tasks; i++) {
        int waiting_time = current_time - tasks[i].arrival_time;
        total_waiting_time += waiting_time;
        current_time += tasks[i].cpu_burst;
    }
    
    // Return average waiting time
    return (float)total_waiting_time / num_tasks;
}