#pragma once

#define MAX_EVENTS 1024                                //Max. number of events to process at one go
#define LEN_NAME 1024                                  //Assuming length of the filename won't exceed 16 bytes
#define EVENT_SIZE (sizeof(struct inotify_event))      //Size of one event
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME)) //Buffer to store the data of events

int init_notify(config *conf);
int get_event(config *conf, int (*callback)(config *conf, char *file_name, int wd));

void free_notify(config *conf);
