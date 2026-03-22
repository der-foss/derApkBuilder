#include "util.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#ifndef strdup
#define strdup _strdup
#endif
#ifndef pmkdir
#define pmkdir(s) _mkdir(s)
#endif
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#ifndef pmkdir
#define pmkdir(s) mkdir(s, 0700)
#endif
#endif

bool fexists(const char *s)
{
        return access(s, F_OK) == 0;
}

char *which(const char *cmd)
{
        FILE *f;
        char buf[256];
        snprintf(buf, sizeof buf, "which %s", cmd);

        f = popen(buf, "r");
        if (!f) {
                return NULL;
        }

        memset(buf, 0, sizeof buf);
        if (fgets(buf, sizeof buf, f) != NULL) {
                char *p = strdup(buf);
                p[strcspn(p, "\n")] = 0;
                return p;
        }
        return NULL;
}

bool makedir(const char *s)
{
        if (!fexists(s)) {
                pmkdir(s);
                return true;
        }
        return false;
}

int list_java_files(struct file_list_t *fl, const char *ph)
{
        struct DIR *dir = NULL;
        struct dirent *ent = NULL;

        dir = opendir(ph);
        if (!dir)
                return -1;

        if (fl->capacity == 0)
                fl->capacity = 1;
        if (!fl->data) {
                fl->data = malloc(sizeof(char *) * fl->capacity);
                if (!fl->data) {
                        perror("failed to alloc data");
                        return -1;
                }
        }

        while ((ent = readdir(dir)) != NULL) {
                char fph[512];
                if (strcmp(ent->d_name, ".") == 0 ||
                    strcmp(ent->d_name, "..") == 0)
                        continue;
                snprintf(fph, sizeof fph, "%s/%s", ph, ent->d_name);

                if (ent->d_type == DT_DIR) {
                        list_java_files(fl, fph);
                } else if (ent->d_type == DT_REG &&
                           strstr(ent->d_name, ".java")) {
                        if (fl->count >= fl->capacity) {
                                fl->capacity *= 2;
                                fl->data =
                                        realloc(fl->data,
                                                sizeof(char *) * fl->capacity);
                        }
                        fl->data[fl->count++] = strdup(fph);
                }
        }

        closedir(dir);
        return 0;
}

void clean_file_list(struct file_list_t *fl)
{
        size_t i = 0;
        for (; i < fl->count; i++) {
                free(fl->data[i]);
        }
        free(fl->data);
        fl->capacity = 0;
        fl->count = 0;
}
