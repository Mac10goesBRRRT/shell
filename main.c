#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syslimits.h>

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
  const char* path = get_path();
  const char* cwd = get_cwd();

  if(path == NULL) {
    printf("path is null\n");
    exit(EXIT_FAILURE);
  }

  if(cwd == NULL) {
    printf("current working directory is null\n");
    exit(EXIT_FAILURE);
  }

  printf("Path: %s\n", path);
  printf("CWD: %s\n", cwd);

  free((void*) path);
  free((void*) cwd);
  exit(EXIT_SUCCESS);
}
