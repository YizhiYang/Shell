#include "sfish.h"

extern struct list_node *head;
extern bool print_flag;
extern int sec;
extern bool yes_flag;
extern bool spec_redir;


int stdout_desc = 1;
int stdin_desc = 1;
volatile sig_atomic_t got_interrupt = 0;


int main(int argc, char const *argv[], char* envp[]){

    rl_catch_signals = 0;
    /* register signals */
    char *cmd;
    head = NULL;
    print_flag = false;
    yes_flag = false;
    spec_redir = false;

    struct sigaction act;
    memset(&act,0, sizeof(act));
    act.sa_sigaction = kill_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &act, NULL);

    struct sigaction alarm_struc;
    memset(&alarm_struc,0, sizeof(alarm_struc));
    alarm_struc.sa_handler = alarm_handler;
    sigaction(SIGALRM, &alarm_struc, NULL);

    struct sigaction sigusr2_stru;
    memset(&sigusr2_stru,0, sizeof(sigusr2_stru));
    sigusr2_stru.sa_handler = siguros_handler;
    sigaction(SIGUSR2, &sigusr2_stru, NULL);

    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);
    /* end of registering signals */

    /* main method is a parent process that never die,
    and keep prompting by this while loop */
    char* name = prmpt_name();
    while((cmd = readline(name)) != NULL) {
        spec_redir = false;

        char *cpy_cmd = malloc(strlen(cmd) + 1);
        strcpy(cpy_cmd, cmd);

        int count = count_pipe_char(cmd);

        /* parser command line, and add exectuable prog name,
        argc to new node */
        /* no pipelining is needed */
        if(count == 0){
            pars_cmd_2_struct(cmd);
        }
        /* pipelining is detacted */
        else{
            char **prog_ary = malloc(count * sizeof(char*) + 1);
            memset(prog_ary, 0, count);
            prog_ary[0] = malloc(strlen(cmd) + 1);
            prog_ary[0] = strtok(cpy_cmd, "|");
            int count = 0;
            while(prog_ary[count] != NULL){
                count++;
                prog_ary[count] = malloc(strlen(cmd) + 1);
                prog_ary[count] = strtok(NULL, "|");
            }
            prog_ary[count] = NULL;

            count = 0;
            while(prog_ary[count] != NULL){
                pars_cmd_2_struct(prog_ary[count]);
                count++;
            }
        }

        piping_progs(head);
        while(head != NULL){

            if (strcmp(cmd, "exit")==0){
                cust_exit();
                break;
            }
            if(strcmp(cmd, "help")==0){

                help();
            }

            else if(strcmp(head->exec_name, "cd")==0){

                if(change_dir(cmd) == -1)
                    fprintf(stdout, "No such file or directory\n");
            }

            else if(strcmp(head->exec_name, "alarm")==0){
                //signal(SIGALRM, alarm_handler);

                if(head -> argv[1] == NULL){
                    printf("Invalid argument\n");
                }

                else{
                    int counter = 0;
                    int invalid = false;
                    while(head->argv[1][counter]!= '\0'){

                        if(head->argv[1][counter] >57 || head->argv[1][counter] < 48){
                            invalid = true;
                        }
                        counter ++;
                    }
                    if(invalid == false){
                        sec = atoi(head->argv[1]);
                        alarm(sec);
                        break;
                    }
                    else{
                        printf("Invalid argument\n");
                    }
                }
            }

            else {
                char *sys_PATH = getenv("PATH");
                parse_PATH(sys_PATH);
                pid_t pid = fork();
                if(pid == 0){
                /* child provess */

                    dup2(head->stdin, STDIN_FILENO);
                    dup2(head->stdout, STDOUT_FILENO);
                    if(head->redir_cmd !=NULL){
                        chg_file_desptor(head->redir_cmd);
                    }

                    if(strcmp(head->exec_name, "pwd")==0){
                        prt_wrk_dir();
                    }
                    else if(*(head->exec_name) == '/'){
                        if(execv(head->exec_name, head->argv) == -1){

                            printf("%s: command not found\n", head->exec_name);
                        }

                    }
                    else{
                        bool rsl = srch_and_exec(head->exec_name, head->argv);
                        if(rsl == false){
                            printf("%s: command not found\n", head->exec_name);
                        }
                    }
                    exit(EXIT_SUCCESS);
                }

                else{
                 /* parent process */
                    if (head->stdin > 2)
                        close(head->stdin);
                    if (head->stdout > 2)
                        close(head->stdout);
                    int child_exit_status;

                    if(head->next == NULL || yes_flag == false){
                        waitpid(pid, &child_exit_status, 0);
                    }
                }
            }
            head = head->next;
        }
        /* Free the linked list */
        struct list_node* temp;
        while((temp = head) != NULL){
           head = head -> next;
           free(temp);
       }
       head = NULL;
       name = prmpt_name();
   }

    free(cmd);

   return EXIT_SUCCESS;
}

