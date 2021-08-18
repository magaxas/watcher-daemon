#include "main.h"

char *create_default_config(void)
{
    cJSON *watchers = NULL;
    cJSON *dirs_to_watch = NULL;
    cJSON *json = cJSON_CreateObject();
    size_t index = 0;

    char *home_dir = get_home_dir();
    strcat(home_dir, "/");

    const char *names[] = {
        "audio",
        "video",
        "photo",
        "document"};

    const char *types[4][3] = {
        {"mp3", "flac", "wav"},
        {"mkv", "avi", "mp4"},
        {"svg", "jpeg", "png"},
        {"docx", "txt", "pdf"}};

    const char *dirs[] = {
        "Downloads/",
        "Music/",
        "Pictures/",
        "Videos/",
        "Documents/"};

    watchers = cJSON_AddArrayToObject(json, "watchers");
    if (watchers == NULL)
    {
        goto end;
    }

    for (index = 0; index < 5; index++)
    {
        char path[PATH_MAX] = "";
        strcat(path, home_dir);
        strcat(path, dirs[index]);
        recursive_mkdir(path);

        if (index == 0)
        {
            dirs_to_watch = cJSON_AddArrayToObject(json, "dirs_to_watch");
            if (dirs_to_watch == NULL)
            {
                goto end;
            }
            else
            {
                cJSON_AddItemToArray(dirs_to_watch, cJSON_CreateString(path));
            }
        }
        else
        {
            cJSON *w = cJSON_CreateObject();

            if (cJSON_AddStringToObject(w, "name", names[index - 1]) == NULL)
            {
                goto end;
            }
            if (cJSON_AddStringToObject(w, "dir_to_move", path) == NULL)
            {
                goto end;
            }
            if (cJSON_AddBoolToObject(w, "enabled", true) == NULL)
            {
                goto end;
            }
            cJSON *file_types = NULL;
            file_types = cJSON_AddArrayToObject(w, "file_types");
            if (file_types == NULL)
            {
                goto end;
            }
            else
            {
                for (int i = 0; i < 3; i++)
                {
                    cJSON_AddItemToArray(file_types, cJSON_CreateString(types[index - 1][i]));
                }
            }

            cJSON_AddItemToArray(watchers, w);
        }
    }

    char *string = NULL;
    string = cJSON_Print(json);
    if (string == NULL)
    {
        printf("Failed to print monitor\n");
    }

end:
    cJSON_Delete(json);
    return string;
}

char *read_file(char *file_name)
{
    long lSize;
    char *buffer = NULL;

    FILE *fp = fopen(file_name, "r");
    if (!fp)
    {
        logger(WARNING, "Cannot find config file: %s", file_name);
        goto end;
    }

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    buffer = calloc(1, lSize + 1);
    if (!buffer)
    {
        logger(ERROR, "Memory allocation failed while reading file: %s", file_name);
    }
    else if (fread(buffer, lSize, 1, fp) != 1)
    {
        logger(ERROR, "Cannot open config file: %s", file_name);
    }

    fclose(fp);
end:
    return buffer;
}

void init_dirs_to_watch(config *cnf, cJSON *json)
{
    const cJSON *dir_to_watch = NULL;
    const cJSON *dirs_to_watch = NULL;

    dirs_to_watch = cJSON_GetObjectItemCaseSensitive(json, "dirs_to_watch");
    cnf->dirs_lenght = cJSON_GetArraySize(dirs_to_watch);
    cnf->dirs_to_watch = (char **)malloc(cnf->dirs_lenght * sizeof(char *));
    if (cnf->dirs_to_watch == NULL)
    {
        logger(ERROR, "Error occured while allocating \"dirs_to_watch\"");
        free_notify(cnf);
        free_config(cnf);
        close_log();
        exit(1);
    }

    int i = 0;
    cJSON_ArrayForEach(dir_to_watch, dirs_to_watch)
    {
        if (cJSON_IsString(dir_to_watch))
        {
            cnf->dirs_to_watch[i] = (char *)malloc(
                (strlen(dir_to_watch->valuestring) + 1) * sizeof(char));
            if (cnf->dirs_to_watch[i] == NULL)
            {
                logger(ERROR, "Error occured while allocating \"dirs_to_watch[%d]\"", i);
                free_notify(cnf);
                free_config(cnf);
                close_log();
                exit(1);
            }

            strcpy(cnf->dirs_to_watch[i], dir_to_watch->valuestring);
            recursive_mkdir(dir_to_watch->valuestring);
        }
        else
        {
            logger(WARNING, "Warning: wrong value in \"dirs_to_watch\"");
        }
        i++;
    }
}

