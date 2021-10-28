#include "dsmexec_utils.h"
#include "common_impl.h"

#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int read_machine_names(char *path, char **buffer){

    int err;
    int nbr_lines = 0;
    int fd = open(path, O_RDONLY | O_EXCL);
    if(fd == -1){
        ERROR_EXIT("read_machine_names open");
    }

    // Recuperer nbr_lines
    char c;
    do{
        err = read(fd, (void *)&c, sizeof(char));
        if(err == 0){
            break;
        }
        if(c == '\n')
            nbr_lines++;
    }while(1);

    *buffer = malloc(nbr_lines * MAX_STR * sizeof(char));
    if(*buffer == NULL){
        ERROR_EXIT("read_machine_names malloc");
    }

    close(fd);

    fd = open(path, O_RDONLY | O_EXCL);
    if(fd == -1){
        ERROR_EXIT("read_machine_names open");
    }

    int machine = 0;
    int index_in_name=0;
    do{
        err = read(fd, (void *)&c, sizeof(char));
        if(err == 0){
            break;
        }
        
        if(c == '\n'){
            (*buffer)[machine*MAX_STR+index_in_name] = '\0';
            machine++;
            index_in_name = 0;
        }else{
            (*buffer)[machine*MAX_STR+index_in_name] = c;
            index_in_name++;
        }

    }while(1);

    close(fd);

    return nbr_lines;

}