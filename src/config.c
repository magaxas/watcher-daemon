#include "main.h"

char *create_default_config(char *default_home)
{
    cJSON *watchers = NULL;
    cJSON *dirs_to_watch = NULL;
    cJSON *json = cJSON_CreateObject();
    size_t index = 0;

    char *home_dir = NULL;
    if (default_home == NULL) {
        home_dir = get_home_dir();
    } else {
        home_dir = calloc(strlen(default_home) + 2, 1);
        if (home_dir == NULL) goto end;
        strncat(home_dir, default_home, strlen(default_home));
    }

    if (home_dir == NULL) goto end;
    else if (home_dir[strlen(home_dir) - 1] == '/') {
        memmove(
            &home_dir[strlen(home_dir) - 1],
            &home_dir[strlen(home_dir)], 1
        );
    }
    strcat(home_dir, "/");

    const char *names[] = {
        "audio",
        "video",
        "photo",
        "document"
    };

    const char *types[4][3] = {
        {"mp3", "flac", "wav"},
        {"mkv", "avi", "mp4"},
        {"svg", "jpeg", "png"},
        {"docx", "txt", "pdf"}
    };

    const char *dirs[] = {
        "Downloads/",
        "Music/",
        "Pictures/",
        "Videos/",
        "Documents/"
    };

    watchers = cJSON_AddArrayToObject(json, "watchers");
    if (watchers == NULL) {
        goto end;
    }

    for (index = 0; index < 5; index++) {
        char path[PATH_MAX] = "";
        strncat(path, home_dir, strlen(home_dir));
        strncat(path, dirs[index], strlen(dirs[index]));
        recursive_mkdir(path);

        if (index == 0) {
            dirs_to_watch = cJSON_AddArrayToObject(json, "dirs_to_watch");
            if (dirs_to_watch == NULL) {
                goto end;
            } else {
                cJSON_AddItemToArray(dirs_to_watch, cJSON_CreateString(path));
            }
        } else {
            cJSON *w = cJSON_CreateObject();

            if (cJSON_AddStringToObject(w, "name", names[index - 1]) == NULL) {
                goto end;
            }
            if (cJSON_AddStringToObject(w, "dir_to_move", path) == NULL) {
                goto end;
            }
            if (cJSON_AddBoolToObject(w, "enabled", true) == NULL) {
                goto end;
            }

            cJSON *file_types = NULL;
            file_types = cJSON_AddArrayToObject(w, "file_types");

            if (file_types == NULL) {
                goto end;
            } else {
                for (int i = 0; i < 3; i++) {
                    cJSON_AddItemToArray(
                        file_types,
                        cJSON_CreateString(types[index - 1][i])
                    );
                }
            }

            cJSON_AddItemToArray(watchers, w);
        }
    }

    char *string = NULL;
    string = cJSON_Print(json);
    if (string == NULL) {
        logger(ERROR, "Failed to print default config!");
    }

end:
    FREE(home_dir);
    cJSON_Delete(json);
    return string;
}

char *read_file(char *file_name)
{
    long lSize;
    char *buffer = NULL;

    FILE *fp = fopen(file_name, "r");
    if (!fp) {
        logger(WARNING, "Cannot find config file: %s", file_name);
        goto end;
    }

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    buffer = calloc(1, lSize + 1);

    if (!buffer) {
        logger(ERROR, "Memory allocation failed while reading file: %s", file_name);
    } else if (fread(buffer, lSize, 1, fp) != 1) {
        logger(ERROR, "Cannot open config file: %s", file_name);
    }

    fclose(fp);
end:
    return buffer;
}

int init_dirs_to_watch(config *cnf, cJSON *json)
{
    const cJSON *dir_to_watch = NULL;
    const cJSON *dirs_to_watch = NULL;

    dirs_to_watch = cJSON_GetObjectItemCaseSensitive(json, "dirs_to_watch");
    cnf->dirs_lenght = cJSON_GetArraySize(dirs_to_watch);
    cnf->dirs_to_watch = (char **) malloc(cnf->dirs_lenght * sizeof(char *));

    if (cnf->dirs_to_watch == NULL) {
        logger(ERROR, "Error occured while allocating \"dirs_to_watch\"");
        return 1;
    }

    int i = 0;
    cJSON_ArrayForEach(dir_to_watch, dirs_to_watch) {
        if (cJSON_IsString(dir_to_watch)) {
            cnf->dirs_to_watch[i] = (char *) malloc(
                (strlen(dir_to_watch->valuestring) + 1) * sizeof(char)
            );
            if (cnf->dirs_to_watch[i] == NULL) {
                logger(
                    ERROR,
                    "Error occured while allocating \"dirs_to_watch[%d]\"",
                    i
                );
                return 1;
            }

            strcpy(cnf->dirs_to_watch[i], dir_to_watch->valuestring);
            recursive_mkdir(dir_to_watch->valuestring);
        } else {
            logger(WARNING, "Warning: wrong value in \"dirs_to_watch\"");
        }
        i++;
    }

    return 0;
}

