#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "manager.h"

using namespace std;

const int MAX_RUNTIME = 100000;
const int MAX_MEMORY_SIZE = 30000;

int number_of_proccesses = 0;
int page_size = 0;
int memory_size = 0;
string file_name = "";
int last_announcement = -1;
vector<PROCESS> process_list;
Process_Queue waitList;
frameList framelist;

int main(){
    //collects the user input
    collect_user_input();

    //reads values from the input file into a shared process list
    putProcessInList();

    //create a shared queue with a capacity equal to the number of process
    waitList = create_process_queue(number_of_proccesses);

    //create a shared framelist
    framelist = createFrameList(memory_size / page_size, page_size);

    main_loop();

    return 0;
}

void main_loop(){
    long currentTime = 0;
    while (1){
        //queues any process that have arrived
        new_queued_process(currentTime);
        //removes completed processes
        terminate_completed_process(currentTime);
        //assign available memory to process that needs it
        assigningMemToProcesses(currentTime);
        currentTime ++; 
        if(currentTime > MAX_RUNTIME){
            printf("DEADLOCK: max time reached\n");
            break;
        }
        if (waitList.size == 0 && frameListIsEmpty(framelist)){
            break;
        }
    }
    print_turnaround_times();
}

void collect_user_input(){
    while (true){
        cout << "Please enter memory size(0-30000): ";
        cin >> memory_size;
        cout << "Please enter page size: ";
        cin >> page_size;
        if (memory_size > 0 && page_size > 0&& (memory_size)% (page_size) == 0 && memory_size <= MAX_RUNTIME)
            break;
        cout << "ERROR"<< "Invalid input" << "Please Enter again" << endl;
    }
}

void putProcessInList() {
    cout << "Please enter the file name: ";
    cin >> file_name;
    ifstream myFile;
    myFile.open(file_name);
    if(!myFile){
        perror("failed to open file");
        putProcessInList();

    }
    if (myFile.is_open()) {
        //gets number of proccesses
        myFile >> number_of_proccesses;
        process_list.resize(number_of_proccesses);

        for(int i = 0; i < number_of_proccesses; i++){
            //set id
            myFile >> process_list[i].pid;
            //set arrival time and life time
            myFile >> process_list[i].arrival_time >> process_list[i].life_time;
            //set memory size
            int memory_request_number = 0;
            int memory_request_size[10000] = {0};
            int sum = 0;
            myFile >> memory_request_number;
            for (int j = 0; j <memory_request_number; j ++){
                myFile >> memory_request_size[j];
                sum += memory_request_size[j];
            }
            process_list[i].memoryRequest = sum;
            //init other data in process list
            process_list[i].is_active = 0;
            process_list[i].time_added_to_memory = -1;
            process_list[i].time_finished = -1;
        }
    }
    myFile.close();
}

// adds new processes to the input queue
void new_queued_process(int currentTime){
    PROCESS process;
    for (int i = 0; i < number_of_proccesses; i += 1){
        process = process_list[i];
        if (process.arrival_time == currentTime){
            string print_time = get_announcement_prefix(currentTime);
            cout << print_time << "Process " << process.pid << " arrives" << endl;
            waitList = queued_process(waitList, process);
            print_process_queue(waitList);
        }
    }
}
string get_announcement_prefix(int currentTime){
    string result = "";
    if (last_announcement == currentTime){
        result = "\t";
    }
    else{
        result = "t = " +to_string(currentTime) + "\t";
    }
    last_announcement = currentTime;
    return result;
}


void terminate_completed_process(int currentTime) {
	int i, time_spent_in_memory;
	// dequeue any procs that need it
	for (i = 0; i < number_of_proccesses; i++) {
		time_spent_in_memory = currentTime - process_list[i].time_added_to_memory;
		if (process_list[i].is_active && (time_spent_in_memory >= process_list[i].life_time)) {
			cout << get_announcement_prefix(currentTime)  << "Process " << process_list[i].pid << " completes" << endl;
			process_list[i].is_active = 0;
			process_list[i].time_finished = currentTime;
			framelist = freeMemoryForPID(framelist, process_list[i].pid);
			printFrameList(framelist);
		}
	}
}

// prints the average turnaround time
void print_turnaround_times() {
	int i;
	float total = 0;
	for (i = 0; i < number_of_proccesses; i += 1) {
		total += process_list[i].time_finished - process_list[i].arrival_time;
	}
	cout << "Average Turnaround Time " << total / number_of_proccesses << endl;
}

// assigns any available memory to waiting procs that'll fit in there
void assigningMemToProcesses(int currentTime) {
	int index, limit;
	PROCESS process;

	limit = waitList.size;

	// enqueue any procs that can be put into mem
	for (int i = 0; i < limit; i++) {
		index = iterate_queue_index(waitList, i);
		process = waitList.elements[index];

		if (procCanFitIntoMem(framelist, process)) {
			cout << get_announcement_prefix(currentTime) << "MM moves Process "
				<< process.pid << " to memory" << endl;

			framelist = fitProcIntoMem(framelist, process);
			for (int j = 0; j < number_of_proccesses; j++) {
				if (process_list[j].pid == process.pid) {
					process_list[j].is_active = 1;
					process_list[j].time_added_to_memory = currentTime;
					waitList = dequeue_proc_at_index(waitList, i);
				}
			}

			print_process_queue(waitList);

			printFrameList(framelist);
		}
	}
}
