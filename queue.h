#pragma once
#include <vector>

using namespace std;

struct Process_Queue {
	int capacity;
	int size;
	int front;
	int rear;
	vector<PROCESS> elements;
};

Process_Queue create_process_queue(int length) {
	Process_Queue q;
	q.elements.resize(length);
	q.size = 0;
	q.capacity = length;
	q.front = 0;
	q.rear = -1;

	return q;
}

Process_Queue queued_process(Process_Queue q, PROCESS proc) {
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

PROCESS peek_queue_at_index(Process_Queue q, int index) {
	return q.elements[index];
}

int iterate_queue_index(Process_Queue q, int index) {
	return q.front + index;
}

void print_process_queue(Process_Queue q) {
	PROCESS proc;

	cout << "\tInput queue: [ ";
	for (int i = 0; i < q.size; i ++) {
		proc = peek_queue_at_index(q, iterate_queue_index(q, i));
		cout << proc.pid << " ";
	}
	cout << "]" << endl;
}



int queue_has_next(Process_Queue q) {
	return q.size == 0 ? 0 : 1;
}


void dequeue_proc(Process_Queue q) {
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



Process_Queue dequeue_proc_at_index(Process_Queue q, int index) {
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

