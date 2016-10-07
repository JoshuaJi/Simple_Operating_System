//
//  main.c
//  OS_assignment_1
//
//  Created by Xu Ji on 2016-09-26.
//  Copyright © 2016 Xu Ji. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMD_LENGTH 20
#define NUMBER_OF_HISTORY 10
//
// This code is given for illustration purposes. You need not include or follow this
// strictly. Feel free to writer better or bug free code. This example code block does not
// worry about deallocating memory. You need to ensure memory is allocated and deallocated
// properly so that your shell works without leaking memory.
//
int getcmd(char *prompt, char *args[], int *background)
{
    int length, i = 0;
    char *token, *loc;
    char *line = NULL;
    size_t linecap = 0;

    printf("%s", prompt);
    length = (int)getline(&line, &linecap, stdin);
    if (length <= 0) {
        exit(-1);
    }
    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else
        *background = 0;
    while ((token = strsep(&line, " \t\n")) != NULL) {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
            args[i++] = token;
    }
    return i;
}

int main(void)
{
    char *history_cmd[NUMBER_OF_HISTORY];
    char *args[CMD_LENGTH];
    int history_index = 0;
    int bg;

    for (int i = 0; i < NUMBER_OF_HISTORY; i++){
        history_cmd[i] = NULL;
    }

    while(1) {
        bg = 0;
        int cnt = getcmd("\n>> ", args, &bg);

        args[cnt] = NULL;
        pid_t pid = fork();
        if (pid == 0){
            if (strcmp(*args, "history") == 0){
                for (int i=0; i < NUMBER_OF_HISTORY; i++){
                    printf("%d %s\n", history_index-10+i, history_cmd[i]);
                }
            }else{
                execvp(args[0], args);
            }
            exit(0);
        }else{
            if (!bg){
                int status;
                waitpid(pid, &status, 0);
            }else{
                int status;
                waitpid(pid, &status, WNOHANG);
            }

            free(history_cmd[history_index%NUMBER_OF_HISTORY]);
            strcpy(history_cmd[history_index++], *args);
            if (history_index >= NUMBER_OF_HISTORY)
                history_index = 0;
        }
    }
}
