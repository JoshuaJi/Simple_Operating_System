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


// Declare a node datatype to be implemented as linked list
typedef struct node{
    int pid;
    char *cmd;
    struct node *next;
}node_t;

// Declare a history command object
struct history_cmd
{
    int index;
    char *cmd[CMD_LENGTH];
    int is_bg;
};

// Create a history command object by using the history_cmd struct
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

// Pass in a stirng array and print out with format
void print_cmd(char ** args){
    int i = 0; 
    while (*(args+i)){
        printf("%s ", *(args+i));
        i++;
    }
}

// Passing in the history list and print out history in order
void print_history(struct history_cmd ** history_cmds, int history_index){
    printf("\n");
    printf("====== HISTORY ======\n");
    for (int i=0; i < NUMBER_OF_HISTORY; i++){
        struct history_cmd *temp_cmd;
        if ((temp_cmd = history_cmds[(history_index+i)%10])){
            printf("%3d\t", temp_cmd->index);
            print_cmd(temp_cmd->cmd);
            if (temp_cmd->is_bg)
                printf("&");
            printf("\n");
        }
    }
}

// If pipe is involved, execuate pipe in a new child process
void exec_pipe(char* args[], int pipe_index, int fd[], int *status, int wait_pid_opt){
    if (pipe_index != -1){
        pid_t pid_pipe = fork();
        if (pid_pipe == 0){
            close(fd[1]);
            dup2(fd[0], 0);
            char *args_pipe[CMD_LENGTH - pipe_index];
            int i = 0;
            while (i < (CMD_LENGTH - pipe_index - 1)){
                args_pipe[i] = args[pipe_index + 1 + i];
                i++;
            }
            args_pipe[i] = NULL;
            execvp(args_pipe[0], args_pipe);
            exit(0);
        }else{
            close(fd[0]);
            close(fd[1]);
            waitpid(pid_pipe, status, wait_pid_opt);
        }
    }
}

// Add a command to the jobs list with the corresponding pid number 
void add_job(node_t *job_list, int pid, char *args[]){

    node_t *new_node = malloc(sizeof(node_t));
    new_node->pid = pid;
    new_node->cmd = strdup(*args);
    node_t *current = job_list;
    while(current->next != NULL)
        current = current->next;
    current->next = new_node;
}

// Initiate a history object with the given commands,
// and add the history object in the history list
void add_history(char* args[], struct history_cmd **history_cmds, int *history_index, int bg){
    if (history_cmds[*history_index%10]){
        free(history_cmds[*history_index%10]);
    }
    struct history_cmd *new_history_cmd;
    new_history_cmd = create_history_cmd(*history_index+1, args, bg);
    history_cmds[*history_index%10] = new_history_cmd;
    *history_index = *history_index + 1;
}

// Print jobs list with format
void print_jobs(node_t *job_list){
    if (job_list->next){
        printf("\n");
        printf("====== JOBS ======\n");
    }else{
        printf("Job list is empty\n");
        return;
    }

    node_t *current = job_list->next;
    while(current != NULL){
        printf("%d\t%s\n", current->pid, current->cmd);
        current = current->next;
    }
}

// Bring process to foreground with the corresponding pid 
void fg(node_t *job_list, char* args[]){

    if (*(args+1) == NULL){
        printf("Please pass in the pid number with fg\n");
        return;
    }
    int pid = atoi(*(args+1));
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

// Change current directory
void change_directory(char *args[]){
    if(*(args+1)){
        int status = chdir(*(args+1));
        if (status != 0)
            perror("Invalid directory\n");
    }else{
        chdir("/");
    }
}

// Print out the corrent path
void PWD(){
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL){
        fprintf(stdout, "%s\n", cwd);
    }
    else{
        perror("Unable to get current director");
    }
}

// Given a string array, return the char position in this string array
// If there is none, return -1
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

// Fork a child process and execuate the given command in the child process
void exec_in_fork(char* args[], node_t* job_list, int redir_index, int pipe_index, int fd[], int bg){
    pid_t pid = fork();
    if (pid == 0){

        // If there is a redirection character, close stdout, open a file and write the ourput to the file
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
        waitpid(pid, &status, wait_pid_opt);
        exec_pipe(args, pipe_index, fd, &status, wait_pid_opt);
    }
}

// Method given by prof which retrieves the user command
int getcmd(char *prompt, char *args[], int *background, int *is_history){
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

// Phase, analyze, process and exec the command
void exec_cmd(char* args[], int cnt,int bg, int is_history, int *history_index, struct history_cmd **history_cmds, node_t *job_list){
    if (cnt == 0)
        return;
    args[cnt] = NULL;

    // If user type "!" with a number, replace the args by the corresponding command in the history list
    if (is_history){
        int history_num = atoi(args[0]);
        if (history_num > *history_index+1){
            printf("Couldn't find corresponding history number\n");
            return;
        }
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

    // Look up special char in the command, if yes, replace the special char by null
    if ((pipe_index = find_char(args, '|')) != -1)
        args[pipe_index] = NULL;
    
    if ((redir_index = find_char(args, '>')) != -1)
        args[redir_index] = NULL;
    
    // Match commands with custom function and execuate
    if (strcmp(*args, "history") == 0)
        print_history(history_cmds, *history_index);
    else if (strcmp(*args, "jobs") == 0)
        print_jobs(job_list);
    else if (strcmp(*args, "fg") == 0)
        fg(job_list, args);
    else if (strcmp(*args, "cd") == 0)
        change_directory(args);
    else if (strcmp(*args, "pwd") == 0)
        PWD();
    else if (strcmp(*args, "exit") == 0)
        exit(0);
    else
        exec_in_fork(args, job_list, redir_index, pipe_index, fd, bg);
    
    // If the special has been replaced by null, bring it back
    if (pipe_index != -1)
        args[pipe_index] = "|";

    if (redir_index != -1)
        args[redir_index] = ">";
    
    // Log command into the history list
    add_history(args, history_cmds, history_index, bg);
}

int main(void){

    // Initiate a empty history command list and a empty job list when program starts 
    struct history_cmd **history_cmds = malloc(sizeof(struct history_cmd)*NUMBER_OF_HISTORY);
    node_t *job_list = malloc(sizeof(node_t));
    char *args[CMD_LENGTH];
    int bg, is_history;
    int history_index = 0;

    // Looping until Ctrl-D
    while(1) {
        bg = 0;

        // Reterieve command from stdin
        int cnt = getcmd("\n>> ", args, &bg, &is_history);

        // Execuate the command
        exec_cmd(args, cnt, bg, is_history, &history_index, history_cmds, job_list);
    }
}
