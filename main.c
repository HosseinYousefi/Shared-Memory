#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MEM_SZ 1024

int main(int argc, const char * argv[]) {
    volatile void *mem = mmap(NULL, MEM_SZ, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    
    volatile int* state = mmap(NULL, 1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    
    enum states {
        CHILD_ACCESS,
        PARENT_ACCESS
    };
    
    *state = CHILD_ACCESS;
    
    if(mem == NULL) {
        printf("Failed to map memory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    while (1) {
        if(pid == 0) {
            // Child process
            if (*state == CHILD_ACCESS) {
                printf("Dad: %s\n", (char*)mem);
                const char* msg = "Hi daddy!";
                strncpy((char*)mem, msg, strlen(msg) + 1);
                *state = PARENT_ACCESS;
            }
        } else {
            // Parent process
            if (*state == PARENT_ACCESS) {
                printf("Child: %s\n", (char*)mem);
                const char* msg = "Hello my son :)";
                strncpy((char*)mem, msg, strlen(msg) + 1);
                *state = CHILD_ACCESS;
            }
        }
    }
    munmap((char*)mem, MEM_SZ);
    return 0;
}
