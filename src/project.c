#include "project.h"

#include <dirent.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <yaml.h>

#include "util.h"

#define PROJECT_CONFIG_NAME "name"
#define PROJECT_CONFIG_DEPENDENCIES "dependencies"
#define PROJECT_CONFIG_BUILD_PATH "build-path"
#define PROJECT_CONFIG_A_SDK_PATH "android-sdk-path"
#define PROJECT_CONFIG_A_SDK_API_VERSION "android-sdk-api-version"
#define PROJECT_CONFIG_A_SDK_MIN_API_VERSION "android-sdk-min-api-version"
#define PROJECT_CONFIG_A_MANIFEST_PATH "android-manifest-path"
#define PROJECT_CONFIG_A_RES_PATH "android-res-path"
#define PROJECT_CONFIG_A_JAVA_PATH "android-java-path"

static char *expand(const char *x, const char *y)
{
        size_t zl = strlen(x) + strlen(y) + 2;
        char *z = malloc(zl);
        snprintf(z, zl, "%s/%s", x, y);
        return z;
}

int load_project(struct project_t *p, const char *path)
{
        int config_fd;
        char *config_content;
        size_t config_content_len;
        yaml_parser_t yp;
        yaml_event_t ye;
        char ckey[100] = { 0 }, buf[128] = { 0 };
        bool in_deps;

        snprintf(buf, sizeof buf, "%s/project.yml", path);
        if (!fexists(buf)) {
                memset(buf, 0, sizeof buf);
                snprintf(buf, sizeof buf, "%s/project.yaml", path);
        }
        if (!fexists(buf)) {
                printf("error: no config found at %s. you must provide it in a project.yaml or project.yml.\n",
                       path);
                return -1;
        }

        config_fd = open(buf, O_RDONLY);
        if (config_fd < 0) {
                snprintf(buf, sizeof buf, "error: failed to open your %s",
                         path);
                perror(buf);
                return -1;
        }

        config_content_len = lseek(config_fd, 0, SEEK_END);
        if (config_content_len < 0) {
                memset(buf, 0, sizeof buf);
                snprintf(buf, sizeof buf,
                         "error: failed to get size of your %s", path);
                perror(buf);
                close(config_fd);
                return -1;
        }
        lseek(config_fd, 0, SEEK_SET);

        config_content = malloc(config_content_len + 1);
        if (!config_content ||
            read(config_fd, config_content, config_content_len) < 0) {
                memset(buf, 0, sizeof buf);
                snprintf(buf, sizeof buf,
                         "error: failed to read config of your %s", path);
                perror(buf);
                free(config_content);
                close(config_fd);
                return -1;
        }
        config_content[config_content_len] = '\0';
        close(config_fd);

        p->deps.capacity = 1;
        p->deps.count = 0;
        p->deps.data = malloc(sizeof(char *) * p->deps.capacity);

        yaml_parser_initialize(&yp);
        yaml_parser_set_input_string(&yp, (unsigned char *)config_content,
                                     config_content_len);

        for (;;) {
                yaml_parser_parse(&yp, &ye);

                if (ye.type == YAML_STREAM_END_EVENT)
                        break;
                if (ye.type == YAML_SCALAR_EVENT) {
                        char *value = (char *)ye.data.scalar.value;
                        if (strlen(ckey) == 0) {
                                strcpy(ckey, value);
                                if (strcmp(ckey, PROJECT_CONFIG_DEPENDENCIES) ==
                                    0) {
                                        in_deps = true;
                                }
                        } else {
                                if (strcmp(ckey, PROJECT_CONFIG_NAME) == 0)
                                        p->name = strdup(value);
                                else if (strcmp(ckey,
                                                PROJECT_CONFIG_BUILD_PATH) == 0)
                                        p->build_path = expand(path, value);
                                else if (strcmp(ckey,
                                                PROJECT_CONFIG_A_SDK_PATH) == 0)
                                        p->android_sdk_path = strdup(value);
                                else if (strcmp(ckey,
                                                PROJECT_CONFIG_A_SDK_API_VERSION) ==
                                         0)
                                        p->android_sdk_api_version =
                                                atoi(value);
                                else if (strcmp(ckey,
                                                PROJECT_CONFIG_A_SDK_MIN_API_VERSION) ==
                                         0)
                                        p->android_sdk_min_api_version =
                                                atoi(value);
                                else if (strcmp(ckey,
                                                PROJECT_CONFIG_A_MANIFEST_PATH) ==
                                         0)
                                        p->android_manifest_path =
                                                expand(path, value);
                                else if (strcmp(ckey,
                                                PROJECT_CONFIG_A_RES_PATH) == 0)
                                        p->android_res_path =
                                                expand(path, value);
                                else if (strcmp(ckey,
                                                PROJECT_CONFIG_A_JAVA_PATH) ==
                                         0)
                                        p->android_java_path =
                                                expand(path, value);

                                ckey[0] = '\0';

                                if (in_deps && strlen(ckey) == 0) {
                                        if (p->deps.count >= p->deps.capacity) {
                                                p->deps.capacity *= 2;
                                                p->deps.data = realloc(
                                                        p->deps.data,
                                                        sizeof(char *) *
                                                                p->deps.capacity);
                                        }
                                        p->deps.data[p->deps.count++] =
                                                strdup(value);
                                }
                        }
                }

                if (ye.type == YAML_SEQUENCE_END_EVENT) {
                        in_deps = false;
                }

                yaml_event_delete(&ye);
        }

        yaml_parser_delete(&yp);
        free(config_content);

        if (!p->name) {
                puts("error: missing mandatory field in config: name");
                return -1;
        }

        if (!p->build_path) {
                p->build_path = strdup("./build");
                puts("-- no build-path provided. falling back to ./build");
        }

        if (!p->android_sdk_path) {
                char *sdkpath = getenv("ANDROID_SDK");
                if (!sdkpath)
                        sdkpath = getenv("ANDROID_HOME");
                if (!sdkpath) {
                        puts("error: env(ANDROID_SDK) nor env(ANDROID_HOME) are defined.");
                        return -1;
                }
                printf("-- no android-sdk-path provided. falling back to %s\n",
                       sdkpath);
                p->android_sdk_path = strdup(sdkpath);
        }

        if (p->android_sdk_api_version == 0) {
                puts("error: missing mandatory field in config: android-sdk-api-version");
                return -1;
        }

        if (p->android_sdk_min_api_version == 0) {
                puts("error: missing mandatory field in config: android-sdk-min-api-version");
                return -1;
        }

        if (!p->android_manifest_path) {
                puts("error: missing mandatory field in config: android-manifest-path");
                return -1;
        }

        if (!p->android_res_path) {
                puts("error: missing mandatory field in config: android-res-path");
                return -1;
        }

        if (!p->android_java_path) {
                puts("error: missing mandatory field in config: android-java-path");
                return -1;
        }

        if (!p->bins.aapt2) {
                char *aapt2 = which("aapt2");
                if (!aapt2) {
                        puts("error: aapt2 not found using which.");
                        return -1;
                }
                printf("-- no android-aapt2-bin provided. falling back to %s\n",
                       aapt2);
                p->bins.aapt2 = aapt2;
        }

        if (!p->bins.javac) {
                char *javac = which("javac");
                if (!javac) {
                        puts("error: javac not found using which.");
                        return -1;
                }
                printf("-- no android-javac-bin provided. falling back to %s\n",
                       javac);
                p->bins.javac = javac;
        }

        if (!p->bins.d8) {
                char *d8 = which("d8");
                if (!d8) {
                        puts("error: d8 not found using which.");
                        return -1;
                }
                printf("-- no android-d8-bin provided. falling back to %s\n",
                       d8);
                p->bins.d8 = d8;
        }
        return 0;
}

