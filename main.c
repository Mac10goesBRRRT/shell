#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syslimits.h>
#include <signal.h>
/*
 * Output a Help Message when Ctrl + C is pressed
 */
void handle_ctrlc(int _unused) {
  printf("\nuse the command \"exit\" to quit this program.\n");
}

/*
 * Queries the current working directory
 * */
char *get_cwd() {
  char *cwd = (char*) malloc(PATH_MAX);
  getcwd(cwd, PATH_MAX);
  return cwd;
}
/*
 * Searches for the program in PATH and executes it as child process.
 * Blocks the parent process until the command in the child process terminates
 */
void launch_cmd(char **args) {
  pid_t pid;
  int status;

  pid = fork();
  if(pid == 0) {
    if(execvp(args[0], args) == -1) {
      printf("error launching command process\n");
    }
    exit(EXIT_FAILURE);
  } else if(pid < 0) {
    printf("error forking\n");
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
}

/*
 * Splits the input from the user into tokens separated by " "
 */
char **tokenize_cmd(char* cmd) {
  int buffer_size = 64;
  int pos = 0;
  char **tokens = malloc(buffer_size * sizeof(char*));
  char *token, **tokens_backup;

  if(!tokens) {
    printf("error allocating\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(cmd, " ");
  while(token != NULL) {
    tokens[pos] = token;
    pos += 1;

    if(pos >= buffer_size) {
      buffer_size += buffer_size;
      tokens_backup = tokens;
      tokens = realloc(tokens, buffer_size * sizeof(char*));
      if(!tokens) {
        free(tokens_backup);
        printf("error growing the command array");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, " ");
  }
  tokens[pos] = NULL;
  return tokens;
}

int main(int argc, char **argv) {
  // Get current working dir to display to the user
  char *cwd = get_cwd();
  // The prompt string to display
  const char *prompt = "$> ";

  if(cwd == NULL) {
    printf("current working directory is null\n");
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, handle_ctrlc);

  // Loop endlessly until the user inputs an "exit" command
  for(;;) {
    // Prints the prompt
    printf("%s %s", cwd, prompt);

    // Reads the user command from the console (inefficient).
    size_t len = 0;
    char *cmd = (char*) malloc(1);
    char c;
    while((c = getchar()) != '\n' && c != EOF) {
      char *t = realloc(cmd, len + 1);
      cmd = t;
      cmd[len] = c;
      len += 1;
    }

    // Restart the loop if the user provided no input
    if(len == 0) {
      free(cmd);
      continue;
    }

    char **tokens = tokenize_cmd(cmd);

    if(strcmp(tokens[0], "pwd") == 0) {
      // Just print the current working directory
      printf("%s\n", cwd);
      free(cmd);
      free(tokens);
    } else if(strcmp(tokens[0], "exit") == 0) {
      // Free resources and then break from the loop
      free(cmd);
      free(tokens);
      break;
    } else if(strcmp(tokens[0], "cd") == 0) {
      char *folder = tokens[1];
      // Change dir or output an error message
      if(chdir(folder) != 0) {
        printf("cd: path %s does not exist\n", folder);
      } else {
        free(cwd);
        cwd = get_cwd();
      }
      free(cmd);
      free(tokens);
    } else {
      // Try to find the command in PATH and execute it
      launch_cmd(tokens);
      free(cmd);
      free(tokens);
    }
  }

  printf("Bye\n");

  free((void*) cwd);
  exit(EXIT_SUCCESS);
}
