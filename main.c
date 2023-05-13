#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syslimits.h>
#include <sys/stat.h>

/*
 * Queries the path variable and returns a copy of it
 * */
char *get_path() {
  char *envvar = "PATH";
  int path_len = strlen(getenv(envvar));

  char *path = (char*) malloc(path_len);
  strncpy(path, getenv(envvar), path_len);
  return path;
}

/*
 * Queries the current working directory
 * */
char *get_cwd() {
  char *cwd = (char*) malloc(PATH_MAX);
  getcwd(cwd, PATH_MAX);
  return cwd;
}

int main(int argc, char **argv) {
  const char *path = get_path();
  char *cwd = get_cwd();
  const char *prompt = "$> ";


  if(path == NULL) {
    printf("path is null\n");
    exit(EXIT_FAILURE);
  }

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

    char *token = strtok(cmd, " ");

    if(strcmp(token, "pwd") == 0) {
      printf("%s\n", cwd);
    } else if(strcmp(token, "exit") == 0) {
      free(cmd);
      break;
    } else if(strcmp(token, "cd") == 0) {
      char *folder = strtok(NULL, "");
      
      if(chdir(folder) != 0) {
        printf("cd: path %s does not exist\n", folder);
      } else {
        free(cwd);
        cwd = get_cwd();
      }
      free(cmd);
    } else {
      free(cmd);
      continue;
    }
  }

  printf("Bye\n");

  free((void*) path);
  free((void*) cwd);
  exit(EXIT_SUCCESS);
}
