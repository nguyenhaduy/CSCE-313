#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <vector>
using namespace std;


#define LSH_RL_BUFSIZE 1024

int cd_command(vector<string> args);
int help_command(vector<string> args);
int exit_command(vector<string> args);

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
int cd_command(vector<string> args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int command_help(vector<string> args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
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
int command_exit(vector<string> args)
{
  return 0;
}

int execute(vector<string> args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}



// Grabs all tokens
char* split(char* s) {
  int count = 0;
  int bufsize = TOK_BUFSIZE, position = 0;
  char **tokens = (char**)malloc(bufsize * sizeof(char*));
  char *token, **tokens_copy;
  string temp;
  vector<string> new_input;
  while(count < s.size()) {
    temp = "";
    while(count < s.size() && s[count] != ' ' && s[count] != '\"' && s[count] != '<' && s[count] != '>' && s[count] != '|' && s[count] != '&') {
      temp += s[count];
      count++;
    }
    if(s[count] == '\"') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      temp = s.substr(count, s.substr(count+1).find("\"")+2);
      count = count + 2 + (s.substr(count+1)).find("\"");
    } else if(s[count] == ' ') {
      count++;
    } else if(s[count] == '<') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      count++;
      temp = "<";
    } else if(s[count] == '>') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      count++;
      temp = ">";
    } else if(s[count] == '|') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      count++;
      temp = "|";
    } else if(s[count] == '&') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      count++;
      temp = "&";
    }
    if(temp != "") {
      new_input.push_back(temp);
    }
  }
  return new_input;
}

void lsh_loop(void)
{
  string line;
  vector<string> args;
  int status;

  do {
    printf("> ");
    getline(cin, line);
    args = split_line(line);
    status = execute(args);

    free(line);
    free(args);
  } while (status);
}

int main() {
  string input = "";
  while(true) {
    cout << "$ ";
    getline(cin, input);
    vector<string> temp = split(input);
    for(int i = 0; i < temp.size(); i++) {
      cout << "Item => " << temp[i] << endl;
    }
  }
  return 0;
}
