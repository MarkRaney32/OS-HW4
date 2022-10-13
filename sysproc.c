#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  //Added code - updating tickets on exit
  acquire(&ptable.lock);
  update_tickets(0);
  release(&ptable.lock);

  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{

  //Added code - updating tickets on kill
  acquire(&ptable.lock);
  update_tickets(0);
  release(&ptable.lock);

  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// NEW SYS CALLS
// Mark & Greg - OS

int
sys_settickets(void) {

  // gets the parameter for # of tickets
  int ticketVal;
  argint(0, &ticketVal);
  if(ticketVal < 1){
    return -1;
  }

  acquire(&ptable.lock);
  update_tickets(ticketVal);
  release(&ptable.lock);
  return 0;

}

int sys_getpinfo(void) {

  // argptr assigns the pointer value of parameter
  // to our pstat object pst
  struct pstat *pst;
  if(argptr(1, (void*)&pst, sizeof(*pst)) < 0)
    return -1;

  int index = 0;
  struct proc *p;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    pst->pid[index] = p->pid;
    pst->inuse[index] = p->state != UNUSED;
    pst->tickets[index] = p->tickets;
    pst->ticks[index] = p->ticks;
    if(pst->inuse[index] != UNUSED) {
      cprintf("PID: %d      tickets: %d        ticks: %d\n", pst->pid[index], pst->tickets[index], pst->ticks[index]);
    }
  }
  release(&ptable.lock);

  return 0;
}
