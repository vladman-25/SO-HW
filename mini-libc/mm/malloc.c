// SPDX-License-Identifier: BSD-3-Clause

#include <internal/mm/mem_list.h>
#include <internal/types.h>
#include <internal/essentials.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

void *malloc(size_t size)
{
	/* TODO: Implement malloc(). */
	void* head = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	int ret = mem_list_add(head,size);
	return head;
}

void *calloc(size_t nmemb, size_t size)
{
	/* TODO: Implement calloc(). */
	void* head = malloc(nmemb * size);
	int ret = memset(head,0,nmemb * size);
	return head;
}

void free(void *ptr)
{
	/* TODO: Implement free(). */
	
	struct mem_list* aux = mem_list_find(ptr);
	munmap(ptr,aux->len);
	mem_list_del(ptr);
	return;
}

void *realloc(void *ptr, size_t size)
{
	/* TODO: Implement realloc(). */
	struct mem_list* head = mem_list_find(ptr);
	void *new = malloc(size);
	memcpy(new,ptr,head->len);
	free(ptr);
	return new;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	/* TODO: Implement reallocarray(). */
	void *head = realloc(ptr,nmemb*size);
	return head;
}
