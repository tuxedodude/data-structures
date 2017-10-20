#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEWLINE putchar('\n');

struct bintree {
        void *item;
        struct bintree *left;
        struct bintree *right;
};

struct entry {
        int x;
};

typedef struct bintree* BT;

int cmp_entry(void *a, void *b);
void destroy_entry(void *e);
void print_entry(void *e);

static inline BT bt_alloc(void) { return (BT) malloc(sizeof(struct bintree)); }

void putstr(char *s)
{
        char c;
        while ((c = *s++) != '\0')
                putchar(c);
}

BT newtree(void)
{
        BT n = bt_alloc();
        n->left = n->right = NULL;
        n->item = NULL;
        return n;
}

static BT new_node(void *item)
{
        BT n = newtree();
        n->item = item;
        return n;
}

void free_tree(BT tree, void (*destroy_item)(void *))
{
        if (NULL == tree)
                return;
                
        free_tree(tree->left, destroy_item);
        free_tree(tree->right, destroy_item);
        destroy_item(tree->item);
        free(tree);
}

void *lookup(void *key, BT tree, int (*cmp)(void *, void *))
{
        int ord;
        if (NULL == tree)
                return NULL;
        
        ord = cmp(key, tree->item);
        
        if (0 == ord) 
                return tree->item;
        else if (ord < 0)
                return lookup(key, tree->left, cmp);
        else
                return lookup(key, tree->right, cmp);
}

int isempty(BT tree) { return NULL != tree && NULL == tree->item; }

void insert(void *item, BT tree, int (*cmp)(void *, void *))
{
        int ord;
        BT *child;
        if (NULL == tree)
                return;
        /* null element is empty tree */
        else if (isempty(tree)) 
                tree->item = item;
        
        ord = cmp(item, tree->item);
        if (0 == ord)
                return;
                
        child = (ord < 0) ? &(tree->left) : &(tree->right);
        
        if (NULL == *child)
                *child = new_node(item);
        else
                insert(item, *child, cmp);
}

void inorder_traversal(BT tree, void (*callback)(void *))
{
        if (NULL == tree)
                return;
                
        inorder_traversal(tree->left, callback);
        callback(tree->item);
        inorder_traversal(tree->right, callback);
}

void printtabs(int depth)
{
        NEWLINE
        for (int i=0; i<depth; i++)
                putstr("   ");
}

static void help_prettyprint(BT tree, void (*printfunc)(void *), int depth)
{
        if (NULL == tree) {
                putstr("NIL");
        } else {
                printtabs(depth); 
                putchar('(');
                printfunc(tree->item);
                putchar(' ');
                help_prettyprint(tree->left, printfunc, depth+1);
                putchar(' ');
                help_prettyprint(tree->right, printfunc, depth+1);
                putchar(')');
        }
}

void prettyprint(BT tree, void (*printfunc)(void *))
{
        if (NULL == tree)
                putstr("()");
        else
                help_prettyprint(tree, printfunc, 0);
}

static int unbox(void *e) { return ((struct entry *)e)->x; }

int cmp_entry(void *a, void *b)
{
        int A, B;
        if (NULL==a && NULL==b)
                return 0;
        else if (NULL==a)
                return 1;
        else if (NULL==b)
                return 1;
                
        A = unbox(a);
        B = unbox(b);
        if (A < B)
                return -1;
        else if (A == B)
                return 0;
        else
                return 1;
}

void destroy_entry(void *e) 
{ 
        if (NULL != e)
                free((struct entry *)e); 
} 

void print_entry(void *e) {
        if (NULL==e)
                printf("NIL");
        else
                printf("%d", unbox(e));
}

void printree(BT t)
{
        prettyprint(t, print_entry);
}

/* print a tree, then dispose of it. */
void testprint(const char *s, BT t)
{
        putstr(s);
        putstr(":\n->\t");
        printree(t);
        NEWLINE
        free_tree(t, destroy_entry);
}

struct entry *box(int x)
{
        struct entry *e = (struct entry*) malloc(sizeof(struct entry));
        e->x = x;
        return e;
}

BT create(int x) { return new_node(box(x)); }

void insert_int(int x, BT t) { insert(box(x), t, cmp_entry); }

void printbox(void *e) { printf("%d, ", unbox(e)); }

void traverse(BT tree) 
{ 
        inorder_traversal(tree, printbox); 
        NEWLINE
}

int main(int argc, char **argv)
{
        BT t = NULL;
        testprint("NULL", t);
        
        t = create(100);
        testprint("INT", t);
        
        t = newtree();
        testprint("Empty tree", t);
        
        t = newtree();
        insert_int(5, t);
        testprint("Insert 5 into empty tree", t);
        
        t = newtree();
        insert_int(5, t);
        insert_int(6, t);
        testprint("Insert 5, 6", t);
        
        t = newtree();
        char nums[100] = "100 79 52 96 60 78 94 62 65 84 71 12 66 39 86 19 75 4	35 45";
        char *tok;
        char delim[] = "\n\t ";
        tok = strtok(nums, delim);
        while (NULL != tok) {
                insert_int(atoi(tok), t);
                tok = strtok(NULL, delim);
        }
        traverse(t);
        testprint("Many insertions", t);
        return 0;
}

