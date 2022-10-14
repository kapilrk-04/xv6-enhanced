// #include "types.h"
// #include "param.h"
// #include "memlayout.h"
// #include "riscv.h"
// #include "spinlock.h"
// #include "proc.h"
// #include "defs.h"

// int
// do_rand(unsigned long *ctx)
// {
// /*
//  * Compute x = (7^5 * x) mod (2^31 - 1)
//  * without overflowing 31 bits:
//  *      (2^31 - 1) = 127773 * (7^5) + 2836
//  * From "Random number generators: good ones are hard to find",
//  * Park and Miller, Communications of the ACM, vol. 31, no. 10,
//  * October 1988, p. 1195.
//  */
//     long hi, lo, x;

//     /* Transform to [1, 0x7ffffffe] range. */
//     x = (*ctx % 0x7ffffffe) + 1;
//     hi = x / 127773;
//     lo = x % 127773;
//     x = 16807 * lo - 2836 * hi;
//     if (x < 0)
//         x += 0x7fffffff;
//     /* Transform to [0, 0x7ffffffd] range. */
//     x--;
//     *ctx = x;
//     return (x);
// }

// unsigned long rand_next = 1;

// int
// rand(void)
// {
//     return (do_rand(&rand_next));
// }

// void
// scheduleRR(struct cpu *c, struct proc *proc){
//   for(struct proc *p = proc; p < &proc[NPROC]; p++) {
//     acquire(&p->lock);
//     if(p->state == RUNNABLE) {
//       // Switch to chosen process.  It is the process's job
//       // to release its lock and then reacquire it
//       // before jumping back to us.
//       p->state = RUNNING;
//       c->proc = p;
//       swtch(&c->context, &p->context);

//       // Process is done running for now.
//       // It should have changed its p->state before coming back.
//       c->proc = 0;
//     }
//     release(&p->lock);
//   }
// }

// void
// scheduleFCFS(struct cpu *c, struct proc *proc){
//   struct proc *proc_least;
//   proc_least = 0;
//   uint64 time_least = 0;

//   for(struct proc *p = proc ; p < &proc[NPROC] ; p++){
//     acquire(&p->lock);
//     if(p->state == RUNNABLE){
//       if(p->ctime < time_least || !proc_least){
//         if(proc_least){
//           release(&proc_least->lock);
//         }
//         proc_least = p;
//       }else{
//         release(&p->lock);
//       }
//     }else{
//       release(&p->lock);
//     }
//   }

//   if(proc_least){
//     proc_least->state = RUNNING;
//     c->proc = proc_least;
//     swtch(&c->context, &proc_least->context);
    
//     c->proc = 0;
//     release(&proc_least->lock);
//   }
// }

// void
// schedulePBS(struct cpu *c, struct proc *proc){
//   struct proc *p_to_run = 0, *p;
//   int prio_temp = 101, prio_temp_p = 0;
//   int temp;
  
//   for(p = proc; p < &proc[NPROC]; p++){
//     acquire(&p->lock);
//     temp = p->prio - p->nice + 5;

//     if(temp > 100){
//       temp = 100;
//     }

//     if(temp < 0){
//       prio_temp_p = 0;
//     }else{
//       prio_temp_p = temp;
//     }

//     if(p->state == RUNNABLE){
//       if(p_to_run == 0 || prio_temp > prio_temp_p || (prio_temp == prio_temp_p && (p_to_run->round_count > p->round_count || (p_to_run->round_count == p->round_count && p_to_run->ctime < p->ctime)))){
//         if(p_to_run){
//           release(&p_to_run->lock);
//         }
//         p_to_run = p;
//         prio_temp = prio_temp_p;
//         continue;
//       }
//     }
//     release(&p->lock);
//   }
  
//   if (p_to_run){
//     p_to_run->state = RUNNING;
//     p_to_run->round_count++;
//     p_to_run->stime = ticks;
//     p_to_run->rtime = 0;
//     p_to_run->sltime = 0;
//     c->proc = p_to_run;
//     swtch(&c->context, &p_to_run->context);
//     p->nice = ((p->sltime) * 10 ) / (p->rtime + p->sltime);
//     c->proc = 0;
//     release(&p_to_run->lock);
//   }
// }

// int noTickets(struct proc *process)
// {
//   struct proc *p;
//   int tot = 0;
//   for(p = process; p < &process[NPROC]; p++){
//     if(p->state == RUNNABLE){
//       tot += p->tickets;
//     }
//   }
//   return tot;
// }

// void scheduleLBS(struct cpu *c, struct proc *proc)
// {
//   printf("LBS entry\n");
//   struct proc *p;
//   c->proc = 0;

//   for(;;)
//   {
//     intr_on();
//     for(p = proc; p < &proc[NPROC]; p++)
//     {
//       acquire(&p->lock);
//       if(p->state != RUNNABLE){
//         printf("cannot\n");
//         release(&p->lock);
//         continue;
//       }
//       int totTickets = noTickets(proc);
//       printf("%d is total no.of tickets\n", totTickets);
//       int draw = -1;
//       if(totTickets > 0){
//         draw = rand() % totTickets;
//       }
//       printf("%d\n",draw);
//       draw -= p->tickets;
//       if(draw >= 0)
//       { 
//         printf("releasing\n");
//         release(&p->lock);
//         continue;
//       }

//     }
//     release(&p->lock);
//     if(p->state == RUNNABLE)
//     {
//       printf("runnable\n");
//       p->state = RUNNING;
//       c->proc = p;
//       swtch(&c->context, &p->context);
//       c->proc = 0;
//     }
//     //release(&p->lock);
//   }
  
// }