void destroy_project(struct project_t *p)
{
        size_t i;
        free(p->name);
        free(p->build_path);
        free(p->android_sdk_path);
        free(p->android_manifest_path);
        free(p->android_res_path);
        free(p->android_java_path);
        free(p->bins.javac);
        free(p->bins.aapt2);
        free(p->bins.d8);
        for (i = 0; i < p->deps.count; ++i) {
                free(p->deps.data[i]);
        }
        free(p->deps.data);
}

int build_res(struct project_t *p)
{
        char cmd[512] = { 0 };
        char buf[256] = { 0 };

        makedir(p->build_path);

        snprintf(cmd, sizeof cmd, "%s compile --dir %s -o %s/res.zip",
                 p->bins.aapt2, p->android_res_path, p->build_path);

        printf("-- Compiling resources with %s\n", p->bins.aapt2);
        if (system(cmd) != 0) {
                puts("error: failed to run aapt2 compile.");
                return -1;
        }

        snprintf(buf, sizeof buf, "%s/platforms/android-%d/android.jar",
                 p->android_sdk_path, p->android_sdk_api_version);
        if (!fexists(buf)) {
                printf("error: android.jar doesn't exists at %s\n", buf);
                return -1;
        }

        memset(cmd, 0, sizeof cmd);
        snprintf(cmd, sizeof cmd,
                 "%s link --auto-add-overlay "
                 "--manifest %s "
                 "-I %s "
                 "--min-sdk-version %d "
                 "--target-sdk-version %d "
                 "--java %s/java/generated/ "
                 "-R %s/res.zip "
                 "-o %s/unaligned.apk",
                 p->bins.aapt2, p->android_manifest_path, buf,
                 p->android_sdk_min_api_version, p->android_sdk_api_version,
                 p->build_path, p->build_path, p->build_path);

        printf("-- Linking resources with %s\n", p->bins.aapt2);
        if (system(cmd) != 0) {
                puts("error: failed to run aapt2 link.");
                return -1;
        }
        return 0;
}

