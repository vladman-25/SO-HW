// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>

char *strcpy(char *destination, const char *source)
{
	/* TODO: Implement strcpy(). */

	char *d;

	for (d = destination; *source != '\0'; source++, d++)
		*d = *source;

	/* Also place NUL byte. */
	*d = *source;
	return destination;
}

char *strncpy(char *destination, const char *source, size_t len)
{
	/* TODO: Implement strncpy(). */

	char *d;
	size_t i;

	for(d = destination, i = 0; (i < len) && (*source != '\0'); source++, d++, i++)
		*d = *source; //copiez pana cand ajung la lungimea ceruta sau pana cand source se face null

	if (*source != '\0')
		return destination; //daca source nu e null dar s-a ajuns la limita, se intoarce stringul fara null la final 

	for(; i < len; source++, d++, i++) 
		*d = 0; //daca e null iar  lungimea nu este inca acolo, punem 0

	*d = '\0';
	
	return destination;
}

char *strcat(char *destination, const char *source)
{
	/* TODO: Implement strcat(). */

	char *d;
	for(d = destination; *d != '\0'; d++) {

	} //mergem la finalul stringului dest

	for(;*source != '\0'; source++, d++) {
		*d = *source; //adaugam al doilea string in coada
	}
	*d = '\0'; //punem null byte
	return destination;
}

char *strncat(char *destination, const char *source, size_t len)
{
	/* TODO: Implement strncat(). */

	char *d;
	size_t i = 0;
	for(d = destination; *d != '\0'; d++) {}//mergem la finalul stringului dest

	for(;(i < len) && (*source != '\0'); source++, d++, i++)
		*d = *source; //adaugam al doilea string in coada

	*d = '\0';

	return destination;
}

int strcmp(const char *str1, const char *str2)
{
	/* TODO: Implement strcmp(). */
	for(; (*str1 == *str2)&&(*str1 != '\0')&&(*str2 != '\0'); str1++, str2++) {

	}

	return *str1 - *str2;
}

int strncmp(const char *str1, const char *str2, size_t len)
{
	/* TODO: Implement strncmp(). */
	size_t i = 0;
	for(; (*str1 == *str2)&&(*str1 != '\0')&&(*str2 != '\0')&&(i < len); str1++, str2++, i++) {

	}
	if (i < len) {
		return (int)(*(str1) - *(str2));
	} else {
		return (int)(*(str1-1) - *(str2-1));
	}
}

size_t strlen(const char *str)
{
	size_t i = 0;

	for (; *str != '\0'; str++, i++)
		;

	return i;
}

char *strchr(const char *str, int c)
{
	/* TODO: Implement strchr(). */

	for(; *str != '\0'; str++) {
		if(*str == c) {
			return str;
		}
	}
	return NULL;
}

char *strrchr(const char *str, int c)
{
	/* TODO: Implement strrchr(). */

	char *last = NULL;
	for(; *str != '\0'; str++) {
		if(*str == c) {
			last = str;
		}
	}
	return last;
}

char *strstr(const char *haystack, const char *needle)
{
	/* TODO: Implement strstr(). */
	int len_s2 = strlen(needle);
	int len_s1 = strlen(haystack);
	int i = 0;
	for(;(i <= len_s1 - len_s2); haystack++, i++) {
		if(strncmp(haystack,needle,len_s2) == 0) {
			return haystack;
		}
	}
	return NULL;
}

char *strrstr(const char *haystack, const char *needle)
{
	/* TODO: Implement strrstr(). */
	int len_s2 = strlen(needle);
	int len_s1 = strlen(haystack);
	int i = 0;
	char *last = NULL;
	for(;(i <= len_s1 - len_s2); haystack++, i++) {
		if(strncmp(haystack,needle,len_s2) == 0) {
			last = haystack;
		}
	}
	return last;
}

void *memcpy(void *destination, const void *source, size_t num)
{
	/* TODO: Implement memcpy(). */
	char *d = destination;
	char *s = source;
	size_t i = 0;
	for (; i < num; s++, d++, i++)
		*d = *s;

	return destination;
}

void *memmove(void *destination, const void *source, size_t num)
{
	/* TODO: Implement memmove(). */
	char *d = destination;
	char *s = source;
	char *tmp;
	char *tmp_aux = tmp;


	size_t i;
	for (i = 0; i < num; s++, tmp_aux++, i++) {
		*tmp_aux = *s;
	}
	for (i = 0; i < num; d++, tmp++, i++) {
		*d = *tmp;
	}

	return destination;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	/* TODO: Implement memcmp(). */
	char *m1 = ptr1;
	char *m2 = ptr2;
	size_t i = 0;
	for (; (i < num) && (*m1 == *m2); m1++, m2++, i++) {
	}

	return (*(m1 - 1) - *(m2 - 1));
}

void *memset(void *source, int value, size_t num)
{
	/* TODO: Implement memset(). */
	char *s = source;
	size_t i = 0;
	for (; (i < num); s++, i++) {
		*s = value;
	}

	return source;
}
