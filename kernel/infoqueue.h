#include "param.h"

struct infoNode{
	int size[5];
	int back[5];
	int max[5];
};

extern struct infoNode queue;

extern struct proc *schedq[5][NPROC];
