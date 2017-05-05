#include "sfish.h"

extern struct list_node *head;
extern bool yes_flag;
#define SUCCESS 0;
#define FAILURE -1;
extern bool spec_redir;

char delimeters[] = {' ', '<', '|', '>', '\0'};

void help(){
	ptr_help();
}

void cust_exit(){

	exit(EXIT_SUCCESS);
}

int change_dir(char* cmd){

	char *curr_dir = getcwd(NULL, 0);
	if(head->amt_args==1){
		setenv("OLDPWB", curr_dir, 1);
		char* home_dir = getenv("HOME");
		chdir(home_dir);
		return SUCCESS;
	}

	if(strcmp(head->argv[1], "-") == 0){

		char *prev_dir = getenv("OLDPWB");
		if(prev_dir == NULL){
			fprintf(stderr, "%s\n", "No previous working directory");
			return FAILURE;
		}
		else{
			chdir(prev_dir);
			setenv("OLDPWB", curr_dir, 1);
			return SUCCESS;
		}
	}

	else{
		setenv("OLDPWB", curr_dir, 1);
		int rsl = chdir(head->argv[1]);

		if(rsl == 0){
			return SUCCESS;
		}
		else{
			return FAILURE;
		}
	}
	return FAILURE;
}

void prt_wrk_dir(){

	fprintf(stdout, "%s\n", getcwd(NULL, 0));
}

void pars_cmd_2_struct(char* cmd){

	char *cpy_cmd = malloc(strlen(cmd)+1);
	strcpy(cpy_cmd, cmd);

	if(*cpy_cmd == ' '){

		while(*cpy_cmd == ' '){

			cpy_cmd++;
		}
	}

	struct list_node* new_node = malloc(sizeof(struct list_node));

	char * formated_str = format_string(cpy_cmd, new_node);
	/* get the length of exec name, and null terminate it */
	/* and add it the new ndoe */
	//struct list_node* new_node = malloc(sizeof(struct list_node));
	int length_of_exec = executable_length(formated_str);
	char *exec_name = extract_exec_name(formated_str, length_of_exec);
	if(strcmp(exec_name, "yes")==0){
		yes_flag = true;
	}
	add_exec_name(new_node, exec_name);
	new_node -> stdin = STDIN_FILENO;
	new_node -> stdout = STDOUT_FILENO;

	temp_add_args_2_ndoe(new_node ,formated_str);
	add_node_2_list(new_node);

}


char** parse_PATH(char *string){

	int PATH_length = strlen(string+1);
	char *PATH_dup = malloc(PATH_length+1);
	strcpy(PATH_dup, string);

	/* parsing PATH based on ':' */
	/* for iterating throug all paths */
	int amt_tks = cal_toks(PATH_dup) + 2;
	char ** list = malloc((amt_tks+1) * sizeof(char*) + 1);
	char *temp = strtok(PATH_dup, ":");

	/* get each string seperate by ':'*/
	int i;
	for(i = 0; i < amt_tks - 1; i++){

		list[i] = malloc(1024);
		strcpy(list[i], temp);
		temp = strtok(NULL, ":");
	}
	/* NULL terminate the array of strings */
	list[i] = malloc(1024);
	list[i] = NULL;

	return list;
}

int cal_toks(char *string){

	/* cal how many toks in the PATH, seperate by ':' */
	int counter = 0;
	int length = strlen(string);
	for(int i =0; i< length; i++){
		if((*string) == ':')
			counter++;
		string++;
	}
	return counter;
}

int executable_length(char *cmd_string){

	/* cal the length of exetable string */
	int length = 0;

	while(*cmd_string != '\0'){
		if((*cmd_string == ' ' || *cmd_string == '<' || *cmd_string == '|'
			|| *cmd_string == '>' || *cmd_string == '\0')&&length!=0){
			return length;
	}
	length++;
	cmd_string++;
}

return length;
}

void add_exec_name(struct list_node* node, char* exec_name){
	/* add exectutable program name to the node */
	node->exec_name = exec_name;
}

void add_node_2_list(struct list_node* node){

	/* node added to the end of doubly linked list */
	node->next = NULL;
	struct list_node* iterator = head;

	if(head==NULL){
		/* new head of empty list */
		head = node;
		node->prev = NULL;
	}
	else{
		/* attach to the end of the list */
		while(iterator->next != NULL){
			iterator = iterator-> next;
		}
		iterator->next = node;
		node->prev = iterator;
	}
}

char* extract_exec_name(char* cmd_string, int exec_legh){

	/* space for the exec name, and about to add it to the node*/
	char* exec_name = malloc(exec_legh + 1);
	char* iterator = cmd_string;
	bool off = false;

	int i, j = 0;
	for(i = 0; i < exec_legh; i++){
		if(iterator[j] == ' '){
			off = true;
			i--;
		}
		else{
			exec_name[i] = iterator[j];
		}
		j++;
	}
	/* null terminate exec name */
	if(off)
		exec_name[i-1] = '\0';
	else
		exec_name[i] = '\0';

	return exec_name;
}

