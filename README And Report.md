# Assignment 4 : Enhancing XV-6<br>Operating Systems and Networks, Monsoon 2022

## Kapil 2021101028
## Harshavardhan 2021111003
<br>

## Specification 1
## System Call 1: strace
- Used steps in 'Slides.pdf' to do necessary changes in order to implement the syscall.
- We added `mask` in proc data-structure to store the mask value of the syscall. This is used for storage of mask in strace alone, and does not affect any other aspects of the syscalls.
- We also added strace syscall function which can be used to set the mask, with a given argument.
- Since syscalls all go through `syscall` function, we have modified the function to check the bits of the mask `1 << n` .
- We save argument count and names in an array to use in printing out strace output.
- To prevent the syscall from modifying required data, i.e., a0, we already store data in trapframe.

## System Call 2: sigalarm/sigreturn
- The following data values were added to proc data-structure to implement sigalarm/sigreturn: `alarm_handler` - stores the address of handler function called by sigalarm when triggered, `ticks` - stores no. of ticks before sigalarm is triggered (syscall value), `cur_ticks` - stores no. of ticks which have passed, `alarm_trapframe` - stores a copy of the variables from the original trapframe to prevent overwrites, `alarm_on` - shows status of the alarm.
- We initialise these data values in sys_sigalarm.
- `cur_ticks` is incremented every loop until it is equal to `ticks`. When this occurs, the function specified in sigalarm parameter is called, and `cur_ticks` is set back to zero. 
- We store the copy of the process trapframe in `alarm_trapframe`, and update the trapframe's program counter with the handler function.
- in sys_sigreturn(), a `restore()` function is implemented which basically restores the updated states to the original trapframe. To ensure that the value of a0 register is not altered, sys_sigreturn returns the value of the a0 register.
<br>

## Specification 2
- Initial modifications to the existing files were done to make further changes simpler and procedural.
- Add `SCHEDULER_TYPE` to Makefile to enable changing of schedulers in each run.
- We moved the existing Round Robin (RR) scheduling into a function `scheduleRR` and call it from within the `scheduler` function, if the `SCHEDULE_TYPE` in Makefile is `RR`.

## FCFS (First Come First Serve)
- We add a new attribute `ctime` meaning 'create-time' in proc data-structure.
- In creation of a process, we set the `ctime` of all processes as the then ticks count.
- In the scheduler algorithm, we first acquire the lock. Then go through all the processes and find the process with least `ctime`. If current process is the least, then we do not release the lock. Else we release the lock and context swtch to the optimal process and let it run.
- We also prevent `yield` function to prevent timer interrupts, to prevent preemption.
- This scheduler is the simplest implementation as we only have to set the next process in queue for execution when current process finishes and do not worry about anything else.

## LBS (Lottery Based Scheduler)
- We add a new attribute `tickets` to the proc data structure, this represents the number of tickets which the processor. 
- We also make use of the `rand()`function present in `grind.c` as the random number generator.
- We use `intr_on()` to enable interrupts, to ensure that there are no deadlocks.
- A system call `settickets` is also implemented.
- Before the draw, we iterate over the page table to calculate the total number of tickets from all the runnable processes.
- The `rand()` function now selects a random number from the range 0 to totTickets-1. We then once again iterate over the page table, but this time we add the number of tickets of the process to a variable "passed_tkts" which stores the number of tickets held by all the iterated processes at a given point. 
- when passed_tkts exceeds the value of the picked ticket, it implies that the current process has the ticket. Thus the current process is run. 

## PBS (Priority Based Scheduler)
- We added new attributes `prio`, `sltime`, `rtime`, `round_count`, `nice`. We set prio to 60 and nice to 5 as default in `allocproc` and other attributes to 0.
- we added a function `update_time` to update the sltime and rtime of the processes in the system in each timer interrupt.
- We also added a new function `set_priority` for user and program to set priority of a process. This function was added similar to Specification 1 functions by following the same steps.
- We then simply followed the steps mentioned in the Assignment document to find Niceness and Dynamic Property.
- The scheduler implemented by the rules given always keeps the process with best priority in the running state. Processes which exceed time (ticks) will loose priority and preemption takes place after ticks, due to `update_time`.

## MLFQ (Multi Level Feedback Queue)
- To implement MLFQ, we added a new data-structure by the name of `infoNode` to contain queues' data of all the processes.
- We then defined the operations required for the queue manipulation, namely `Enqueue`, `Dequeue` and `Pop`. This queue is checked to find out the specifics of each queue in the scheduler.
- To implement the queues, we also need `schedq` to hold all the values, and is used in `infoqueue.c`.
- The scheduler first gives time to priority 0 queue. When done with this queue, next execution is of priority 1 queue, and so on till queue 4.
- New processes are added to the priority 0 queue always.
- After a process uses it's given ticks in current queue, it drops to lower queue, until it bottoms at queue 4.
- Processes in queue 4 are incremented in queue after aging 100 ticks, set as default by us.
- At end of each tick, we update all the 'times' of each process.
- Queue 4 processes remain their until they age and move up in priority or exit by completing. If not, staying in queue 4 means it by itself is in round robin, so that was implemented by itself.
- I/O bound processes that call interrupt for I/O within one tick will always stay in queue 0 and exploit this property to make the system very responsive to user interaction. This fact is further supported by the lesser number of ticks given in lower priority queues.
<br>

## Specification 3
## Copy-on-write fork
- First, we modified the `uvmcopy()` function in `vm.c` to map the parent's physical pages into the child. Then we clear `PTE_W` in both child and parent.
- This is done because we need to make the parent's writable pages, unwritable, and use a new bit to mark them as COW. We now get the ability to share readable pages between parents and children.
- We created a function `page_trap_handler()` to recognize and handle page faults - it returns -1 when it cannot allocate memory, and -2 when the address provided is invalid.
- `usertrap()` was also modified to handle page faults.
- A few functions were also used to handle "reference count" of the no. of user page tables which refer to the page. These functions included `init_page_ref`, `dec_page_ref`, `inc_page_ref`, `get_page_ref`. These reference counts were stored in an array `count`.
- We also modified `copyout()` to use the same page fault scheme when it encounters a COW page.
<br>

## Scheduling Algorithms Analysis
RR : rtime : 13, wtime : 112
<br>
FCFS : rtime : 30, wtime : 35
<br>
LBS  : rtime : 15, wtime : 106
<br>
PBS : rtime : 15, wtime : 100
<br>
MLFQ : rtime : 14, wtime : 145
<br>
The performance of the various scheduling algorithms give us specifics on how each algorithm influences run and wait time of the processes. We see that LBS and PBS have very similar performance times as RR. Both PBS and LBS have very good run times, and their wait times can be attributed to choosing the best process in the queue for running next. FCFS has very less wait time as it does not care about which one to run next, and simply runs the next in the queue, reducing overhead in choosing. Also, no requirement of preemption gives very less wait time compared to other processes. However, it's high run time, due to lack of optimisation of process scheduling in a smart way, is not desirable. MLFQ has the best output overall. We ran all scheduling algorithms in 3 CPUs except for MLFQ and yet it performed very well. The movement in queues and updating various times in processes after each tick paired with verifying and choosing the next process gives it a high wait time.

## MLFQ Analysis
We did the plot for 5 processes as shown in the graph.