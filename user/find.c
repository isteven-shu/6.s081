#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define BUF_SIZE 512

void find(char *path_buf, char *name)
{
    /* Open the path. */
    int fd;
    if((fd = open(path_buf, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path_buf);
        return;
    }

    /* Check if it's a dir. */
    struct stat st;
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path_buf);
        close(fd);
        return;
    }
    if (st.type != T_DIR) {
        fprintf(2, "find: %s is not a dir\n", path_buf);
        close(fd);
        return;
    }

    /* Chekc path length. */
    if(strlen(path_buf) + 1 + DIRSIZ + 1 > BUF_SIZE){
        printf("find: path too long\n");
        return;
    }

    /* Iterate through the path. */
    char *p;
    p = path_buf + strlen(path_buf);
    *p++ = '/';
    struct dirent de;
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0) { continue; }
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) { continue; }
        
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(path_buf, &st) < 0){
            printf("find: cannot stat %s\n", path_buf);
            continue;
        }
        if (st.type == T_DIR) {
            find(path_buf, name);
        }
        if (strcmp(de.name, name) == 0) {
            printf("%s\n", path_buf);
        }
    }
}

int
main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(2, "Usage: find path filename\n");
        exit(1);
    }

    char buf[BUF_SIZE];
    if(strlen(argv[1]) + 1 + DIRSIZ + 1 > BUF_SIZE){
        printf("find: path too long\n");
        exit(1);
    }
    strcpy(buf, argv[1]);
    buf[strlen(argv[1])] = 0;

    find(buf, argv[2]);

    exit(0);
}
