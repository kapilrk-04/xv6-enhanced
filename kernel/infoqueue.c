#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"
#include "infoqueue.h"

struct infoNode queue;

struct proc *schedq[5][NPROC];

void
initQueue(){
	for(int q = 0 ; q < 5 ; q++){
		queue.size[q] = 0;
		queue.back[q] = 0;
		for(int i = 0; i < NPROC ; i++){
			schedq[q][i] = 0;
		}
	}
	queue.max[0] = 1;
	queue.max[1] = 2;
	queue.max[2] = 4;
	queue.max[3] = 8;
	queue.max[4] = 16;
}

void
Enqueue(struct proc *p, int queno){
	schedq[queno][queue.back[queno]] = p;
	p->eqtime = ticks;
	p->queno = queno;
	p->cwtime = 0;
	p->inqueue = 1;
	p->ticksin = 0;
	queue.back[queno]++;
	queue.size[queno]++;
}

struct proc*
Dequeue(int queno){
	struct proc *temp = schedq[queno][0];
	
  schedq[queno][0] = 0;
	for(int i = 1 ; i < NPROC ; i++){
		schedq[queno][i-1] = schedq[queno][i];
		if(schedq[queno][i]==0){
      break;
    }
	}
	queue.back[queno]--;
	queue.size[queno]--;
  temp->inqueue = 0;

	return temp;
}

void
Pop(struct proc *p, int queno){
	int found = -1;
	for(int i = 0 ; i < NPROC ; i++){
		if(schedq[queno][i] == p){
      found = i;
    }
  }
	
  if(found == -1){
    return;
  }

	schedq[queno][found] = 0;
	for(int i = found + 1 ; i < NPROC ; i++){
		schedq[queno][i-1] = schedq[queno][i];
		if(schedq[queno][i]==0){
      break;
    }
	}
}