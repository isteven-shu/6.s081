#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int i = 2;
    int pl[2];
    pipe(pl);

    int pid = fork();
    /** Main process.
     *  Feed all numbers into pipe pl.
     */
    if (pid > 0) {
        close(pl[0]);
        for (; i <= 35; ++i) {
            write(pl[1], &i, 4);
        }
        close(pl[1]);
        wait((int *)0);

    /** Child processes.
     *  Read from pipe pl, display the first number as prime and write undividable numbers into pipe pr.
     */
    } else if (pid == 0) {
        // At the beginning, every process has 2 file descriptors open, which are stored in pl.
        while (1) {
            close(pl[1]);   // Important! Otherwise line35 will block!

            int prime;
            read(pl[0], &prime, 4);
            fprintf(1, "prime %d\n", prime);

            int n;
            // If only one number is transmitted to this pipe, then done.
            if (read(pl[0], &n, 4) == 0) {
                close(pl[0]);
                exit(0);
            }
            
            int pr[2];
            pipe(pr);
            pid = fork();
            if (pid < 0) {
                close(pl[0]);
                close(pr[0]);
                close(pr[1]);
                exit(1);
            } else if (pid == 0) {
                close(pl[0]);
                pl[0] = pr[0];
                pl[1] = pr[1];
            } else {
                close(pr[0]);
                
                do {
                    if (n % prime != 0) {
                        write(pr[1], &n, 4);
                    }
                } while (read(pl[0], &n, 4) != 0);

                close(pl[0]);
                close(pr[1]);
                wait((int *)0);
                exit(0);
            }
        }
    } else {
        exit(1);
    }
  
    exit(0);
}
