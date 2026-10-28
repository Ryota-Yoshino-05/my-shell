// POSIXの機能を利用する
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROMPT      "myshell> "
#define MAX_ARGS    128
#define DELIMITERS  " \t\r\n"

// 入力を分割する
// staticはこのファイルからのみ使用可能
static int tokenize(char *line, char **argv, int max_args) {
    int argc = 0;
    char *saveptr = NULL;
    char *tok = strtok_r(line, DELIMITERS, &saveptr);

    while (tok != NULL && argc < max_args - 1) {
        argv[argc++] = tok;
        tok = strtok_r(NULL, DELIMITERS, &saveptr);
    }
    argv[argc] = NULL;
    return argc;
}

static int run_command(char **argv) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("myshell: fork");
        return 1;
    }

    // 子プロセス
    if (pid == 0) {
        execvp(argv[0], argv);
        perror(argv[0]);
        _exit(127);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("myshel: waitpid");
        return 1;
    }
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }
    return 1;
}

int main(void) {
    char *line = NULL;
    size_t cap = 0;
    char *argv[MAX_ARGS];
    int last_status = 0;

    for (;;) {
        printf(PROMPT);
        fflush(stdout);

        ssize_t n = getline(&line, &cap, stdin);
        if (n < 0) {
            putchar('\n');
            break;
        }

        int argc = tokenize(line, argv, MAX_ARGS);
        if (argc == 0) {
            continue;
        }
        
        if (strcmp(argv[0], "exit") == 0) {
            break;
        }

        last_status = run_command(argv);
    }

    free(line);
    return last_status;
}