char* extract_args_str(char* str, int exec_length){

	/* get to where the exec name end */
	//str +=exec_length;
	strtok(str, " ");
	char* args = strtok(NULL, " ");
	return args;
}

char** temp_add_args_2_ndoe(struct list_node* node, char* args){

	bool has_redir = false;
	char *cpy_args = (char*)malloc(strlen(args) + 1);
	strcpy(cpy_args, args);

	//char **redir_cmd = malloc(1024 * sizeof(char*));
	char **argv_ary = malloc(1024 * sizeof(char*));
	char **iterator = argv_ary;
	//char **redir_itor = redir_cmd;

	*iterator = strtok(cpy_args, " ");
	node -> amt_args = 0;
	node -> amt_args += 1;

	while(*iterator != NULL){
		iterator++;
		*iterator = malloc(strlen(args) + 1);
		*iterator = strtok(NULL, " ");
		if((*iterator != NULL) && (strcmp(*iterator, ">")==0 || strcmp(*iterator, "<")==0
			|| strcmp(*iterator, "2>") == 0 || strcmp(*iterator, "1>") == 0 || strcmp(*iterator, "&>") == 0
			|| strcmp(*iterator, ">>") == 0 || strcmp(*iterator, "<<") == 0)){

			has_redir = true;
		break;
	}
	if(*iterator != NULL)
		node -> amt_args += 1;
}

node -> argv = argv_ary;

if(has_redir == true){
	char *cpy2_args = (char*)malloc(strlen(args) + 1);
	strcpy(cpy2_args, args);
	char * redir_ary = malloc(strlen(args));

	if(strcmp(*iterator, ">")==0){

		strtok(cpy2_args, ">");
		char * temp = malloc(strlen(args));
		temp = strtok(NULL, ">");
		strcat(&redir_ary[0], ">");
		strcat(&redir_ary[0], temp);
	}
	else if(strcmp(*iterator, "<")==0){
		strtok(cpy2_args, "<");
		char * temp = malloc(strlen(args));
		temp = strtok(NULL, "<");
		strcat(&redir_ary[0], "<");
		strcat(&redir_ary[0], temp);
	}
	else if(strcmp(*iterator, "2>")==0){
		strtok(cpy2_args, "2>");
		char * temp = malloc(strlen(args));
		temp = strtok(NULL, "2>");
		strcat(&redir_ary[0], "2>");
		strcat(&redir_ary[0], temp);

	}
	node ->redir_cmd = redir_ary;
	*iterator = NULL;
}
else
	node -> redir_cmd = NULL;

return argv_ary;
}

void ptr_help(){

	printf("help  - Print a list of basic functionalities of the shell\n");
	printf("exit  - Exits the shell.\n");
	printf("pwd   - Changes the current working directory of the shell.\n");
	printf("cd    - Print current working directory\n");
	printf("ls    - list all files in the current working directory\n");
}

char* format_string(char *str, struct list_node* new_node){

	/* format input string, every word is seperate by one whitespace */
	int amount_toks = count_toks(str);
	int byts_4_whitespace = amount_toks*2;
	bool start = true;
	char pre_char = 0;
	char curr_char = *str;
	int max_str_length = strlen(str) + byts_4_whitespace;
	char* new_str = (char*)malloc(max_str_length + 1);
	char* new_str_iterator = new_str;
	char last_char = 0;

	while(max_str_length != 0 && curr_char!= '\0' && (pre_char!='\0' || start ==true)){

		if(special_toks(curr_char) == true || special_toks(pre_char) == true){

			/* current char is one of the special chars */
			if(special_toks(curr_char) == true){
				*new_str_iterator = pre_char;
				new_str_iterator++;
				if(pre_char != ' '){
					*new_str_iterator = ' ';
					new_str_iterator++;
				}
				if((pre_char > 47 && pre_char < 51)
					&& (curr_char == '&' || curr_char == '>' || curr_char == '<')){
					new_node->spec_redir = last_char;
				spec_redir = true;
			}
			*new_str_iterator = curr_char;
			new_str_iterator++;
		}
		else if(special_toks(pre_char) == true){

			if((last_char > 47 && last_char < 51)
				&& (pre_char == '&' || pre_char == '>' || pre_char == '<')){
				new_node->spec_redir = last_char;
			spec_redir = true;
		}

		if(curr_char!=' '){
			*new_str_iterator = ' ';

			new_str_iterator++;
		}
		*new_str_iterator = pre_char;
		new_str_iterator++;
		if(curr_char!= ' '){
			*new_str_iterator = ' ';
			new_str_iterator++;
		}
		*new_str_iterator = curr_char;
		new_str_iterator++;
	}
	else{
		if(curr_char!=' ' && pre_char!= ' '){
			*new_str_iterator = ' ';
			new_str_iterator++;
		}
		*new_str_iterator = pre_char;
		new_str_iterator++;
		*new_str_iterator = curr_char;
		new_str_iterator++;
	}

}

/* current_iterator != special chars */
else{
	if(pre_char != 0){
		if(special_toks(last_char) && curr_char != ' ' && pre_char!=' '){
			*new_str_iterator = ' ';
			new_str_iterator++;
		}
		*new_str_iterator = pre_char;
		new_str_iterator++;
		*(new_str_iterator) = curr_char;
		new_str_iterator++;
	}
	if(pre_char == 0){

		*new_str_iterator = curr_char;
		new_str_iterator++;
	}
}
last_char = curr_char;
str++;
pre_char = *str;
str++;
curr_char = *(str);
max_str_length--;
start = false;
}

if(curr_char == '\0'){
	*new_str_iterator = pre_char;
	new_str_iterator++;
}
*new_str_iterator = '\0';
return new_str;

}

