#pragma once
#include<vector>
#include <string>

using namespace std;

//Function prototypes

void collect_user_input();

void putProcessInList();
void new_queued_process(int currentTime);
string get_announcement_prefix(int currentTime);// <-- ?
void main_loop();
int main();
void assigningMemToProcesses(int currentTime);

void terminate_completed_process(int currentTime);
void print_turnaround_times();

///////////////////////////////////////////////////

//Structs and Classes

struct FRAME {
	int assigned;
	char location[40];
	int assignProc;
	int pageNum;
};

struct frameList {
	vector<FRAME> frames;
	int numOfFrames;
	int pageSize;
};

class PROCESS{
public:
	int pid;
	int arrival_time;
	int life_time;
	int memoryRequest;

	int time_added_to_memory;
	int is_active;
	int time_finished;
};

struct inputQueue {
	int capacity;
	int size;
	int front;
	int rear;
	vector<PROCESS> elements;
};



/////////////////////////////////////////////////

//Functions:

frameList createFrameList(int numOfFrames, int pageSize) {
	frameList f;

	f.frames.resize(numOfFrames);

	f.pageSize = pageSize;
	f.numOfFrames = numOfFrames;

	for (int i = 0; i < f.numOfFrames; i ++) {
		f.frames[i].assigned = 0;
		f.frames[i].pageNum = 0;
		f.frames[i].assignProc = 0;
	}

	return f;
}

void printFrameList(frameList list) {
	bool inFreeBlock = false;
	int start = 0;

	cout << "\tMemory map:" << endl;

	for (int i = 0; i < list.numOfFrames; i++) {
		if (!inFreeBlock && !list.frames[i].assigned) {
			inFreeBlock = true;
			start = i;
		}
		else if (inFreeBlock && list.frames[i].assigned) {
			inFreeBlock = false;
			cout << "\t\t" << start * list.pageSize
				<< "-" << (i * list.pageSize) - 1
				<< ": Free frame(s)\n";
		}
		if (list.frames[i].assigned) {

			cout << "\t\t" << i * list.pageSize
				<< "-" << ((i + 1) * list.pageSize) - 1
				<< ": Process"
				<< list.frames[i].assignProc << ", Page "
				<< list.frames[i].pageNum << endl;
		}

	}
	if (inFreeBlock) {
		cout << "\t\t" << start * list.pageSize
			<< "-" << ((list.numOfFrames)* list.pageSize) - 1
			<< ": Free frame(s)" << endl; 
	}
}


frameList freeMemoryForPID(frameList list, int pid) {
	for (int i = 0; i < list.numOfFrames; i++) {
		if (list.frames[i].assignProc == pid) {
			list.frames[i].assignProc = 0;
			list.frames[i].pageNum = 0;
			list.frames[i].assigned = 0;
		}
	}
	return list;
}

int frameListIsEmpty(frameList list) {
	for (int i = 0; i < list.numOfFrames; i++) {
		if (list.frames[i].assigned) {
			return 0;
		}
	}
	return 1;
}

int frameList_is_empty(frameList list) {
	for (int i = 0; i < list.numOfFrames; i++) {
		if (list.frames[i].assigned) {
			return 0;
		}
	}
	return 1;
}

int procCanFitIntoMem(frameList list, PROCESS process) {
	int numOfFreeFrames = 0;

	for (int i = 0; i < list.numOfFrames; i++) {
		if (!list.frames[i].assigned) {
			numOfFreeFrames++;
		}
	}
	return (numOfFreeFrames * list.pageSize) >= process.memoryRequest;
}

frameList fitProcIntoMem(frameList list, PROCESS process) {
	// this assumes you've already checked that you CAN fit the proc into mem
	int remaining_mem, current_page = 1;

	remaining_mem = process.memoryRequest;
	
	for (int i = 0; i < list.numOfFrames; i++) {
		// if this frame is not assigned
		if (!list.frames[i].assigned) {
			// assign it
			list.frames[i].assigned = 1;
			// set the page number
			list.frames[i].pageNum = current_page;
			// set the proc num
			list.frames[i].assignProc = process.pid;

			current_page++;
			//cout << list.frames[i].assigned << endl;
			remaining_mem -= list.pageSize;
		}

		if (remaining_mem <= 0) {
			break;
		}
	}

	return list;
}


inputQueue create_process_queue(int length) {
	inputQueue q;
	q.elements.resize(length);
	q.size = 0;
	q.capacity = length;
	q.front = 0;
	q.rear = -1;

	return q;
}

inputQueue queued_process(inputQueue q, PROCESS proc) {
	if (q.size == q.capacity) {
		cout << "ERROR: queue is full to capacity!" << endl;
		exit(2);
	}

	q.size++;
	q.rear++;

	if (q.rear == q.capacity) {
		q.rear = 0;
	}
	q.elements[q.rear] = proc;
	return q;
}

PROCESS peek_queue_at_index(inputQueue q, int index) {
	return q.elements[index];
}

int iterate_queue_index(inputQueue q, int index) {
	return q.front + index;
}

void print_process_queue(inputQueue q) {
	PROCESS proc;

	cout << "\tInput queue: [ ";
	for (int i = 0; i < q.size; i ++) {
		proc = peek_queue_at_index(q, iterate_queue_index(q, i));
		cout << proc.pid << " ";
	}
	cout << "]" << endl;
}



int queue_has_next(inputQueue q) {
	return q.size == 0 ? 0 : 1;
}


void dequeue_proc(inputQueue q) {
	if (!queue_has_next(q)) {
		cout << "ERROR: queue is empty, can't dequeue anything." << endl;
		exit(2);
	}

	q.size--;
	q.front++;

	if (q.front == q.capacity) {
		q.front = 0;
	}
}



inputQueue dequeue_proc_at_index(inputQueue q, int index) {
	int prev = 0;
	for (int i = 0; i < q.size; i += 1) {
		if (i > index) {
			q.elements[prev] = q.elements[i];
		}

		prev = i;
	}

	q.size--;
	q.rear--;

	return q;
}
