#ifndef dab_util
#define dab_util

#include <stdbool.h>
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

static __attribute__((unused))
bool fexists(const char *s)
{
        return access(s, F_OK) == 0;
}

static __attribute__((unused))
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
                char *p =strdup(buf);
                p[strcspn(p, "\n")] = 0;
                return p;
        }
        return NULL;
}

static __attribute__((unused))
bool makedir(const char *s) {
        if (!fexists(s)) {
                pmkdir(s);
                return true;
        }
        return false;
}

#endif
