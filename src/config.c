#include "config.h"

#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <yaml.h>

#define CONFIG_P_NAME "name"
#define CONFIG_P_DEPENDENCIES "dependencies"
#define CONFIG_P_BUILD_PATH "build-path"
#define CONFIG_P_A_SDK_PATH "android-sdk-path"
#define CONFIG_P_A_SDK_API_VERSUON "android-sdk-api-version"
#define CONFIG_P_A_MANIFEST_PATH "android-manifest-path"
#define CONFIG_P_A_RES_PATH "android-res-path"
#define CONFIG_P_A_JAVA_PATH "android-java-path"

int load_project_config(struct project_config *pc, const char *content, size_t content_len)
{
        yaml_parser_t yp;
        yaml_event_t ye;
        char ckey[100];
        bool in_deps;

        memset(ckey, 0, sizeof ckey);

        pc->deps.capacity = 1;
        pc->deps.count = 0;
        pc->deps.data = malloc(sizeof(char *) * pc->deps.capacity);

        yaml_parser_initialize(&yp);
        yaml_parser_set_input_string(&yp, (unsigned char *)content,
                                     content_len);

        for (;;) {
                yaml_parser_parse(&yp, &ye);

                if (ye.type == YAML_STREAM_END_EVENT)
                        break;
                if (ye.type == YAML_SCALAR_EVENT) {
                        char *value = (char *)ye.data.scalar.value;
                        if (strlen(ckey) == 0) {
                                strcpy(ckey, value);
                                if (strcmp(ckey, CONFIG_P_DEPENDENCIES) == 0) {
                                        in_deps = true;
                                }
                        } else {
                                if (strcmp(ckey, CONFIG_P_NAME) == 0)
                                        pc->name = strdup(value);
                                else if (strcmp(ckey, CONFIG_P_BUILD_PATH) == 0)
                                        pc->build_path = strdup(value);
                                else if (strcmp(ckey, CONFIG_P_A_SDK_PATH) == 0)
                                        pc->android_sdk_path = strdup(value);
                                else if (strcmp(ckey,
                                                CONFIG_P_A_SDK_API_VERSUON) == 0)
                                        pc->android_sdk_api_version =
                                                atoi(value);
                                else if (strcmp(ckey,
                                                CONFIG_P_A_MANIFEST_PATH) == 0)
                                        pc->android_manifest_path =
                                                strdup(value);
                                else if (strcmp(ckey, CONFIG_P_A_RES_PATH) == 0)
                                        pc->android_res_path = strdup(value);
                                else if (strcmp(ckey, CONFIG_P_A_JAVA_PATH) == 0)
                                        pc->android_java_path = strdup(value);

                                ckey[0] = '\0';

                                if (in_deps && strlen(ckey) == 0) {
                                        if (pc->deps.count >=
                                            pc->deps.capacity) {
                                                pc->deps.capacity *= 2;
                                                pc->deps.data = realloc(
                                                        pc->deps.data,
                                                        sizeof(char *) *
                                                                pc->deps.capacity);
                                        }
                                        pc->deps.data[pc->deps.count++] =
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
        return 0;
}

void destroy_project_config(struct project_config *pc)
{
        size_t i = 0;
        for (; i < pc->deps.count; ++i) {
                free(pc->deps.data[i]);
        }
        free(pc->deps.data);
}
