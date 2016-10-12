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
#define NUMBER_OF_JOBS 100
//
// This code is given for illustration purposes. You need not include or follow this
// strictly. Feel free to writer better or bug free code. This example code block does not
// worry about deallocating memory. You need to ensure memory is allocated and deallocated
// properly so that your shell works without leaking memory.
//


typedef struct node{
    int pid;
    char *cmd;
    struct node *next;
}node_t;

struct history_cmd
{
    int index;
    char *cmd[CMD_LENGTH];
    int is_bg;
};

struct history_cmd *create_history_cmd(int index, char * args[], int is_bg){
    struct history_cmd *temp_history_cmd = malloc(sizeof(struct history_cmd));
    temp_history_cmd->index = index;
    temp_history_cmd->is_bg = is_bg;
    int i = 0;
    while(*(args+i)){
        temp_history_cmd->cmd[i] = strdup(*(args+i));
        i++;
    }
    return temp_history_cmd;
};

void print_cmd(char ** args){
    int i = 0; 
    while (*(args+i)){
        printf("%s ", *(args+i));
        i++;
    }
}

void print_history(struct history_cmd ** history_cmds, int history_index){
    for (int i=0; i < NUMBER_OF_HISTORY; i++){
        struct history_cmd *temp_cmd;
        if ((temp_cmd = history_cmds[(history_index+i)%10])){
            printf("%d ", temp_cmd->index);
            print_cmd(temp_cmd->cmd);
            if (temp_cmd->is_bg)
                printf("&");
            printf("\n");
        }
    }
}

void add_job(node_t *job_list, int pid, char *args[]){

    node_t *new_node = malloc(sizeof(node_t));
    new_node->pid = pid;
    new_node->cmd = strdup(*args);
    node_t *current = job_list;
    while(current->next != NULL)
        current = current->next;
    current->next = new_node;
}

void print_jobs(node_t *job_list){
    node_t *current = job_list->next;
    while(current != NULL){
        printf("%d\t%s\n", current->pid, current->cmd);
        current = current->next;
    }
}

void fg(node_t *job_list, int pid){
    waitpid(pid, NULL, 0);
    node_t *current = job_list;
    while(current->next!= NULL){
        if (current->next->pid == pid){
            node_t *temp = current->next;
            current->next = current->next->next;
            free(temp);
            return;
        }
        current = current->next;

    }
}

void change_directory(char *args[]){
    if(*(args+1)){
        int status = chdir(*(args+1));
        if (status != 0)
            perror("Invalid directory\n");
    }else{
        chdir("/");
    }
}

void PWD(){
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL){
        fprintf(stdout, "%s\n", cwd);
    }
    else{
        perror("Unable to get current director");
    }
}

int find_char(char *args[], char c){
    int i = 0;
    while(*(args+i)){
        if(strchr(*(args+i), c)){
            return i;
        }
        i++;
    }
    return -1;
}

int getcmd(char *prompt, char *args[], int *background, int *is_history)
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

    if (((loc = index(line, '!')) != NULL) && (line[0] == '!')) {
        *is_history = 1;
        *loc = ' ';
    } else
    *is_history = 0;

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
        node_t *job_list = malloc(sizeof(node_t));
        char *args[CMD_LENGTH];
        int bg, is_history;
        int history_index = 0;

        while(1) {
            bg = 0;
            int cnt = getcmd("\n>> ", args, &bg, &is_history);

            if (cnt == 0)
                continue;
            args[cnt] = NULL;

            if (is_history){
                int history_num = atoi(args[0]);
                struct history_cmd *temp_cmd = history_cmds[history_num-1];
                int i = 0;
                while(temp_cmd->cmd[i]){
                    args[i] = temp_cmd->cmd[i];
                    i++;
                }
                args[i] = NULL;
                bg = temp_cmd->is_bg;
            }

            int redir_index, pipe_index;
            int fd[2];
            pipe(fd);
            if ((pipe_index = find_char(args, '|')) != -1){
                args[pipe_index] = NULL;
                printf("piping index: %d\n", pipe_index);
            }
            if ((redir_index = find_char(args, '>')) != -1){
                args[redir_index] = NULL;
                printf("redirection index: %d\n", redir_index);
            }

            if (strcmp(*args, "history") == 0){
                print_history(history_cmds, history_index);
            }else if (strcmp(*args, "jobs") == 0){
                print_jobs(job_list);
            }else if (strcmp(*args, "fg") == 0){
                fg(job_list, atoi(*(args+1)));
            }else if (strcmp(*args, "cd") == 0){
                change_directory(args);
            }else if (strcmp(*args, "pwd") == 0){
                PWD();
            }else if (strcmp(*args, "exit") == 0){
                exit(0);
            }else{
                pid_t pid = fork();
                if (pid == 0){
                    if (redir_index != -1)
                    {
                        fclose(stdout);
                        FILE *fp = fopen(args[redir_index+1], "w+");
                    }
                    if (pipe_index != -1)
                    {
                        close(fd[0]);
                        dup2(fd[1], 1);
                    }
                    execvp(args[0], args);
                    exit(0);
                }else{
                    int wait_pid_opt = bg? 1 : 0;
                    int status;
                    if (bg){
                        add_job(job_list, pid, args);
                    }
                    // waitpid(pid, &status, wait_pid_opt && (pipe_index == -1));
                    waitpid(pid, &status, wait_pid_opt);
                    if (pipe_index != -1)
                    {
                        pid_t pid_pipe = fork();
                        if (pid_pipe == 0)
                        {
                            close(fd[1]);
                            dup2(fd[0], 0);
                            char *args_pipe[CMD_LENGTH - pipe_index];
                            int i = 0;
                            while (i < (CMD_LENGTH - pipe_index - 1))
                            {
                                args_pipe[i] = args[pipe_index + 1 + i];
                                i++;
                            }
                            args_pipe[i] = NULL;
                            execvp(args_pipe[0], args_pipe);
                            exit(0);
                        }else{
                            close(fd[0]);
                            close(fd[1]);
                            waitpid(pid_pipe, &status, wait_pid_opt);
                        }
                    }
                }
            }

            if (pipe_index != -1){
                args[pipe_index] = "|";
            }

            if (redir_index != -1){
                args[redir_index] = ">";
            }

            if (history_cmds[history_index%10]){
                free(history_cmds[history_index%10]);
            }
            struct history_cmd *new_history_cmd;
            new_history_cmd = create_history_cmd(history_index+1, args, bg);
            history_cmds[history_index%10] = new_history_cmd;
            history_index++;
        }
    }
