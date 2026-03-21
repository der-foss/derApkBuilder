#ifndef dab_project
#define dab_project

#include <stddef.h>

struct project_config {
        char *name;
        char *build_path;
        char *android_sdk_path;
        char *android_manifest_path;
        char *android_res_path;
        char *android_java_path;
        int android_sdk_api_version;

        struct {
               char **data;
               size_t count;
               size_t capacity; 
        } deps;
};

int load_project_config(struct project_config *, const char *, size_t);
void destroy_project_config(struct project_config *);

#endif
