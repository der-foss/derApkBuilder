#ifndef dab_project
#define dab_project

#include <stddef.h>

struct project_t {
        char *name;
        char *build_path;
        char *android_sdk_path;
        char *android_manifest_path;
        char *android_res_path;
        char *android_java_path;
        int android_sdk_api_version;
        int android_sdk_min_api_version;

        struct {
                char **data;
                size_t count;
                size_t capacity;
        } deps;

        struct {
                char *aapt2;
                char *javac;
                char *d8;
        } bins;
};

int load_project(struct project_t *, const char *);
void destroy_project(struct project_t *);

int build_res(struct project_t *);
int build_java(struct project_t *);
int build_dex(struct project_t *);

#endif
