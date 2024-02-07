#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include "lwp.h"
#include "rr.c"
//needed for map_anonymous and map_stack
#include <asm-generic/mman-common.h>
#include <asm-generic/mman.h>

#define STACK_SIZE_DEFAULT (8 * 1024 * 1024) // 8MB
#define LWP_RUNNING     1
#define LWP_READY       2
#define LWP_BLOCKED     3
#define LWP_TERMINATED  4

tid_t threadId = 1;

static scheduler s = &rr;
//access s with s->admin(lwp1) or s->next()

//lwp exit exits pog
//lwp wrap take eveyrthing manually (function, argument, return value lwpexit)
//get context of register from swaprfiles

//state struct 

int lwp_wrap(lwpfun function, void* argument){

}

tid_t lwp_create(lwpfun function, void* argument){
    thread newThread = (thread)malloc(sizeof(thread));
    //printf("got here\n");
    
    long page_size = sysconf(_SC_PAGE_SIZE);
    if (page_size == -1){
        perror("sysconf");
        return -1;
    }

    // Check stack size resource limit
    struct rlimit rlim;
    if (getrlimit(RLIMIT_STACK, &rlim) != 0 || rlim.rlim_cur == RLIM_INFINITY){
        rlim.rlim_cur = STACK_SIZE_DEFAULT;
    }

    // Round to nearest multiple of page size
    rlim.rlim_cur = (rlim.rlim_cur + page_size - 1) / page_size * page_size;
    printf("%d\n", rlim.rlim_cur);

    void* stack = mmap(NULL, rlim.rlim_cur, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    newThread->tid = threadId;
    newThread->stack = stack;
    newThread->stacksize = rlim.rlim_cur; 
    newThread->status = LWP_READY;
    newThread->exited = 0;
    newThread->lib_one = NULL;
    newThread->lib_two = NULL;
    newThread->sched_one = NULL;
    newThread->sched_two = NULL;

    threadId += 1;
}


void test(){
    printf("Hello\n");
}

int main() {
    lwp_create((lwpfun)&test, NULL);
}

//use function call to go back to the main thread that you have
//go back call at the very top of the stack (manually go up there and put it in)
// and then have emptyness below

//remember to unmap using munmap()

//yielding = scheduler determing what is coming next (call to next) swap in context and swap out old one
//yield function = 4 or 5 lines
//swap context, determine which thread you are yielding to

//take pointer to stack (base of the stack and add the size of the stack to jump to top of stack)
//then put exit to go back to main function