#ifndef dab_project
#define dab_project

#include <stddef.h>

struct project_t {
        char *name;
        char *build_path;

        struct {
                char *sdk_path;
                char *manifest_path;
                char *res_path;
                char *java_path;
                char *keystore_path;

                char *keystore_alias;
                char *keystore_store_pass;
                char *keystore_key_pass;

                int sdk_api_version;
                int sdk_min_api_version;
        } android;

        struct {
                char **data;
                size_t count;
                size_t capacity;
        } deps;

        struct {
                char *aapt2;
                char *javac;
                char *jar;
                char *d8;
                char *zip;
                char *zipalign;
                char *jarsigner;
        } bins;
};

int load_project(struct project_t *, const char *);
void destroy_project(struct project_t *);

int build_res(struct project_t *);
int build_java(struct project_t *);
int build_dex(struct project_t *);
int make_apk(struct project_t *);

#endif
