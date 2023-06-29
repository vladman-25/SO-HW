#include "osmem.h"
#include "helpers.h"

//constants/macros
#define M_MMAP_THRESHOLD 128 * 1024
#define PRE_PADDING (8 - (sizeof(struct block_meta) % 8)) % 8
#define HEADER_SIZE sizeof(struct block_meta) + PRE_PADDING
#define POST_PADDING(content_size) (8 - content_size % 8) % 8

//linked list memory
int heapEnd = 0;
struct block_meta *block_start = NULL;
struct block_meta *block_end = NULL;


//find best-fit block
void *find_free_block(size_t size)
{
	struct block_meta *block_current = block_start;
	struct block_meta *block_candidate = NULL;

	while (block_current != NULL)
	{	
		//block must be free
		if (block_current->status == STATUS_FREE)
		{
			//block size must be bigger than what we need
			if (block_current->size >= size)
			{
				//but not too big, we take the smallest possible
				if (block_candidate == NULL || (block_candidate->size > block_current->size))
				{
					block_candidate = block_current;
				}
			}
		}
		block_current = block_current->next;
	}
	return block_candidate;
}

// split a memory block IF we can (calculations are done before)
void *split_block(struct block_meta *block_candidate, size_t size_taken)
{

	struct block_meta *block_next = block_candidate->next;
	
	size_t available = block_candidate->size + POST_PADDING(block_candidate->size);
	size_t newSize = size_taken + POST_PADDING(size_taken);

	void *head = (void*) block_candidate + HEADER_SIZE + size_taken + POST_PADDING(size_taken);
	struct block_meta *new_block = head;

	//set new block as FREE
	new_block->status = STATUS_FREE;
	new_block->size = available - newSize - HEADER_SIZE;
	new_block->next = block_next;
	
	//set new block as next in linked list and current block new size and status
	block_candidate->size = size_taken;
	block_candidate->status = STATUS_ALLOC;
	block_candidate->next = new_block;
	head = block_candidate;
	head += HEADER_SIZE;
	return head;


}

//expand a block size if it is the last
void *expand_block(size_t size)
{

	int current_padded_size = block_end->size + POST_PADDING(block_end->size);
	int diff = size - current_padded_size;
	if (diff > 0)
	{
		int post_padding = POST_PADDING(diff);
		void *head = sbrk(diff + post_padding);
		heapEnd += diff + post_padding;
		block_end->size = size;
		head = block_end;
		head += HEADER_SIZE;
		return head;
	}
	else
	{
		block_end->size = size;
		void *head = block_end;
		head += HEADER_SIZE;
		return head;
	}
}

//merge blocks
void *coalesce_blocks()
{
	struct block_meta *block_current = block_start;
	struct block_meta *block_next = block_current->next;
	int run = 1;
	while ((block_current != NULL) && (block_next != NULL) && run)
	{
		while ((block_next != NULL) && (block_current->status == STATUS_FREE) && (block_next->status == STATUS_FREE))
		{
			size_t current_size = block_current->size + POST_PADDING(block_current->size);
			size_t next_size = HEADER_SIZE + block_next->size + POST_PADDING(block_next->size);
			struct block_meta *future_next = block_next->next;
			block_current->size = current_size + next_size;
			block_current->next = future_next;

			if (future_next == NULL)
			{
				block_end = block_current;
				run = 0;
			}

			block_next = block_current->next;
		}

		if (run == 0)
		{
			break;
		}

		block_current = block_current->next;
		block_next = block_current->next;
	}

}

