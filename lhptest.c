#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "chtrie.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "log_malloc.h"
#include "marktime.h"

#define fatal(s) do { perror(s); exit(-1); } while (0)
#define N 102774*3
#define M 256

static char *dict1[] = { "", "the", "a", "an" };
static char *dict2[] = { "he", "she", "his", "hers" };
static char *dict3[] = { "this", "that" };
static char *stop[] = { "the", "an", "a" };

static chtrie *tr;

static int term[N];	/* is `i` a  termination node */
static int nchild[N];	/* number of children of `i` */

static void add(char *s);
static void del(char *s);
static int query(char *s);

int main(void)
{
	static char line[N];
	int i, j;

	if (!(tr = chtrie_alloc(N, M)))
		fatal("chtrie_alloc");
    FILE *datain = fopen("/usr/share/dict/words","r");
    if(datain==NULL) fatal("can't open /usr/share/dict/words");

    marktime();
	while (fgets(line, sizeof line, datain)) {
        line[strcspn(line, "\n")] = '\0';
        add(line);
	//	printf("%d\n", query(line) ? 1 : 0);
	}
	fclose(datain);
    marktime();
    char *cmd;
    while(1) {
        cmd = readline("test2> ");
        if(cmd==NULL||strlen(cmd)==0)break;
        marktime();
        int q = query(cmd);
        logtime(cmd);
        printf("%s -> %d\n", cmd, q);
#undef free
        free(cmd);
#define free log_free
    }
    chtrie_free(tr);
    print_timemarks();
    log_malloc_report();
    return 0;
}

static void add(char *s)
{
	int it;

	it = 0;
	while (*s) {
		if (chtrie_walk(tr, it, (unsigned char)*s, 0) == -1)
			nchild[it]++;
		if ((it = chtrie_walk(tr, it, (unsigned char)*s, 1)) == -1)
			fatal("chtrie_walk");
		s++;
	}
	term[it] = 1;
}

static void del(char *s)
{
	static int nodes[N], symbs[N], n;	/* trace the path */
	int it;

	n = 0;
	it = 0;
	while (it >= 0 && *s) {
		nodes[n] = it;
		symbs[n] = *s;
		n++;
		it = chtrie_walk(tr, it, (unsigned char)*s++, 0);
	}
	if (it < 0 || !term[it])
		return;
	term[it] = 0;
	while (it > 0 && !term[it] && nchild[it] == 0) {
		n--;
		chtrie_del(tr, nodes[n], symbs[n]);
		it = nodes[n];
		nchild[it]--;
	}
}

static int query(char *s)
{
	int it;

	it = 0;
	while (it >= 0 && *s)
		it = chtrie_walk(tr, it, (unsigned char)*s++, 0);
	if(it >= 0 && term[it]) return it;
    return 0;
}
