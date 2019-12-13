#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "manager.h"

using namespace std;

//Two constants specified in project 2 description
const int MAX_RUNTIME = 100000;
const int MAX_MEM_SIZE = 30000;

int number_of_proccesses = 0;
int pageSize = 0;
int memSize = 0;
string file_name = "";
int last_announcement = -1;
vector<PROCESS> procList;
inputQueue waitList;
frameList framelist;


int main(){
 //get user input
    while (true){
        cout << "Please enter memory size(0-30000): ";
        cin >> memSize;
        cout << "Please enter page size: ";
        cin >> pageSize;
        if (memSize > 0 && pageSize > 0 && (memSize) % (pageSize) == 0 && memSize <= MAX_RUNTIME)
        cout << "Error: " << "Memory size/page size out of bounds" << endl;
            break;
        
    }

    //read values from the input file into process list
    cout << "Please enter the file name: ";
    cin >> file_name;
    ifstream myFile;
    myFile.open(file_name);
    if(!myFile){
        perror("failed to open file");
    }
    if (myFile.is_open()) {
        //gets number of proccesses
        myFile >> number_of_proccesses;
        procList.resize(number_of_proccesses);

        for(int i = 0; i < number_of_proccesses; i++){
            //set process id's
            myFile >> procList[i].pid;
            
            //set arrival time and life time
            myFile >> procList[i].arrivalTime;
            myFile >> procList[i].lifeTime;
            
            //set memory sizes
            int memory_request_number = 0;
            int memory_request_size[10000] = {0};
            int sum = 0;
            myFile >> memory_request_number;
            for (int j = 0; j <memory_request_number; j ++){
                myFile >> memory_request_size[j];
                sum += memory_request_size[j];
            }
            procList[i].memoryRequest = sum;
            //init other data in process list for later use
            procList[i].notAvailable = true;
            procList[i].timePutInMem = -1;
            procList[i].time_finished = -1;
        }
    }
    myFile.close();
    
    //create empty queue with a capacity equal to the number of process
    waitList = create_process_queue(number_of_proccesses);

    //create a shared framelist
    framelist = createFrameList(memSize / pageSize, pageSize);
    
    long currentTime = 0;
    int running = 1;
    
    while (running){
        //queues any process that have arrived
        PROCESS process;
    for (int i = 0; i < number_of_proccesses; i += 1){
        process = procList[i];
        if (process.arrivalTime == currentTime){
            string print_time = get_announcement_prefix(currentTime);
            cout << print_time << "Process " << process.pid << " arrives" << endl;
            waitList = queued_process(waitList, process);
            print_process_queue(waitList);
        }
    }
        //removes completed processes
        int i, time_spent_in_memory;
	// dequeue any procs that need to be dequeue'd
	for (i = 0; i < number_of_proccesses; i++) {
		time_spent_in_memory = currentTime - procList[i].timePutInMem;
		if (procList[i].notAvailable && (time_spent_in_memory >= procList[i].lifeTime)) {
			cout << get_announcement_prefix(currentTime)  << "Process " << procList[i].pid << " completes" << endl;
			procList[i].notAvailable = true;
			procList[i].time_finished = currentTime;
			framelist = freeMemoryForPID(framelist, procList[i].pid);
			printFrameList(framelist);
		}
	}
        //assign available memory to process that needs it
        int index = 0;
	int limit = 0;
	PROCESS newProcess;

	limit = waitList.size;

	// enqueue any procs that can be put into mem
	for (int i = 0; i < limit; i++) {
		index = iterate_queue_index(waitList, i);
		newProcess = waitList.elements[index];

		if (procCanFitIntoMem(framelist, newProcess)) {
			cout << get_announcement_prefix(currentTime) << "MM moves Process "
				<< newProcess.pid << " to memory" << endl;

			framelist = fitProcIntoMem(framelist, newProcess);
			for (int j = 0; j < number_of_proccesses; j++) {
				if (procList[j].pid == newProcess.pid) {
					procList[j].notAvailable = false;
					procList[j].timePutInMem = currentTime;
					waitList = dequeue_proc_at_index(waitList, i);
				}
			}

			print_process_queue(waitList);

			printFrameList(framelist);
		}
	}
        currentTime ++; 
        if(currentTime > MAX_RUNTIME){
        //Program is supposed to detect deadlock
            printf("DEADLOCKED: max time reached\n");
            break;
        }
        if (waitList.size == 0 && frameListIsEmpty(framelist)){
            break;
        }
    }
    print_turnaround_times();
    
    return 0;
}

// adds new processes to the input queue

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




// prints the average turnaround time
void print_turnaround_times() {
	int i;
	float total = 0;
	for (i = 0; i < number_of_proccesses; i += 1) {
		total += procList[i].time_finished - procList[i].arrivalTime;
	}
	cout << "Average Turn-around Time " << total / number_of_proccesses << endl;
}


