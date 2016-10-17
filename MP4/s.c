/* Compile with: g++ -Wall –Werror -o shell shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
 
#include <sys/types.h>
#include <sys/wait.h>
 
/* The array below will hold the arguments: args[0] is the command. */
static char** args;
pid_t pid;
int command_pipe[2];
 
#define READ  0
#define WRITE 1
 
/*
 * Handle commands separatly
 * input: return value from previous command (useful for pipe file descriptor)
 * first: 1 if first command in pipe-sequence (no input from previous pipe)
 * last: 1 if last command in pipe-sequence (no input from previous pipe)
 *
 * EXAMPLE: If you type "ls | grep shell | wc" in your shell:
 *    fd1 = command(0, 1, 0), with args[0] = "ls"
 *    fd2 = command(fd1, 0, 0), with args[0] = "grep" and args[1] = "shell"
 *    fd3 = command(fd2, 0, 1), with args[0] = "wc"
 *
 * So if 'command' returns a file descriptor, the next 'command' has this
 * descriptor as its 'input'.
 */

/*
  Function Declarations for builtin shell commands:
 */
int command_cd(char **args);
int command_help(char **args);
int command_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &command_cd,
  &command_help,
  &command_exit
};

int num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int command_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Error: Expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("Wrong directory");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int command_help(char **args)
{
  int i;
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int command_exit(char **args)
{
  exit(0);
  return 0;
}

#define RL_BUFSIZE 1024

static int command(int input, int first, int last)
{
  int pipefd[2];
 
  /* Invoke pipe */
  pipe( pipefd ); 
  pid = fork();
 
  /*
   SCHEME:
    STDIN --> O --> O --> O --> STDOUT
  */
 
  if (pid == 0) {
    if (first == 1 && last == 0 && input == 0) {
      // First command
      dup2( pipefd[WRITE], STDOUT_FILENO );
    } else if (first == 0 && last == 0 && input != 0) {
      // Middle command
      dup2(input, STDIN_FILENO);
      dup2(pipefd[WRITE], STDOUT_FILENO);
    } else {
      // Last command
      dup2( input, STDIN_FILENO );
    }
 
    if (execvp( args[0], args) == -1)
      _exit(EXIT_FAILURE); // If child fails
  }
 
  if (input != 0) 
    close(input);
 
  // Nothing more needs to be written
  close(pipefd[WRITE]);
 
  // If it's the last command, nothing more needs to be read
  if (last == 1)
    close(pipefd[READ]);
 
  return pipefd[READ];
}
 
/* Final cleanup, 'wait' for processes to terminate.
 *  n : Number of times 'command' was invoked.
 */
static void cleanup(int n)
{
  int i;
  for (i = 0; i < n; ++i) 
    wait(NULL); 
}
 
static int run(char* cmd, int input, int first, int last);
static char line[1024];
static int n = 0; /* number of calls to 'command' */
 
int main()
{
  printf("SIMPLE SHELL: Type 'exit' or send EOF to exit.\n");
  while (1) {
    /* Print the command prompt */
    printf("$> ");
    fflush(NULL);
 
    /* Read a command line */
    if (!fgets(line, 1024, stdin)) 
      return 0;
 
    int input = 0;
    int first = 1;
 
    char* cmd = line;
    char* next = strchr(cmd, '|'); /* Find first '|' */
 
    while (next != NULL) {
      /* 'next' points to '|' */
      *next = '\0';
      input = run(cmd, input, first, 0);
 
      cmd = next + 1;
      next = strchr(cmd, '|'); /* Find next '|' */
      first = 0;
    }
    input = run(cmd, input, first, 1);
    cleanup(n);
    n = 0;
  }
  return 0;
}
 
static void split(char* cmd);
 
static int run(char* cmd, int input, int first, int last)
{
  split(cmd);
  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (int i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return command(input, first, last);;
}
 
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

static void split(char* cmd)
{
  int bufsize = TOK_BUFSIZE, position = 0;
  char **tokens = (char**)malloc(bufsize * sizeof(char*));
  char *token, **tokens_copy;

  if (!tokens) {
    fprintf(stderr, "Allocation error\n");
    exit(EXIT_FAILURE);
  }

  //use strtok to tokenize the input line
  token = strtok(cmd, " \n\t\r\a\"");
  while (token != NULL) {
    // printf( " %s\n", token );
    if (token[0] == '\'' || token[0] == '\"')
    {
      char sign = token[0];
      char* temp = (char*)malloc(strlen(cmd));
      for (int i = 0; i < (sizeof(token)-1); ++i){
        char* copy = token +1;
        strcpy(temp, "\"");
        strcat(temp, copy);

        // *(temp + i) = *(token + i + 1);
      }

      strcat(temp, " ");
      token = strtok(NULL, "\"\'");
      
      strcat(temp, token);
      strcat(temp, "\"");
      printf( "temp = %s\n", temp );

      tokens[position] = temp;
      position++;
    }
    else{
      tokens[position] = token;
      position++;
    }

    //If char array token get bigger than BUFSIZE
    if (position >= bufsize) {
      bufsize += TOK_BUFSIZE;
      tokens_copy = tokens;
      tokens = (char**)realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        free(tokens_copy);
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOK_DELIM);
  }

  tokens[position] = NULL;
  args = tokens;
}
