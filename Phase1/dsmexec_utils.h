#ifndef __DSMEXEC_UTILS_H__
#define __DSMEXEC_UTILS_H__

#include "common_impl.h"

/*
Param:
    char **buffer : allocated in this function free *buffer after use.

returns : number of files.
*/
int read_machine_names(char *path, dsm_proc_t **dsm_procs);
void read_from_pipe(int pipe_fd, char *buffer);

#endif