#ifndef dab_util
#define dab_util

#include <stdbool.h>
#include <stddef.h>

struct file_list_t {
        char **data;
        size_t count;
        size_t capacity;
};

bool fexists(const char *);
char *which(const char *);
bool makedir(const char *);

int list_java_files(struct file_list_t *, const char *);
void clean_file_list(struct file_list_t *);

#endif
