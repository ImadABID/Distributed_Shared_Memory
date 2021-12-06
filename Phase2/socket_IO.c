#include "socket_IO.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void receive_data(int sckt, char * buffer, size_t size_p){

    //Receiving data
    int treated_size = 0;
    char *data_cursor = buffer;
    int err;
    int max_attempts = 10;
    int attempts=0;

    while(treated_size < size_p){
        err = read(sckt, data_cursor, size_p-treated_size);
        if(err == -1){
            perror("read");
            exit(EXIT_FAILURE);
        }
        treated_size+=err;
        data_cursor+=err;
        if(err == 0){
            if(attempts == max_attempts){
                fprintf(stderr, "receive_data : Max attempts reached.\n");
                exit(EXIT_FAILURE);
            }
            attempts ++;
        }else{
            attempts = 0;
        }
    }

}