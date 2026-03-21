#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "project.h"

static int run(const char *path)
{
        struct project_t pc;
        memset(&pc, 0, sizeof pc);
        if (load_project(&pc, path) != 0) {
                printf("Failed to load project.\n");
                return -1;
        }

        build_res(&pc);
        build_java(&pc);
        build_dex(&pc);

        destroy_project(&pc);
        return 0;
}

int main(int argc, char **argv)
{
        const char *action;
        if (argc < 2) {
                printf("error: please provide an action, see \"%s help\".\n",
                       argv[0]);
                return -1;
        }

        action = argv[1];
        if (strcmp(action, "help") == 0) {
        } else if (strcmp(action, "run") == 0) {
                char *path;
                
                if (argc < 3) {
                        puts("error: you must provide a project path");
                        return -1;
                }
                path = argv[2];

                if (!path) {
                        printf("error: somehow project config file path is null\n");
                        return -1;
                }

                if (run(path) != 0) {
                        return -1;
                }
        }

        return 0;
}