int count_toks(char *str){

	/* cal number of tokens, if none then reurn 0 */
	char *iterator = str;
	int counter = 0;
	while(*iterator != '\0'){

		if(*iterator == '>' || *iterator == '<' || *iterator == '|'){
			counter++;
		}
		iterator++;
	}
	return counter;
}

bool special_toks(char input){
	if(input == '>' || input == '<' || input == '|')
		return true;
	else
		return false;
}

bool srch_and_exec(char* exec_name, char** args){

	char *PATH = getenv("PATH");
	int counter = 0;
	char **PATH_ary = parse_PATH(PATH);
	char* str = NULL;
	bool valid_exec = false;

	while(PATH_ary[counter]!=NULL){

		str = concate_PATH(PATH_ary[counter], exec_name);
		if(vali_file(str)){
			spec_redir = false;
			valid_exec = true;
			execv(str, args);
		}
		counter++;
	}

	return valid_exec;
}

bool vali_file(char* exec_name){

	struct stat file_struct;

	if(stat(exec_name, &file_struct) == 0){
		return true;
	}
	return false;
}

char* concate_PATH(char* PATH, char* exec_name){

	int length = strlen(PATH) + strlen(exec_name);

	char *str = malloc(length + 2);

	strcpy(str, PATH);
	strcat(str, "/");
	strcat(str, exec_name);

	return str;
}


char* prmpt_name(){
	char* curr_dir = getcwd(NULL, 0);
	char* pre = "MattYang: ";
	char *name = malloc(strlen(curr_dir)+strlen(pre)+4);
	memset(name, 0, strlen(curr_dir)+strlen(pre)+4);
	strcat(name, pre);
	strcat(name, curr_dir);
	strcat(name, " $ ");

	return name;
}

int count_pipe_char(char* str){

	char *iteor = str;
	int count = 0;
	while(*iteor!='\0'){

		if(*iteor == '|'){
			count++;
		}
		iteor++;
	}
	return count;
}

void chg_file_desptor(char *input){

	char *in_file = calloc(strlen(input), sizeof(char*));
	char *out_file = calloc(strlen(input), sizeof(char*));
	int count = 0;
	while(*input != '\0'){

		if(*input == '<'){
			input = input+2;
			while(*input != ' ' && *input!= '\0'){
				in_file[count] = *input;
				input++;
				count++;
			}
			in_file[count] = '\0';
			count = 0;
		}
		else if(*input == '>'){
			input = input+2;
			while(*input != ' ' && *input!= '\0'){
				out_file[count] = *input;
				input++;
				count++;
			}
			out_file[count] = '\0';
			count = 0;
		}
		input++;
	}

	if(*in_file != '\0'){
		int in_file_desc = open(in_file, O_RDONLY);
		dup2(in_file_desc, STDIN_FILENO);
	}
	if(*out_file!= '\0'){

		if(spec_redir == false){
			int out_file_desc = open(out_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(out_file_desc, STDOUT_FILENO);
		}
		else{
			int out_file_desc = open(out_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

			if(head->spec_redir == '1'){
				dup2(out_file_desc, STDOUT_FILENO);
			}
			else if(head->spec_redir == '2'){
				dup2(out_file_desc, STDERR_FILENO);
			}
		}
	}
}

bool piping_progs(struct list_node* node){

	struct list_node *cpy_node = node;
	int file_descri[2];
	file_descri[0]=0;
	file_descri[1]=0;

	while(cpy_node ->next != NULL){

		if(pipe(file_descri)==-1){
			return false;
		}
		cpy_node->stdout = file_descri[1];
		cpy_node->next->stdin = file_descri[0];
		cpy_node = cpy_node->next;
	}
	return true;
}

