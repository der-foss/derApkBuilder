#include "project.h"

#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static bool exists(const char *s)
{
        return access(s, F_OK) == 0;
}

static int run(const char *c, size_t clen)
{
        struct project_config pc;
        if (load_project_config(&pc, c, clen) != 0) {
                printf("Failed to load project config.\n");
                return -1;
        }

        printf("name: %s\n", pc.name);
        printf("build-path: %s\n", pc.build_path);
        printf("android-sdk-path: %s\n", pc.android_sdk_path);
        printf("android-sdk-api-version: %d\n", pc.android_sdk_api_version);
        printf("android-manifest-path: %s\n", pc.android_manifest_path);
        printf("android-res-path: %s\n", pc.android_res_path);
        printf("android-java-path: %s\n", pc.android_java_path);

        destroy_project_config(&pc);
        
        return 0;
}

int main(int argc, char **argv)
{
        const char *action;
        if (argc < 2) {
                printf("error: please provide an action, see \"%s help\".\n", argv[0]);
                return -1;
        }

        action = argv[1];
        if (strcmp(action, "help") == 0) {
        } else if (strcmp(action, "run") == 0) {
                char *path;
                int ffd;
                size_t flen;
                char *fbuf;
                static char buf[128];
                bool yaml = false, yml = false;
                if (argc < 3 && !((yaml = exists("project.yaml")) ||
                                  (yml = exists("project.yml")))) {
                        printf("error: you should provide a project config by cli or have it in your PWD as project.yaml.");
                        return -1;
                }

                if (yaml)
                        path = "project.yaml";
                else if (yml)
                        path = "project.yml";
                else
                        path = argv[2];

                if (!path) {
                        printf("error: somehow project config file path is null\n");
                        return -1;
                }

                ffd = open(path, O_RDONLY);
                if (ffd < 0) {
                        snprintf(buf, sizeof(buf), "error: failed to open your %s",
                                 path);
                        perror(buf);
                        return -1;
                }

                flen = lseek(ffd, 0, SEEK_END);
                if (flen < 0) {
                        memset(buf, 0, sizeof(buf));
                        snprintf(buf, sizeof(buf),
                                 "error: failed to get size of your %s", path);
                        perror(buf);
                        close(ffd);
                        return -1;
                }
                lseek(ffd, 0, SEEK_SET);

                fbuf = malloc(flen + 1);
                if (!fbuf || read(ffd, fbuf, flen) < 0) {
                        memset(buf, 0, sizeof(buf));
                        snprintf(buf, sizeof(buf),
                                 "error: failed to read content of your %s", path);
                        perror(buf);
                        free(fbuf);
                        close(ffd);
                        return -1;
                }
                fbuf[flen] = '\0';

                if (run(fbuf, flen - 1)) {
                        free(fbuf);
                        close(ffd);
                        return -1;
                }

                free(fbuf);
                close(ffd);
        }

        return 0;
}
