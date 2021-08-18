#pragma once

//System includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <pwd.h>
#include <limits.h>

//Library
#include "../lib/cJSON.h"

//Source
#include "logger.h"
#include "utils.h"
#include "config.h"
#include "notify.h"
#include "daemon.h"
