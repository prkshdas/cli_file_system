#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// file node
typedef struct File
{
    char name[20];
    char data[120];
    struct File *next;
} File;

// dir node
typedef struct Dir
{
    char name[20];

    struct Dir *parent;
    struct Dir *child;
    struct Dir *sibling;

    File *fileList;
} Dir;

// command queue
#define QSIZE 10
typedef struct
{
    char cmd[10][64];
    int front, rear;
} CmdQueue;

// path stack
#define MAX_DEPTH 20
typedef struct
{
    Dir *stack[20];
    int top;
} PathStack;

// create dir
Dir *createDir(char *name, Dir *parent)
{
    Dir *d = malloc(sizeof(Dir));

    strcpy(d->name, name);

    d->parent = parent;
    d->child = NULL;
    d->sibling = NULL;
    d->fileList = NULL;

    return d;
}

// create file
void createFile(Dir *cwd, char *name)
{
    File *f = malloc(sizeof(File));

    strcpy(f->name, name);
    strcpy(f->data, "");

    f->next = cwd->fileList;
    cwd->fileList = f;
}

// list files
void listFiles(Dir *cwd)
{
    File *t = cwd->fileList;

    while (t)
    {
        printf("%s\n", t->name);
        t = t->next;
    }
}

// mkdir
void mkdir(Dir *cwd, char *name)
{
    Dir *d = createDir(name, cwd);
    d->sibling = cwd->child;
    cwd->child = d;
}

// ls
void ls(Dir *cwd)
{
    Dir *d = cwd->child;

    while (d)
    {
        printf("[DIR] %s\n", d->name);
        d = d->sibling;
    }

    listFiles(cwd);
}

// stack
void push(PathStack *s, Dir *d)
{
    s->stack[++s->top] = d;
}

Dir *pop(PathStack *s)
{
    if (s->top < 0)
        return NULL;

    return s->stack[s->top--];
}

// cd
Dir *cd(Dir *cwd, PathStack *st, char *name)
{
    if (strcmp(name, "..") == 0)
    {
        if (cwd->parent)
            return cwd->parent;
        return cwd;
    }

    Dir *t = cwd->child;

    while (t)
    {
        if (strcmp(t->name, name) == 0)
        {
            push(st, cwd);
            return t;
        }
        t = t->sibling;
    }

    printf("No such dir\n");
    return cwd;
}

// queue
void enqueue(CmdQueue *q, char *c)
{
    strcpy(q->cmd[q->rear], c);
    q->rear = (q->rear + 1) % QSIZE;
}

char *dequeue(CmdQueue *q)
{
    char *c = q->cmd[q->front];
    q->front = (q->front + 1) % QSIZE;
    return c;
}

// cli engine
void process(char *cmd, Dir **cwd, PathStack *st)
{
    if (strncmp(cmd, "mkdir", 5) == 0)
        mkdir(*cwd, cmd + 6);

    else if (strncmp(cmd, "touch", 5) == 0)
        createFile(*cwd, cmd + 6);

    else if (strcmp(cmd, "ls") == 0)
        ls(*cwd);

    else if (strncmp(cmd, "cd", 2) == 0)
        *cwd = cd(*cwd, st, cmd + 3);
}

int main()
{
    Dir *root = createDir("root", NULL);
    Dir *cwd = root;

    PathStack st = {.top = -1};

    char input[64];

    while (1)
    {
        printf("CLI> ");

        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        process(input, &cwd, &st);
    }

    return 0;
}