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
//LWP_TERM = 1
//LWP_LIVE = 0 and we use those

tid_t threadId = 1;
thread currThread = NULL;
static scheduler s = &rr;
static scheduler t = &rr;
static scheduler w = &rr;
//access s with s->admit(lwp1) or s->next()

//lwp exit exits pog
//lwp wrap take eveyrthing manually (function, argument, return value lwpexit)
//get context of register from swaprfiles

//state struct 

void lwp_exit(int exitval){
     currThread->status = LWP_TERM;
     s->remove(currThread);
     if (w->qlen() > 0){
        thread waitThread = w->next();
        w->remove(waitThread);
        waitThread->exited = currThread;
        s->admit(waitThread);
     } else {
        t->admit(currThread);
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

    unsigned long* stack_top = (unsigned long*)((char*)stack + newThread->stacksize);
    *stack_top = (unsigned long)function;
    stack_top--;
    *stack_top = (unsigned long)argument;
    stack_top--;
    *stack_top = (unsigned long)lwp_wrap(); //lwp(function, argument) ??  + 5? (dummy, lwp_wrap, func, args, ___)


    // Initialize Thread Info
    newThread->tid = threadId;
    newThread->stack = stack;
    newThread->stacksize = rlim.rlim_cur; 
    newThread->state.fxsave = FPU_INIT;
    newThread->state.rdi = (unsigned long)function;
    newThread->state.rbp = (unsigned long)stack; // top of stack address?
    newThread->state.rsi = (unsigned long)argument;
    newThread->state.rsp = (unsigned long)stack;
    newThread->status = LWP_LIVE;
    newThread->exited = 0;
    newThread->lib_one = NULL;
    newThread->lib_two = NULL;
    newThread->sched_one = NULL;
    newThread->sched_two = NULL;


    // put lwp wrap at top of stack so that it executes 
    //subtract 1 from stack pointer = subtract 8
    stack[rlim.rlim_cur] = lwp_wrap(function, argument);
    //put dummy number at s+size-1 
    //put lwpwrap at s+size-2
    //
    threadId += 1;
    
    s->admit(newThread);
    return threadId;
}

tid_t lwp_gettid(void){
    return threadId;
}

void lwp_yield(void){
    //need a mysterious error check here that is not swaprfiles?
    thread next = s->next();
    s->remove(next);
    if (next == NULL){
        if ((currThread->stack) != NULL){
            free(currThread->stack);
        }
        free(currThread);
        exit(-1);
    }
    //first argument will populate the current registers 
    swaprfiles(currThread->state, next->state);
    s->admit(currThread);
    currThread = next;
}

void lwp_start(void){
    thread newThread = (thread)malloc(sizeof(thread));
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
    if (t->qlen() > 0){
        removed = t->next();
        if ((removed->stack) != NULL){
            free(removed->stack);
        }
        id = removed->tid;
        *status = removed->status;
        free(removed);
    } else if (s->qlen() > 1){
        w->admit(currThread);
        s->remove(currThread);
        lwp_yield();
        removed = currThread->exited;
        if ((removed->stack) != NULL){
            free(removed->stack);
        }
        id = removed->tid;
        *status = removed->status;
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

// thread tid2thread(tid_t tid){
// }


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