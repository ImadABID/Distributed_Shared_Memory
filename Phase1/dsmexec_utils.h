#ifndef __DSMEXEC_UTILS_H__
#define __DSMEXEC_UTILS_H__

/*
Param:
    char **buffer : allocated in this function free *buffer after use.

returns : number of files.
*/
int read_machine_names(char *path, char **buffer);

/*
Param:
    Nombre de processus.
returns : Create listening socket
*/
int socket_listen_and_bind(int Nb_proc);

#endif