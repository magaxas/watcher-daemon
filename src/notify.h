#pragma once

#define MAX_EVENTS 1024                                //Max. number of events to process at one go
#define LEN_NAME 1024                                  //Assuming length of the filename won't exceed 16 bytes
#define EVENT_SIZE (sizeof(struct inotify_event))      //size of one event
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME)) //buffer to store the data of events

extern void init_notify(config *conf);
extern void get_event(config *conf, void (*callback)());
extern void free_notify(config *conf);
