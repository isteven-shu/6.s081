#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

/**
 * Read a line from standard input.
 * @param buf the buffer to accept the read line.
 * @param max the size of the buffer.
 * @return the num of read bytes.
 */
int readline(char *buf, int max)
{
    int i = 0, cc = 0;
    char c;

    while(1) {
        cc = read(0, &c, 1);
        if(cc < 1) {
            buf[i] = 0;
            break;
        }
        
        if(c == '\n' || c == '\r') {
            buf[i++] = 0;
            break;
        } else {
            buf[i++] = c;
        }

        if (i == max) {
            return -1;
        }
    }
    
    return i;
}

/**
 * Split string by spaces.
 * @param buf the string to separate.
 * @param argv the array to accept the separated strings.
 * @param i the first free space in the array.
 * @return the num of the splitted strings.
 */
int split(char *buf, char *argv[], int i)
{
    int orig_i = i;
    int l = 0;
    while (buf[l] != 0) {
        if (i == MAXARG) {
            fprintf(2, "Too much parameters!\n");
            exit(1);
        }

        while (buf[l] == ' ') {
            l++;
        }
        if (buf[l] == 0) {
            break;
        }

        int r = l + 1;
        while ((buf[r] != 0) && (buf[r] != ' ')) {
            r++;
        }

        argv[i] = malloc(r - l + 1);
        memmove(argv[i], buf + l, r - l);
        argv[i][r - l] = 0;

        l = r;
        i++;
    }

    return i - orig_i;
}

int
main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(2, "usage: xargs command (arg...)\n");
        exit(1);
    }
    
    static char buf[100];
    int j = 0;
    while ((j = readline(buf, sizeof(buf))) > 0) {
        char *new_argv[MAXARG + 1];
        for (int i = 1; i < argc; ++i) {
            new_argv[i - 1] = argv[i];
        }
        
        int n = split(buf, new_argv, argc - 1);
        new_argv[argc - 1 + n] = 0;
        
        if (fork() == 0) {
            exec(new_argv[0], new_argv);
        } else {
            wait((int *)0);
            for (int i = argc; i < n; ++i) {
                free(argv[i]);
            }
        }
    }

    exit(0);
}