void init_watchers(config *cnf, cJSON *json)
{
    const cJSON *w = NULL;
    const cJSON *ws = NULL;

    ws = cJSON_GetObjectItemCaseSensitive(json, "watchers");
    cnf->watchers_length = cJSON_GetArraySize(ws);
    cnf->watchers = (watcher *)calloc(cnf->watchers_length * sizeof(watcher), sizeof(watcher));
    if (cnf->watchers == NULL)
    {
        logger(ERROR, "Error occured while allocating \"watchers\"");
        free_notify(cnf);
        free_config(cnf);
        close_log();
        exit(1);
    }

    int i = 0;
    cJSON_ArrayForEach(w, ws)
    {
        cJSON *name = cJSON_GetObjectItemCaseSensitive(w, "name");
        cJSON *dir_to_move = cJSON_GetObjectItemCaseSensitive(w, "dir_to_move");
        cJSON *enabled = cJSON_GetObjectItemCaseSensitive(w, "enabled");

        const cJSON *file_type = NULL;
        const cJSON *file_types = NULL;
        file_types = cJSON_GetObjectItemCaseSensitive(w, "file_types");

        if (!cJSON_IsString(name) || !cJSON_IsString(dir_to_move) || !cJSON_IsBool(enabled) || !cJSON_IsArray(file_types))
        {
            logger(ERROR, "Invalid config!");
            free_notify(cnf);
            free_config(cnf);
            close_log();
            exit(1);
        }

        //Init watcher name
        cnf->watchers[i].name = (char *)malloc(
            (strlen(name->valuestring) + 1) * sizeof(char));
        if (cnf->watchers[i].name == NULL)
        {
            logger(ERROR, "Error occured while allocating \"watchers[i].name\"");
            free_notify(cnf);
            free_config(cnf);
            close_log();
            exit(1);
        }
        strcpy(cnf->watchers[i].name, name->valuestring);

        //Init watcher dir_to_move
        cnf->watchers[i].dir_to_move = (char *)malloc(
            (strlen(dir_to_move->valuestring) + 1) * sizeof(char));
        if (cnf->watchers[i].dir_to_move == NULL)
        {
            logger(ERROR, "Error occured while allocating \"watchers[i].dir_to_move\"");
            free_notify(cnf);
            free_config(cnf);
            close_log();
            exit(1);
        }
        strcpy(cnf->watchers[i].dir_to_move, dir_to_move->valuestring);
        recursive_mkdir(dir_to_move->valuestring);

        //Init watcher file_types and types_length
        cnf->watchers[i].types_length = cJSON_GetArraySize(file_types);
        cnf->watchers[i].file_types = (char **)malloc(
            cnf->watchers[i].types_length * sizeof(char *));
        if (cnf->watchers[i].file_types == NULL)
        {
            logger(ERROR, "Error occured while allocating \"watchers[i].file_types\"");
            free_notify(cnf);
            free_config(cnf);
            close_log();
            exit(1);
        }
        int j = 0;
        cJSON_ArrayForEach(file_type, file_types)
        {
            if (cJSON_IsString(file_type))
            {
                cnf->watchers[i].file_types[j] = (char *)malloc(
                    (strlen(file_type->valuestring) + 1) * sizeof(char));
                if (cnf->watchers[i].file_types[j] == NULL)
                {
                    logger(ERROR, "Error occured while allocating \"watchers[i].file_types[j]\"");
                    free_notify(cnf);
                    free_config(cnf);
                    close_log();
                    exit(1);
                }
                strcpy(cnf->watchers[i].file_types[j], file_type->valuestring);
            }
            else
            {
                logger(WARNING, "Wrong value in: \"file_types\"...");
            }

            j++;
        }

        i++;
    }
}

config *init_config(char *conf_name)
{
    char *conf_string = read_file(conf_name);
    cJSON *json = cJSON_Parse(conf_string);

    if (json == NULL)
    {
        logger(WARNING, "Using default configuration...");
        json = cJSON_Parse(create_default_config());
    }

    config *cnf = (config *)malloc(sizeof(config));
    if (cnf == NULL)
    {
        logger(ERROR, "Error occured while allocating \"cnf\"");
        cJSON_Delete(json);
        FREE(conf_string);
        close_log();
        exit(1);
    }
    init_dirs_to_watch(cnf, json);
    init_watchers(cnf, json);

    cJSON_Delete(json);
    FREE(conf_string);
    return cnf;
}

void free_config(config *conf)
{
    for (int i = 0; i < conf->dirs_lenght; i++)
    {
        FREE(conf->dirs_to_watch[i]);
    }

    for (int i = 0; i < conf->watchers_length; i++)
    {
        for (int j = 0; j < conf->watchers[i].types_length; j++)
        {
            FREE(conf->watchers[i].file_types[j]);
        }

        MFREE(3,
              conf->watchers[i].file_types,
              conf->watchers[i].name,
              conf->watchers[i].dir_to_move);
    }

    MFREE(4, conf->dirs_to_watch, conf->watchers, conf->wd, conf);
}
