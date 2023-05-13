#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/syslimits.h>
#include <sys/stat.h>
#include <sys/types.h>
/*
 * Queries the current working directory
 * */
char *get_cwd() {
  char *cwd = (char*) malloc(PATH_MAX);
  getcwd(cwd, PATH_MAX);
  return cwd;
}

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
  char *cwd = get_cwd();
  const char *prompt = "$> ";

  if(cwd == NULL) {
    printf("current working directory is null\n");
    exit(EXIT_FAILURE);
  }

  for(;;) {

    printf("%s %s", cwd, prompt);

    size_t len = 0;
    char *cmd = (char*) malloc(1);
    char c;
    while((c = getchar()) != '\n' && c != EOF) {
      char *t = realloc(cmd, len + 1);
      cmd = t;
      cmd[len] = c;
      len += 1;
    }

    if(len == 0) {
      free(cmd);
      continue;
    }

    char **tokens = tokenize_cmd(cmd);

    if(strcmp(tokens[0], "pwd") == 0) {
      printf("%s\n", cwd);
      free(cmd);
    } else if(strcmp(tokens[0], "exit") == 0) {
      free(cmd);
      break;
    } else if(strcmp(tokens[0], "cd") == 0) {
      char *folder = tokens[1];
      
      if(chdir(folder) != 0) {
        printf("cd: path %s does not exist\n", folder);
      } else {
        free(cwd);
        cwd = get_cwd();
      }
      free(cmd);
    } else {
      launch_cmd(tokens);
    }
  }

  printf("Bye\n");

  free((void*) cwd);
  exit(EXIT_SUCCESS);
}
