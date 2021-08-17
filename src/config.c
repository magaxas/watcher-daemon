#include "main.h"

char *readFile(char *file_name)
{
    long lSize;
    char *buffer = NULL;

    FILE *fp = fopen(file_name, "r");
    if (!fp)
    {
        perror(file_name);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    buffer = calloc(1, lSize + 1);
    if (!buffer)
    {
        fclose(fp);
        printf("memory alloc fails\n");
        exit(1);
    }
    else if (fread(buffer, lSize, 1, fp) != 1)
    {
        fclose(fp);
        FREE(buffer);
        printf("Error! trying to open the file\n");
        exit(1);
    }

    fclose(fp);
    return buffer;
}

void init_dirs_to_watch(config *cnf, cJSON *json)
{
    const cJSON *dir_to_watch = NULL;
    const cJSON *dirs_to_watch = NULL;

    dirs_to_watch = cJSON_GetObjectItemCaseSensitive(json, "dirs_to_watch");
    cnf->dirs_lenght = cJSON_GetArraySize(dirs_to_watch);
    cnf->dirs_to_watch = (char **)malloc(cnf->dirs_lenght * sizeof(char *));

    int i = 0;
    cJSON_ArrayForEach(dir_to_watch, dirs_to_watch)
    {
        if (cJSON_IsString(dir_to_watch))
        {
            cnf->dirs_to_watch[i] = (char *)malloc(
                (strlen(dir_to_watch->valuestring) + 1) * sizeof(char));
            strcpy(cnf->dirs_to_watch[i], dir_to_watch->valuestring);
        }
        else
        {
            printf("Warning: wrong value in \"dirs_to_watch\"");
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
            //initialize conf with nulls
            //reallocate if watcher is disabled
            //error handling like this & move to function handle_error(char *msg)
            perror("Wrong config configuration! Exiting...");
            cJSON_Delete(json);
            free_config(cnf);
            exit(1);
        }

        //Init watcher name
        cnf->watchers[i].name = (char *)malloc(
            (strlen(name->valuestring) + 1) * sizeof(char));
        strcpy(cnf->watchers[i].name, name->valuestring);

        //Init watcher dir_to_move
        cnf->watchers[i].dir_to_move = (char *)malloc(
            (strlen(dir_to_move->valuestring) + 1) * sizeof(char));
        strcpy(cnf->watchers[i].dir_to_move, dir_to_move->valuestring);

        //Init watcher file_types and types_length
        cnf->watchers[i].types_length = cJSON_GetArraySize(file_types);
        cnf->watchers[i].file_types = (char **)malloc(
            cnf->watchers[i].types_length * sizeof(char *));

        int j = 0;
        cJSON_ArrayForEach(file_type, file_types)
        {
            if (cJSON_IsString(file_type))
            {
                cnf->watchers[i].file_types[j] = (char *)malloc(
                    (strlen(file_type->valuestring) + 1) * sizeof(char));
                strcpy(cnf->watchers[i].file_types[j], file_type->valuestring);
            }
            else
            {
                printf("Warning: wrong value in \"file_types\"");
            }

            j++;
        }

        i++;
    }
}

config *init_config(char *conf_name)
{
    char *conf_string = readFile(conf_name);
    cJSON *json = cJSON_Parse(conf_string);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        FREE(conf_string);
        exit(1);
    }

    config *cnf = (config *)malloc(sizeof(config));

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

void print_config(config *conf)
{
    printf("Directories to watch:\n");
    for (int i = 0; i < conf->dirs_lenght; i++)
    {
        printf("%s\n", conf->dirs_to_watch[i]);
    }

    printf("Watchers:\n");
    for (int i = 0; i < conf->watchers_length; i++)
    {
        printf("Name: %s\n", conf->watchers[i].name);
        printf("Dir to move: %s\n", conf->watchers[i].dir_to_move);
        printf("Enabled: %s\n", conf->watchers[i].enabled ? "true" : "false");

        printf("File types: ");
        for (int j = 0; j < conf->watchers[i].types_length; j++)
        {
            printf("%s ", conf->watchers[i].file_types[j]);
        }
        printf("\n-----------------------\n");
    }
}