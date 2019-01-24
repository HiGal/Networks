#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <signal.h>

#define BUF_LEN 25
#define READ_END 0
#define WRITE_END 1

struct Node {
    int value;
    struct Node *next;
};

struct Stack {
    int size;
    struct Node *head;
};

struct Stack *stack = NULL;

void create() {
    stack = (struct Stack *) malloc(sizeof(struct Stack));
    stack->head = NULL;
    stack->size = 0;
    printf("Stack was successfully created\n");
}

int empty() {
    return stack->head == NULL;
}

int peek() {
    if (stack == NULL) {
        fprintf(stderr, "Stack has not been created yet");
    }
    if (empty()) {
        fprintf(stderr, "Stack is empty");
    }
    return stack->head->value;
}

void push(int data) {
    struct Stack *val = stack;
    if (stack == NULL) {
        create();
    }

    if (stack->head == NULL) {
        struct Node *node = (struct Node *) malloc(sizeof(struct Node));
        node->value = data;
        stack->head = node;
        stack->size++;
    } else {

        struct Node *node = (struct Node *) malloc(sizeof(struct Node));
        node->value = data;
        node->next = stack->head;
        stack->head = node;
        stack->size++;
    }
    val = stack;
}

void pop() {
    if (stack == NULL) {
        fprintf(stderr, "Stack has not been created yet");
    }
    if (empty()) {
        fprintf(stderr, "Stack is empty");
    }

    struct Node *tmp = stack->head;
    stack->head = stack->head->next;
    stack->size--;
    free(tmp);
}


void display() {
    struct Node *cursor = stack->head;
    while (cursor != NULL) {
        if (cursor->next == NULL) {
            printf("%d", cursor->value);
        } else {
            printf("%d -> ", cursor->value);
        }
        cursor = cursor->next;
    }
    printf("\n");
}


void stack_size() {
    printf("Stack size is %d\n", stack->size);
}



int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
    }

    pid_t pid = fork();

    pid_t parent_id = 0;

    if (pid < 0) {
        fprintf(stderr, "Fork failed");
        return 1;
    }

    if (pid > 0) {
        close(fd[WRITE_END]);
        char buffer[BUF_LEN + 1] = "";
        do {
            read(fd[READ_END], buffer, sizeof buffer);
            // kill(getppid(), SIGSTOP);
            if (strcmp(buffer, "push\n") == 0 ) {
                printf("Enter value to push on stack: ");
                int n;
                scanf("%d",&n);
                push(n);
                strcpy(buffer,"");
            }
            else if(strcmp(buffer,"display\n") == 0)
                display();
            else if(strcmp(buffer,"pop\n") == 0)
                pop();
            else if(strcmp(buffer,"peek\n") == 0)
                printf("%d\n",peek());
            else if (strcmp(buffer,"empty\n") == 0) {
                if (empty())
                    printf("Stack is empty");
                else
                    printf("Stack is not empty");
            }
            else if(strcmp(buffer,"size\n") == 0)
                stack_size();
            kill(parent_id,SIGCONT);

            // printf("%d\n",strcmp(buffer, "exit\n"));
        } while (strcmp(buffer, "exit\n") != 0);
        close(fd[READ_END]);
    } else {
        close(fd[READ_END]);
        char buffer[BUF_LEN + 1] = "";
        bool end_call = false;
        do {
            printf("You entered: ");
            fgets(buffer, sizeof buffer , stdin);
                if (!strcmp(buffer, "exit\n")) {
                    end_call = true;
                }
                write(fd[WRITE_END], buffer, strlen(buffer) + 1);
                parent_id = getpid();
                kill(parent_id,SIGSTOP);
        } while (!end_call);
        close(fd[WRITE_END]);
    }
    return 0;
}