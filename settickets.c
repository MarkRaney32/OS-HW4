#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int
main(int argc, char *argv[])
{
  struct pstat pstats;

  int initial_fork = fork();
  if(initial_fork == 0) {
    settickets(10);
    sleep(50);
  } else if (initial_fork > 0) {
    int new_fork = fork();
    if(new_fork == 0) {
      settickets(20);
      sleep(50);
    } else {
      int final_fork = fork();
      if(final_fork == 0){
        settickets(30);
        sleep(50);
      }
    }
  }

  for(int i = 0; i < 3; i++) {
    wait();
  }

  if(initial_fork == 0){
    getpinfo(&pstats);
  }

  exit();
}
