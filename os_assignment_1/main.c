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


struct history_cmd
{
    int index;
    char *cmd[CMD_LENGTH];
};

struct history_cmd *create_history_cmd(int index, char * args[]){
    struct history_cmd *temp_history_cmd = malloc(sizeof(struct history_cmd));
    temp_history_cmd->index = index;
    int i = 0;
    while(*(args+i)){
        temp_history_cmd->cmd[i] = *(args+i);
        i++;
    }
    return temp_history_cmd;
};

void free_history_cmd(struct history_cmd *temp_cmd){
    free(temp_cmd);
}

void print_cmd(char ** args){
    int i = 0; 
    while (*(args+i)){
        printf("%s ", *(args+i));
        i++;
    }
    printf("\n");
}

void print_history(struct history_cmd * temp_cmd){
    printf("index: %d", temp_cmd->index);
    //print_cmd(temp_cmd->cmd);
}

void print_history_list(struct history_cmd ** history_cmds, int history_index){
    for (int i=0; i < NUMBER_OF_HISTORY; i++){
        if (history_cmds[(history_index-1+i)%10]){
            struct history_cmd *temp_cmd = history_cmds[(history_index-1+i)%10];
            printf("%d", temp_cmd->index);
            //print_cmd(temp_cmd->cmd);
            free_history_cmd(temp_cmd);
        }
    }
}

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
        struct history_cmd **history_cmds = malloc(sizeof(struct history_cmd)*NUMBER_OF_HISTORY);
        char *args[CMD_LENGTH];
        int bg;
        int history_index = 0;

        while(1) {
            bg = 0;
            int cnt = getcmd("\n>> ", args, &bg);

            args[cnt] = NULL;
            //print_cmd(args);
            pid_t pid = fork();
            if (pid == 0){
                if (strcmp(*args, "history") == 0){
                    for (int i=0; i < NUMBER_OF_HISTORY; i++){
                        if (history_cmds[(history_index-1+i)%10]){
                            struct history_cmd *temp_cmd = history_cmds[(history_index-1+i)%10];
                            printf("%d ", temp_cmd->index);
                            print_cmd(temp_cmd->cmd);
                            free_history_cmd(temp_cmd);
                        }
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

                if (history_cmds[history_index%10]){
                    free_history_cmd(history_cmds[history_index%10]);
                }
                struct history_cmd *new_history_cmd = create_history_cmd(history_index, args);
                history_cmds[history_index%10] = new_history_cmd;
                history_index++;
            }
        }
    }
