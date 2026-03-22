#ifndef dab_file_list
#define dab_file_list

#include <stddef.h>

struct file_list_t {
  char **data;
  size_t count;
  size_t capacity;
};

int list_java_files(struct file_list_t *fl, const char *ph);
void clean_file_list(struct file_list_t *fl);

#endif
