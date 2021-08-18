#pragma once

enum { INFO, WARNING, ERROR };

int open_log();
int close_log();
void logger(int level, char *fmt, ...);
