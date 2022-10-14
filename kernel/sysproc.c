#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_waitx(void)
{
  uint64 addr, addr1, addr2;
  uint wtime, rtime;
  argaddr(0, &addr);
  argaddr(1, &addr1); // user virtual memory
  argaddr(2, &addr2);
  int ret = waitx(addr, &wtime, &rtime);
  struct proc* p = myproc();
  if (copyout(p->pagetable, addr1,(char*)&wtime, sizeof(int)) < 0)
    return -1;
  if (copyout(p->pagetable, addr2,(char*)&rtime, sizeof(int)) < 0)
    return -1;
  return ret;
}

uint64
sys_strace(void)
{
  int n;
  if(argint(0, &n) < 0){
    return -1;
  }
  struct proc *process = myproc();
  process->mask = n;
  return 0;
}

extern struct proc proc[NPROC];

uint64
sys_set_priority()
{
  int priority, pid, prev_priority = 101;
  if(argint(0, &priority) < 0)
    return -1;
  if(argint(1, &pid) < 0)
    return -1;
  for(struct proc *p = proc; p < &proc[NPROC]; p++)
  {
    acquire(&p->lock);
    if(p->pid == pid && priority >= 0 && priority <= 100)
    {
      p->rtime = 0;
      p->sltime = 0;
      prev_priority = p->prio;
      p->prio = priority;
    }
    release(&p->lock);
  }
  if(prev_priority > priority){
    yield();
  }
  return prev_priority;
}

uint64
sys_settickets(void)
{ 
  // int proc_id = myproc()->pid;
  // struct proc *p = 0;
  // acquire(&p->lock);
  // for(p = myproc(); p < &myproc()[NPROC]; p++){
  //   if(p->pid == proc_id){
  //     p->tickets = tickets;

  //   }
  // }
  int tickets;
  if(argint(0, &tickets)<0)
    return -1;
  
  myproc()->tickets = tickets;
  return 0;
}

uint64 sys_sigalarm(void){
  int ticks;
  if(argint(0, &ticks) < 0)
    return -1;
  uint64 handler;
  if(argaddr(1, &handler) < 0)
    return -1;
  myproc()->alarm_on = 0;
  myproc()->ticks = ticks;
  myproc()->cur_ticks = 0;
  myproc()->alarm_handler = handler;
  return 0; 
}

void restore()
{
  struct proc*p=myproc();

  p->alarm_trapframe->kernel_satp = p->trapframe->kernel_satp;
  p->alarm_trapframe->kernel_sp = p->trapframe->kernel_sp;
  p->alarm_trapframe->kernel_trap = p->trapframe->kernel_trap;
  p->alarm_trapframe->kernel_hartid = p->trapframe->kernel_hartid;
  *(p->trapframe) = *(p->alarm_trapframe);
}

uint64 sys_sigreturn(void){
  restore();
  myproc()->alarm_on = 0;
  //myproc()->trapframe->a0 = 0xac;
  return myproc()->trapframe->a0;
}