#pragma once
#include "process.h"
#include "queue.h"
#include "memory.h"
#include <string>

void collect_user_input();

void assign_process_list();
void new_queued_process(int current_time);
string get_announcement_prefix(int current_time);
void main_loop();
int main();
void assigning_memory_to_processes(int current_time);

void terminate_completed_process(int current_time);
void print_turnaround_times();