int init_watchers(config *cnf, cJSON *json)
{
    const cJSON *w = NULL;
    const cJSON *ws = NULL;

    ws = cJSON_GetObjectItemCaseSensitive(json, "watchers");
    cnf->watchers_length = cJSON_GetArraySize(ws);

    cnf->watchers = (watcher *) calloc(
        cnf->watchers_length * sizeof(watcher), sizeof(watcher)
    );
    if (cnf->watchers == NULL) {
        logger(ERROR, "Error occured while allocating \"watchers\"");
        return 1;
    }

    int i = 0;
    cJSON_ArrayForEach(w, ws) {
        cJSON *name = cJSON_GetObjectItemCaseSensitive(w, "name");
        cJSON *dir_to_move = cJSON_GetObjectItemCaseSensitive(w, "dir_to_move");
        cJSON *enabled = cJSON_GetObjectItemCaseSensitive(w, "enabled");

        const cJSON *file_type = NULL;
        const cJSON *file_types = NULL;
        file_types = cJSON_GetObjectItemCaseSensitive(w, "file_types");

        if (!cJSON_IsString(name) || !cJSON_IsString(dir_to_move)
            || !cJSON_IsBool(enabled) || !cJSON_IsArray(file_types)) {
            logger(ERROR, "Invalid config!");
            return 1;
        }

        if (cJSON_IsTrue(enabled)) {
            // Init watcher name
            cnf->watchers[i].name = (char *) malloc(
                (strlen(name->valuestring) + 1) * sizeof(char)
            );
            if (cnf->watchers[i].name == NULL) {
                logger(
                    ERROR,
                    "Error occured while allocating \"watchers[i].name\""
                );
                return 1;
            }
            strcpy(cnf->watchers[i].name, name->valuestring);

            // Init watcher dir_to_move
            cnf->watchers[i].dir_to_move = (char *) malloc(
                (strlen(dir_to_move->valuestring) + 1) * sizeof(char)
            );
            if (cnf->watchers[i].dir_to_move == NULL) {
                logger(
                    ERROR,
                    "Error occured while allocating \"watchers[i].dir_to_move\""
                );
                return 1;
            }

            strcpy(cnf->watchers[i].dir_to_move, dir_to_move->valuestring);
            recursive_mkdir(dir_to_move->valuestring);

            // Init watcher file_types and types_length
            cnf->watchers[i].types_length = cJSON_GetArraySize(file_types);
            cnf->watchers[i].file_types = (char **) malloc(
                cnf->watchers[i].types_length * sizeof(char *)
            );
            if (cnf->watchers[i].file_types == NULL) {
                logger(
                    ERROR,
                    "Error occured while allocating \"watchers[i].file_types\""
                );
                return 1;
            }
            int j = 0;
            cJSON_ArrayForEach(file_type, file_types) {
                if (cJSON_IsString(file_type)) {
                    cnf->watchers[i].file_types[j] = (char *) malloc(
                        (strlen(file_type->valuestring) + 1) * sizeof(char)
                    );
                    if (cnf->watchers[i].file_types[j] == NULL) {
                        logger(
                            ERROR,
                            "Error occured while allocating \"watchers[i].file_types[j]\""
                        );
                        return 1;
                    }
                    strcpy(cnf->watchers[i].file_types[j], file_type->valuestring);
                } else {
                    logger(WARNING, "Wrong value in: \"file_types\"...");
                }

                j++;
            }

            i++;
        }
    }

    return 0;
}

config *init_config(char *conf_name, char *default_home)
{
    char *def_conf = NULL;
    char *conf_string = read_file(conf_name);
    cJSON *json = cJSON_Parse(conf_string);

    if (json == NULL) {
        logger(WARNING, "Using default configuration...");
        def_conf = create_default_config(default_home);
        json = cJSON_Parse(def_conf);
    }

    config *cnf = (config *) malloc(sizeof(config));
    if (cnf == NULL || init_dirs_to_watch(cnf, json) == 1
        || init_watchers(cnf, json) == 1) {
        logger(ERROR, "Error on init_config: %s", strerror(errno));
    }

    cJSON_Delete(json);
    FREE(conf_string);
    FREE(def_conf);
    return cnf;
}

void free_config(config *conf)
{
    for (int i = 0; i < conf->dirs_lenght; i++) {
        FREE(conf->dirs_to_watch[i]);
    }

    for (int i = 0; i < conf->watchers_length; i++) {
        for (int j = 0; j < conf->watchers[i].types_length; j++) {
            FREE(conf->watchers[i].file_types[j]);
        }

        FREE(
            conf->watchers[i].file_types,
            conf->watchers[i].name,
            conf->watchers[i].dir_to_move
        );
    }

    FREE(
        conf->dirs_to_watch,
        conf->watchers,
        conf->wd,
        conf
    );
}
