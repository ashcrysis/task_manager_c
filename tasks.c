#include "tasks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Task task_list[100];
int task_count = 0;

void load_tasks() {
    FILE *f = fopen("tasks.dat", "rb");
    if (f) {
        fread(&task_count, sizeof(int), 1, f);
        fread(task_list, sizeof(Task), task_count, f);
        fclose(f);
    }
}

void save_tasks() {
    FILE *f = fopen("tasks.dat", "wb");
    fwrite(&task_count, sizeof(int), 1, f);
    fwrite(task_list, sizeof(Task), task_count, f);
    fclose(f);
}

void add_task(Task task) {
    if (task_count < 100) {
        task_list[task_count++] = task;
        save_tasks();
    }
}

void delete_task(int index) {
    for (int i = index; i < task_count - 1; i++)
        task_list[i] = task_list[i + 1];
    task_count--;
    save_tasks();
}

void update_task(int index, Task task) {
    task_list[index] = task;
    save_tasks();
}

void mark_task_completed(int index) {
    task_list[index].completed = 1;
    save_tasks();
}

int cmp_title(const void *a, const void *b) {
    return strcmp(((Task *)a)->title, ((Task *)b)->title);
}

int cmp_deadline(const void *a, const void *b) {
    return strcmp(((Task *)a)->deadline, ((Task *)b)->deadline);
}

int cmp_difficulty(const void *a, const void *b) {
    return ((Task *)a)->difficulty - ((Task *)b)->difficulty;
}

void sort_tasks_by_title() {
    qsort(task_list, task_count, sizeof(Task), cmp_title);
}

void sort_tasks_by_deadline() {
    qsort(task_list, task_count, sizeof(Task), cmp_deadline);
}

void sort_tasks_by_difficulty() {
    qsort(task_list, task_count, sizeof(Task), cmp_difficulty);
}
