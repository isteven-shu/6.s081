#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p1[2];    // parent -> child
  int p2[2];    // child -> parent
  pipe(p1);
  pipe(p2);

  char *buf[1];
  
  int pid = fork();
  if (pid == 0) {
    /* One direction pipe. */
    close(p1[1]);
    close(p2[0]);
    /* Read and write. */
    read(p1[0], buf, 1);
    close(p1[0]);
    fprintf(1, "%d: received ping\n", getpid());
    write(p2[1], buf, 1);
    close(p2[1]);
    exit(0);
  } else if (pid > 0) {
    /* One direction pipe. */
    close(p1[0]);
    close(p2[1]);
    /* Write and read. */
    write(p1[1], buf, 1);
    close(p1[1]);
    read(p2[0], buf, 1);
    fprintf(1, "%d: received pong\n", getpid());
    close(p2[0]);
    exit(0);
  } else {
    exit(1);
  }
  
  exit(0);
}

  /* 为什么一定要wait？因为parent write之后直接就自己给read了，child卡在了read处。
  int pid;
  int p[2];
  pipe(p);

  if (fork() == 0) // child (receive -> send)
  {
      pid = getpid();
      char buf[2];
      if (read(p[0], buf, 1) != 1)
      {
          fprintf(2, "failed to read in child\n");
          exit(1);
      }
      close(p[0]);
      printf("%d: received ping\n", pid);
      if(write(p[1], buf, 1) != 1)
      {
          fprintf(2, "failed to write in child\n");
          exit(1);
      }
      close(p[1]);
      exit(0);
  }else{			// parent (send -> receive)
      pid = getpid();
      char info[2] = "a";
      char buf[2];
      buf[1] = 0;
      if (write(p[1], info, 1) != 1)
      {
          fprintf(2, "failed to write in parent\n");
          exit(1);
      }
      // wait for child to receive ping
      close(p[1]);
      wait(0);
      if(read(p[0], buf, 1) != 1){
          fprintf(2, "failed to read in parent\n");
          exit(1);
      }
      printf("%d: received pong\n", pid);
      close(p[0]);
      exit(0);
  }
  */
