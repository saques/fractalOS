#include <queue.h>
#include <cond_variable.h>
#include <stdint.h>
typedef struct SQueue {
	Queue_p q;
	cond_variable * full_buffer;
	cond_variable * empty_buffer;
	mutex * m;
	int max_size;
} SQueue;

SQueue * squeue_init(int max_size)
{
	SQueue * new = malloc(sizeof(new));
	new->q = new_queue();
	new->full_buffer = cond_variable_init();
	new->empty_buffer = cond_variable_init();
	new->m = malloc(sizeof(mutex));
	mutex_init(new->m );
	new->max_size = max_size;
	return new;
}

void squeue_destroy(SQueue * sq)
{
	q_destroy(sq->q);
	free(sq->m);
	cond_variable_destroy(sq->full_buffer);
	cond_variable_destroy(sq->empty_buffer);
	free(sq);
}
int8_t is_empty(SQueue * sq)
{
	return q_is_empty(sq->q);
}

void senque(SQueue * sq, int64_t a)
{
	mutex_lock(sq->m);
	if(q_size(sq->q)>= sq->max_size) {
		cond_variable_wait(sq->full_buffer, sq->m);
	}
	enque(sq->q, a);
	mutex_unlock(sq->m);
	cond_variable_signal(sq->empty_buffer);
}

int64_t sdeque(SQueue * sq)
{
	mutex_lock(sq->m);
	if(is_empty(sq)) {
		cond_variable_wait(sq->empty_buffer, sq->m);
	}
	int64_t a = deque(sq->q);
	mutex_unlock(sq->m);
	cond_variable_signal(sq->full_buffer);
	return a;
}

int64_t squeue_size(SQueue * sq)
{
	return q_size(sq->q);
}

int64_t squeue_max_size(SQueue * sq)
{
	return sq->max_size;
}