#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include "lwp.h"
#include "rr.c"
#include "ll.c"
//needed for map_anonymous and map_stack
#include <asm-generic/mman-common.h>
#include <asm-generic/mman.h>

#define STACK_SIZE_DEFAULT (8 * 1024 * 1024) // 8MB
//LWP_TERM = 1
//LWP_LIVE = 0

tid_t threadId = 1;
thread currThread = NULL;
static scheduler s = &rr; 
static LinkedList *wList;
static LinkedList *tList;

void lwp_exit(int exitval){ 
     currThread->status = MKTERMSTAT(LWP_TERM, exitval); 
     s->remove(currThread);
     int wLen = len(wList);
     if (wLen > 0){
        thread waitThread = dequeue(wList);
        s->admit(waitThread);
        waitThread->exited = currThread;
     } else {
        enqueue(tList, currThread);
     }
     lwp_yield();
}

static void lwp_wrap(lwpfun function, void* argument){
    int rval;
    rval = function(argument);
    lwp_exit(rval);
}

tid_t lwp_create(lwpfun function, void* argument){
    thread newThread = (thread)malloc(sizeof(context));  
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
    unsigned long *stack = mmap(NULL, rlim.rlim_cur, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    newThread->stack = (unsigned long *)stack;
    
    if (stack == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    if ((uintptr_t)stack % 16 != 0){
        perror("Stack not aligned");
        exit(EXIT_FAILURE);
    }
    stack += rlim.rlim_cur / sizeof(unsigned long);

    stack--; //decrement 8 bits
    stack--; //decrement 8 bits

    *stack = (unsigned long)lwp_wrap;
    stack--; //decrement 8 more bits

    newThread->state.rsp = (unsigned long)stack;
    newThread->state.rbp = (unsigned long)stack; 

    // Initialize Thread Info
    newThread->tid = threadId;
    newThread->stacksize = (size_t)rlim.rlim_cur; 
    newThread->state.fxsave = FPU_INIT;
    newThread->state.rdi = (unsigned long)function;
    newThread->state.rsi = (unsigned long)argument;
    newThread->status = LWP_LIVE;
    newThread->exited = NULL;
    newThread->lib_one = NULL;
    newThread->lib_two = NULL;
    newThread->sched_one = NULL;
    newThread->sched_two = NULL;

    threadId += 1;
    s->admit(newThread);
    return threadId-1;
}

tid_t lwp_gettid(void){
    return threadId;
}

void lwp_yield(void){
    thread next = s->next();
    if (next == NULL){
        if ((currThread->stack) != NULL){
            munmap(currThread->stack, currThread->stacksize);
        }
        free(currThread);
        exit(-1);
    }
    thread temp = currThread;
    currThread = next; 
    swap_rfiles(&(temp->state), &(currThread->state));
}

void lwp_start(void){
    wList = createLinkedList();
    tList = createLinkedList();
    thread newThread = (thread)malloc(sizeof(context));
    newThread->tid = threadId;
    newThread->stack = NULL;
    newThread->status = LWP_LIVE;

    threadId += 1;

    s->admit(newThread);
    currThread = newThread;
    lwp_yield();
}

tid_t lwp_wait(int *status){
    tid_t id = NO_THREAD;
    thread removed;
    if (len(tList) > 0){
        removed = dequeue(tList);
        if ((removed->stack) != NULL){
            munmap(removed->stack, removed->stacksize);
        }
        id = removed->tid;
        if (status){
            *status = removed->status;
        }
        free(removed);
    } else if (s->qlen() > 1){
        enqueue(wList, currThread);
        s->remove(currThread);
        lwp_yield();
        removed = currThread->exited;
        if ((removed->stack) != NULL){
            munmap(removed->stack, removed->stacksize); //<---- BAD FREE IS HERE
        }
        id = removed->tid;
        if (status){
            *status = removed->status;
        }
        free(removed);
    }
    return id;
}

void lwp_set_scheduler(scheduler sched){
    if (sched == NULL){
        printf("Invalid scheduler. \n");
        s = &rr;
    }
    s = sched;
}

scheduler lwp_get_scheduler(void){
    return s;
}

thread tid2thread(tid_t tid){
    thread curr = currThread;
    while (curr != NULL){
        if (curr->tid == tid){
            return curr;
        }
        curr = curr->lib_one;
    }
    return NULL;
}


void test(){
    printf("Hello\n");
}
