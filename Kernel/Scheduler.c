#include <Scheduler.h>
#include <Process.h>
#include <liballoc.h>
#include <asmlib.h>
#define NULL 0

typedef struct ProcessNode ProcessNode;
typedef struct SleepNode SleepNode;

struct ProcessNode {
	Process * p;
	uint8_t * descr;
	uint8_t skip;
	SleepNode * sn;
	ProcessNode * next;
	ProcessNode * prev;
};

struct SleepNode {
	uint64_t ticks;
	ProcessNode * pn;
	SleepNode * next;
	SleepNode * prev;
};

extern void changeContextFromRsp(uint64_t rsp);
void schedule();
void sleep(uint64_t ticks);
static ProcessNode * deleteProcessNode(ProcessNode * p);
static void deleteSleepNode(SleepNode * sn);
static SleepNode * decrementTicksR(SleepNode * sn);

ProcessNode * current = NULL;
static int process_count = 0;

SleepNode * sleeping = NULL;

void insertProcess(void * entry_point,uint64_t rax,uint64_t rdi, uint64_t rsi,uint8_t * descr){
	Process * p;
	if(current == NULL){
		p=newProcess(entry_point,rax,rdi,rsi,0);
	} else {
		p=newProcess(entry_point,rax,rdi,rsi,current->p->ppid);
	}
	ProcessNode * pnode = la_malloc(sizeof(ProcessNode));
	pnode->p = p;
	pnode->descr = descr;
	pnode->skip = 0;
	pnode->sn = NULL;
	if(current == NULL){
		current = pnode;
		pnode->next = pnode;
		pnode->prev = pnode;
	} else {
		ProcessNode * tmp = current->next;
		current->next = pnode;
		pnode->next = tmp;
		pnode->prev = current;
		tmp->prev  = pnode;
	}
	process_count++;
	return;
}

void deleteProcessScheduler(uint64_t pid){
	if(pid == 0 || pid == 1){
		return;
	}
	if(current->p->pid == pid){
		exit();
	} else {
		ProcessNode * curr = current;
		for(int i=0; i<process_count ; i++ , curr = curr->next){
			if(curr->p->pid == pid){
				ProcessNode * cn = curr;
				deleteProcessNode(cn);
				deleteProcess(cn->p);
				la_free(cn);
				process_count--;
				break;
			}
		}
	}
}

void schedule(){
	current = current->next;
	for(int i=0; i<process_count ; i++ , current = current->next){
		if(!current->skip){
			break;
		}
	}
	return;
}

void * switchStackPointer(void * rsp){
	current->p->stack_pointer = rsp;
	schedule();
	return current->p->stack_pointer;
}

void fkexec(void * entry_point,uint8_t * descr,Args * args){
	insertProcess(entry_point,0,args->argc,args->argv,descr);
}

void begin(){
	_sti();
	((void (*)(void))(current->p->entry_point))();
}

void exit(){
	ProcessNode * cn = current;
	current = deleteProcessNode(cn);
	deleteProcess(cn->p);
	la_free(cn);
	process_count--;
	changeContextFromRsp(current->p->stack_pointer);
}

void * ps(){
	ProcessInfo * ans = la_malloc(sizeof(ProcessInfo));
	ans->process_count = process_count;
	ans->PIDs = la_malloc(sizeof(uint64_t)*process_count);
	ans->descrs = la_malloc(sizeof(uint8_t *)*process_count);
	ans->status = la_malloc(sizeof(uint8_t *)*process_count);
	ProcessNode * curr = current;
	for(int i=0; i<process_count ; i++ , curr = curr->next){
		(ans->PIDs)[i]=curr->p->pid;
		(ans->descrs)[i]=curr->descr;
		if(curr == current){
			(ans->status)[i]="running";
		} else if (curr->skip){
			(ans->status)[i]="sleeping";
		} else {
			(ans->status)[i]="waiting";
		}
	}
	return (void *)ans;
}


void sys_sleep(uint64_t ticks){
	current->skip = 1;
	SleepNode * sn = current->sn ;
	if(sn != NULL){
		sn->ticks = ticks;
		return;
	} 
	sn = la_malloc(sizeof(SleepNode));
	sn->ticks = ticks;
	sn->pn = current;
	current->sn = sn;
	if (sleeping == NULL){
		sleeping = sn;
		sn->next = NULL;
		sn->prev = NULL;
	} else {
		sn->next = sleeping;
		sleeping->prev = sn;
		sleeping = sn;
	}
	yield();
}

void decrementTicks(){
	sleeping = decrementTicksR(sleeping);
}

static SleepNode * decrementTicksR(SleepNode * sn){
	if(sn == NULL){
		return NULL;
	}
	if(sn->ticks == 0){
		SleepNode * t = sn->next;
		sn->pn->skip = 0;
		sn->pn->sn = NULL;
		la_free(sn);
		return decrementTicksR(t);
	}
	sn->ticks --;
	sn->next = decrementTicksR(sn->next);
	return sn;
}

static ProcessNode * deleteProcessNode(ProcessNode * n){
	ProcessNode * cn = n;
	ProcessNode * prev = cn->prev;
	ProcessNode * next = cn->next;
	prev->next = next;
	next->prev = prev;
	SleepNode * sn = n->sn;
	if(sn != NULL){
		deleteSleepNode(sn);
	}
	return next;
}

void wake(uint64_t pid){
	ProcessNode * pn = current;
	for(int i=0 ; i<process_count ; i++ , pn=pn->next){
		if(pn->p->pid == pid){
			pn->skip = 0;
			SleepNode * sn = current->sn;
			if(sn != NULL){
				deleteSleepNode(sn);
				current->sn = NULL;
			}
			break;
		}
	}
}

static void deleteSleepNode(SleepNode * sn){
	if(sn->prev == NULL && sn->next == NULL){
		sleeping == NULL;
	} else if(sn->prev == NULL){
		sleeping = sn->next;
	} else if(sn->next == NULL){
		sn->prev->next = NULL;
	} else {
		sn->prev->next = sn->next;
		sn->next->prev = sn->prev;
	}	
	la_free(sn);
}

void mkwait(uint64_t pid){
	ProcessNode * pn = current;
	if(pn->p->pid == pid && pid!=0 && pid!=1){
		wait();
		return;
	}
	for(int i=0 ; i<process_count ; i++ , pn=pn->next){
		if(pn->p->pid == pid && pid!=0 && pid!=1){
			SleepNode * sn = pn->sn;
			if(sn != NULL){
				deleteSleepNode(sn);
				pn->sn = NULL;
			}
			pn->skip = 1;
			break;
		}
	}
}

void wait(){
	current->skip = 1;
	SleepNode * sn = current->sn;
	if(sn != NULL){
		deleteSleepNode(sn);
		current->sn = NULL;
	}
	yield();
	return;
}
