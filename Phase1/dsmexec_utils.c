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

int socket_listen_and_bind(int Nb_proc) {
	int listen_fd = -1;
	if (-1 == (listen_fd = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("Socket");
		exit(EXIT_FAILURE);
	}
	printf("Listen socket descriptor %d\n", listen_fd);

	int yes = 1;
	if (-1 == setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	struct addrinfo indices;
	memset(&indices, 0, sizeof(struct addrinfo));
	indices.ai_family = AF_INET;
	indices.ai_socktype = SOCK_STREAM; //TCP
	indices.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
	struct addrinfo *res, *tmp;


	int err = 0;
	if (0 != (err = getaddrinfo("0.0.0.0",NULL, &indices, &res))) {
		errx(1, "%s", gai_strerror(err));
	}

	tmp = res;
	while (tmp != NULL) {
		if (tmp->ai_family == AF_INET) {
			struct sockaddr_in *sockptr = (struct sockaddr_in *)(tmp->ai_addr);
			struct in_addr local_address = sockptr->sin_addr;
			
			if (-1 == bind(listen_fd, tmp->ai_addr, tmp->ai_addrlen)) {
				perror("Binding");
			}
			if (-1 == listen(listen_fd, Nb_proc)) {
				perror("Listen");
			}

            struct sockaddr_in sin;
            socklen_t len = sizeof(sin);
            if (getsockname(listen_fd, (struct sockaddr *)&sin, &len) == -1)
                perror("getsockname");
            else
                printf("Binding to %s on port %d\n",inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));
			
			freeaddrinfo(res);
			return listen_fd;
		}
		tmp = tmp->ai_next;
		
	}
	freeaddrinfo(res);
	return listen_fd;
}