struct java_files_t {
        size_t capacity;
        size_t len;
        char *files;
};

int get_java_files(struct java_files_t *jf, const char *d)
{
        struct DIR *dir = opendir(d);
        struct dirent *entry = NULL;

        if (!dir) {
                perror("error: can't open dir");
                return -1;
        }

        if (jf->capacity == 0)
                jf->capacity = 32;
        if (!jf->files) {
                jf->files = malloc(jf->capacity);
                if (!jf->files) {
                        perror("error: can't allocate memory for java_files");
                        return -1;
                }
                jf->files[0] = '\0';
                jf->len = 0;
        }

        while ((entry = readdir(dir)) != NULL) {
                char fph[512];
                if (strcmp(entry->d_name, ".") == 0 ||
                    strcmp(entry->d_name, "..") == 0)
                        continue;

                snprintf(fph, sizeof fph, "%s/%s", d, entry->d_name);

                if (entry->d_type == DT_REG) {
                        size_t n = jf->len + strlen(fph) + 2;
                        size_t len = strlen(fph);
                        if (n > jf->capacity) {
                                char *r;
                                jf->capacity = n * 2;
                                r = realloc(jf->files, jf->capacity);
                                if (!r) {
                                        perror("error: can't reallocate memory for java_files");
                                        free(jf->files);
                                        return -1;
                                }
                                jf->files = r;
                        }

                        memcpy(jf->files + jf->len, fph, len);
                        jf->len += len;
                        jf->files[jf->len++] = ' ';
                        jf->files[jf->len++] = '\0';
                } else if (entry->d_type == DT_DIR) {
                        get_java_files(jf, fph);
                }
        }
        closedir(dir);
        return 0;
}

int build_java(struct project_t *p)
{
        char outdir[512] = { 0 };
        char buf[512] = { 0 };
        char androidjar[512] = { 0 };
        char *cmd = NULL;
        size_t len = 0;
        struct java_files_t javafiles = { 0 };

        makedir(p->build_path);

        snprintf(outdir, sizeof outdir, "%s/java/classes/", p->build_path);
        makedir(outdir);

        snprintf(androidjar, sizeof androidjar,
                 "%s/platforms/android-%d/android.jar", p->android_sdk_path,
                 p->android_sdk_api_version);
        if (!fexists(androidjar)) {
                printf("error: android.jar doesn't exists at %s\n", buf);
                return -1;
        }

        snprintf(buf, sizeof buf,
                 "%s -source 17 "
                 "-target 17 "
                 "-nowarn "
                 "-proc:none "
                 "-classpath %s "
                 "-d %s ",
                 p->bins.javac, androidjar, outdir);

        len = strlen(buf);

        if (get_java_files(&javafiles, p->android_java_path) != 0) {
                printf("error: can't get java files at %s\n",
                       p->android_java_path);
                return -1;
        }

        len += javafiles.capacity;
        cmd = malloc(sizeof(char) * len);
        if (!cmd) {
                perror("error: failed to allocate javac cmd");
                free(javafiles.files);
                return -1;
        }

        strcpy(cmd, buf);
        strcat(cmd, javafiles.files);

        printf("-- Compiling java files with %s\n", p->bins.javac);
        if (system(cmd) != 0) {
                puts("error: failed to run javac compile.");
                free(javafiles.files);
                return -1;
        }
        free(javafiles.files);
        free(cmd);
        return 0;
}

int build_dex(struct project_t *p)
{
        (void)p;
        return 0;
}