void *os_malloc(size_t size)
{
	/*TODO: Implement os_malloc */

	if (size == 0)
	{
		return NULL;
	}

	if (block_start != NULL)
	{
		coalesce_blocks();
	}

	if (size < M_MMAP_THRESHOLD)
	{

		if (block_start == NULL)
		{
			heapEnd = sbrk(0);
		}

		int post_padding = POST_PADDING(size);

		void *head;
		if (block_start == NULL)
		{
			head = sbrk(M_MMAP_THRESHOLD);
		}
		else
		{
			struct block_meta *block_candidate = NULL;
			block_candidate = find_free_block(size);

			if ((block_candidate == NULL) && (block_end->status == STATUS_FREE))
			{
				return expand_block(size);
			}

			if (block_candidate == NULL)
			{
				head = sbrk(HEADER_SIZE + size + post_padding);
			}
			else
			{

				int available = block_candidate->size + POST_PADDING(block_candidate->size);
				int needed = size + POST_PADDING(size) + HEADER_SIZE;

				if (available - needed > 0)
				{

					return split_block(block_candidate, size);
				}
				else
				{

					block_candidate->status = STATUS_ALLOC;
					head = block_candidate;
					head += HEADER_SIZE;
					return head;
				}
			}
		}

		if (head == (void*) - 1) return NULL;

		struct block_meta *block_aux = head;

		block_aux->next = NULL;
		block_aux->size = size + post_padding;
		block_aux->status = STATUS_ALLOC;

		if (block_start == NULL)
		{
			block_start = block_aux;
			block_end = block_aux;
		}
		else
		{
			block_end->next = block_aux;
			block_end = block_aux;
		}

		heapEnd += HEADER_SIZE + size + post_padding;
		head += HEADER_SIZE;

		return head;
	}
	else
	{
		int post_padding = POST_PADDING(size);

		void *head = mmap(NULL, HEADER_SIZE + size + post_padding, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (head == MAP_FAILED) return NULL;

		struct block_meta *mapped = (struct block_meta *) head;
		mapped->size = size + post_padding;
		mapped->status = STATUS_MAPPED;
		head += HEADER_SIZE;
		return head;
	}
}

void os_free(void *ptr)
{
	/*TODO: Implement os_free */

	if (ptr == NULL) return;

	struct block_meta *head = ptr - HEADER_SIZE;

	if (head->status == STATUS_ALLOC)
	{
		head->status = STATUS_FREE;
		return;
	}

	if (head->status == STATUS_MAPPED)
	{
		munmap(head, HEADER_SIZE + head->size + POST_PADDING(head->size));
	}

	return;
}

void *os_calloc(size_t nmemb, size_t size)
{
	/*TODO: Implement os_calloc */
	if ((nmemb == 0) || (size == 0))
	{
		return NULL;
	}

	long page_size = sysconf(_SC_PAGESIZE);

	int alloc_size = HEADER_SIZE + nmemb * size;
	alloc_size += POST_PADDING(alloc_size);

	if (alloc_size >= page_size)
	{
		void *head = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (head == MAP_FAILED) return NULL;

		memset(head, 0, alloc_size);
		struct block_meta *mapped = (struct block_meta *) head;
		mapped->size = alloc_size - HEADER_SIZE;
		mapped->status = STATUS_MAPPED;
		head += HEADER_SIZE;
		return head;
	}

	void *head = os_malloc(nmemb *size);
	if (head == NULL)
	{
		return NULL;
	}

	memset(head, 0, alloc_size - HEADER_SIZE);
	return head;
}

void *os_realloc(void *ptr, size_t size)
{
	/*TODO: Implement os_realloc */

	if (ptr == NULL)
	{
		return os_malloc(size);
	}

	if (size == 0)
	{
		os_free(ptr);
		return NULL;
	}

	struct block_meta *block = ptr - HEADER_SIZE;

	if (block->status == STATUS_FREE) return NULL;

	if (size <= block->size)
	{
		int new_status = (size < M_MMAP_THRESHOLD) ? STATUS_ALLOC : STATUS_MAPPED;

		if (block->status == STATUS_MAPPED)
		{
			void *new_addr = os_malloc(size);
			memcpy(new_addr, ptr, size);
			os_free(ptr);
			return new_addr;
		}

		int available = block->size + POST_PADDING(block->size);
		int needed = size + POST_PADDING(size) + HEADER_SIZE;

		if (available - needed > 0)
		{
			return split_block(block, size);
		}
		else
		{
			return ptr;
		}
	}
	else
	{
		int new_status = (size < M_MMAP_THRESHOLD) ? STATUS_ALLOC : STATUS_MAPPED;

		if (new_status == STATUS_MAPPED)
		{
			void *head = os_malloc(size);
			memcpy(head, ptr, block->size);
			os_free(ptr);
			return head;
		}

		struct block_meta *block_current = ptr - HEADER_SIZE;
		struct block_meta *block_next = block_current->next;

		while ((block_next != NULL) && (block_next->status == STATUS_FREE))
		{
			size_t current_size = block_current->size + POST_PADDING(block_current->size);
			size_t next_size = HEADER_SIZE + block_next->size + POST_PADDING(block_next->size);

			struct block_meta *future_next = block_next->next;
			block_current->size = current_size + next_size;
			block_current->next = future_next;
			if (future_next == NULL)
			{
				block_end = block_current;
			}

			block_next = block_current->next;
			if (block_current->size >= size)
			{
				return ptr;
			}
		}

		if (block_current->next == NULL)
		{
			return expand_block(size);
		}

		void *head = os_malloc(size);
		memcpy(head, ptr, block->size);
		os_free(ptr);
		return head;
	}

	return NULL;
}
