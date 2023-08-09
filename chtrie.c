#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "chtrie.h"
#include <readline/readline.h>
#include "log_malloc.h"
#include "marktime.h"

#define SZ_MAX ((size_t)-1)
#define MIN(x, y) ((x)<(y)?(x):(y))

chtrie *chtrie_alloc(size_t n, size_t m)
{
	chtrie *tr;

	if (n < 1) n = 1;
	if (m < 1) m = 1;
	if (n > INT_MAX || m > INT_MAX) {
		errno = ERANGE;
		goto err;
	}
	if (MIN(INT_MAX, SZ_MAX) - (n-1) < (n-1) / 3) {
		errno = ERANGE;
		goto err;
	}
	if (!(tr = calloc(1, sizeof *tr))) {
		fprintf(stderr, "calloc failed in chtrie_alloc. calloc(1, sizeof *tr=%d)\n", sizeof *tr);
		goto err;
	}
	tr->maxn = n;
	tr->alphsz = m;
	tr->ecap = (n-1) + (n-1)/3;
	if (!(tr->etab = calloc(tr->ecap, sizeof tr->etab[0]))) {
		fprintf(stderr, "calloc failed in chtrie_alloc. calloc(tr->ecap=%d, sizeof tr->etab[0]=%d)\n", tr->ecap, sizeof tr->etab[0]);
		goto free_tr;
	}
	if (!(tr->idxpool = calloc(n, sizeof tr->idxpool[0]))) {
		fprintf(stderr, "calloc failed in chtrie_alloc. calloc(n=%d, sizeof *tri->idxpool[0]=%d)\n", n, sizeof tr->idxpool[0]);
		goto free_ecap;
	}
	tr->idxmax = 1;
	tr->idxptr = tr->idxpool;
	return tr;
free_ecap:
	free(tr->etab);
free_tr:
	free(tr);
err:
	return NULL;
}

int chtrie_walk(chtrie *tr, int from, int sym, int creat)
{
	struct chtrie_edge *p;
	unsigned long h;

	h = (unsigned long)from*tr->alphsz + sym;
	h %= tr->ecap;
	for (p = tr->etab[h]; p; p = p->next)
		if (p->from == from && p->sym == sym)
			return p->to;
	if (creat) {
		if (tr->idxptr == tr->idxpool && tr->idxmax >= tr->maxn) {
			fprintf(stderr, "internal allocation failed in chtrie_walk. tr->idxmax=%d tr->maxn=%d\n", tr->idxmax, tr->maxn);
			errno = ENOMEM;
			return -1;
		}
		if (!(p = malloc(sizeof *p))) {
			fprintf(stderr, "malloc failed in chtrie_walk. sizeof *p = %d\n", sizeof *p);
			return -1;
		}
		p->next = tr->etab[h];
		tr->etab[h] = p;
		p->from = from;
		p->sym = sym;
		if (tr->idxptr != tr->idxpool)
			p->to = *--tr->idxptr;
		else
			p->to = tr->idxmax++;
		return p->to;
	}
	return -1;
}

void chtrie_del(chtrie *tr, int from, int sym)
{
	struct chtrie_edge *p, *q;
	unsigned long h;

	h = (unsigned long)from*tr->alphsz + sym;
	h %= tr->ecap;
	for (p = tr->etab[h], q = NULL; p; q = p, p = p->next)
		if (p->from == from && p->sym == sym)
			break;
	if (!p)
		return;
	if (q)
		q->next = p->next;
	else
		tr->etab[h] = NULL;
	*tr->idxptr++ = p->to;
	free(p);
}

void chtrie_free(chtrie *tr)
{
	struct chtrie_edge *p, *q;
	int i;

	for (i = 0; i < tr->ecap; i++) {
		p = tr->etab[i];
		while (p) {
			q = p->next;
			free(p);
			p = q;
		}
	}
	free(tr->etab);
	free(tr->idxpool);
	free(tr);
}
