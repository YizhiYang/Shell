#ifndef SFISH_H
#define SFISH_H
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#endif

void help();
void cust_exit();
void prt_wrk_dir();
void ptr_help();

struct list_node {
	char* exec_name;
	char** argv;
	char * redir_cmd;
	int argc;
	int amt_args;
	int stdin;
	int stdout;
	int stderr;
	char spec_redir;
	struct list_node* next;
	struct list_node* prev;
};

struct list_node *head;
bool print_flag;
volatile sig_atomic_t got_interrupt;
int sec;
bool yes_flag;
bool spec_redir;

int change_dir(char* cmd);
int cal_toks(char *string);
char** parse_PATH(char *string);
int executable_length(char *);
void pars_cmd_2_struct(char* cmd);
char* extract_exec_name(char* cmd_string, int exec_legh);
void add_exec_name(struct list_node* node, char* exec_name);
char* extract_args_str(char* str, int exec_length);
void add_node_2_list(struct list_node* node);
char** temp_add_args_2_ndoe(struct list_node* node, char* args);
int count_toks(char *str);
char* format_string(char *str, struct list_node* new_node);
bool special_toks(char input);
char* concate_PATH(char* PATH, char* exec_name);
bool vali_file(char* exec_name);
bool srch_and_exec(char* exec_name, char** args);
char* prmpt_name();
int count_pipe_char(char* str);
void chg_file_desptor(char *input);
bool piping_progs(struct list_node* node);
void sig_hder(int signal_num);
void alarm_handler();
void kill_handler();
void siguros_handler();
void spec_redir_char(char ** argv, struct list_node* new_node);
void chg_spec_redir_file_description();