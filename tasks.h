#ifndef TASKS_H
#define TASKS_H

typedef struct {
    char title[100];
    char description[256];
    char deadline[11];
    int difficulty;    
    int completed;   
} Task;

void load_tasks();
void save_tasks();
void add_task(Task task);
void delete_task(int index);
void update_task(int index, Task task);
void mark_task_completed(int index);
void sort_tasks_by_title();
void sort_tasks_by_deadline();
void sort_tasks_by_difficulty();

extern Task task_list[100];
extern int task_count;

#endif
