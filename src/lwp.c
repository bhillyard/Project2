#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include "lwp.h"
#include "rr.c"

tid_t threads = 1;

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
    
    /*long page_size = sysconf(_SC_PAGE_SIZE);
    if (page_size == -1){
        perror("sysconf");
        return -1;
    }

    // Check stack size resource limit
    struct rlimit rlim;
    if (getrliimit(RLIMIT_STACK, &rlim) != 0 || rlim.rlim_cur == RLIM_INFINITY){
        rlime.rlim_cur = STACK_SIZE_DEFAULT;
    }

    // Round to nearest multiple of page size
    rlim.rim_cur = (rlim.rlim_cur + page_size - 1) / page_size * page_size;

    void* stack = mmap(NULL, rlim.rlim_cur, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        return -1;
    }*/
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