#pragma once
class PROCESS{
public:
	int pid;
	int arrival_time;
	int life_time;
	int memory_request;

	int time_added_to_memory;
	int is_active;
	int time_finished;
};