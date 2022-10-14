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
- The following data values were added to proc data-structure to implement sigalarm/sigreturn:
    `alarm_handler` - stores the address of handler function called by sigalarm when triggered
    `ticks` - stores no. of ticks before sigalarm is triggered (syscall value)
    `cur_ticks` - stores no. of ticks which have passed
    `alarm_trapframe` - stores a copy of the variables from the original trapframe to prevent overwrites.
    `alarm_on` - shows status of the alarm
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

## MLFQ (Multi Level Feedback Queue)

## Specification 3
## Copy-on-write fork
- First, we modified the `uvmcopy()` function in `vm.c` to map the parent's physical pages into the child. Then we clear `PTE_W` in both child and parent.
- This is done because we need to make the parent's writable pages, unwritable, and use a new bit to mark them as COW. We now get the ability to share readable pages between parents and children.
- We created a function `page_trap_handler()` to recognize and handle page faults - it returns -1 when it cannot allocate memory, and -2 when the address provided is invalid.
- `usertrap()` was also modified to handle page faults.
- A few functions were also used to handle "reference count" of the no. of user page tables which refer to the page. These functions included `init_page_ref`, `dec_page_ref`, `inc_page_ref`, `get_page_ref`. These reference counts were stored in an array `count`.
- We also modified `copyout()` to use the same page fault scheme when it encounters a COW page.


## Scheduling Algorithms Analysis
Round Robin :   wtime   : 13      rtime    : 112
<br>
FCFS        :   wtime   : 30      rtime   :  35
<br>
LBS         :   wtime   : 15      rtime   : 106
<br>
PBS         :   wtime   : 15       rtime   : 100
<br>
MLFQ        :   wtime   : 14       rtime   : 145
