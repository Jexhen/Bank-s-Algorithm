#ifndef _TYPE_H_
#define _TYPE_H_
#define RESOURCENUM 3
#define WAIT 1
#define RUN 2
#define BLOCK 3
#define FINISH 4

struct PCB_ {
	char name[10];
	int max[RESOURCENUM];
	int need[RESOURCENUM];
	int allocation[RESOURCENUM];
	bool finish;
	int status;
	struct PCB_ *next;
};
typedef struct PCB_ PCB;

struct Queue_ {
	PCB *queue_head;
	PCB *queue_tail;
	int length;
};
typedef struct Queue_ Queue;



#endif // !_TYPE_H_

