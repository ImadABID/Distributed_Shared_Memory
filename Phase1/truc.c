#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common_impl.h"
#include <sys/socket.h>


int main(int argc, char *argv[])
{
   int fd;
   int i;
   char str[1024];
   char exec_path[2048];   
   char *wd_ptr = NULL;
   
   wd_ptr = getcwd(str,1024);
   fprintf(stdout,"Working dir is %s\n",str);
   
   fprintf(stdout,"Number of args : %i\n", argc);
   for(i= 0; i < argc ; i++)
     fprintf(stderr,"arg[%i] : %s\n",i,argv[i]);
    
   sprintf(exec_path,"%s/%s",str,"titi");	      
   fd = open(exec_path,O_RDONLY);
   if(fd == -1) perror("open");
   fprintf(stdout,"================ Valeur du descripteur : %i\n",fd);

  
   /* 1- recevoir du nombre de processus */
	/* On reçoie cette information sous la forme d'un ENTIER */
	/* (IE PAS UNE CHAINE DE CARACTERES */
		int nb_proc;
		if (recv(atoi(argv[argc-1]), &nb_proc, sizeof(int), 0) <= 0) {
			ERROR_EXIT("send");
		}		

	/* 2- recevoir des rangs */

		int nb_rank;
		if (recv(atoi(argv[argc-1]), &nb_rank, sizeof(int), 0) <= 0) { // le sock_fd est le dernier argument
			ERROR_EXIT("send");
		}
	
	/* 3- recevoir des infos de connexion  */
    dsm_proc_conn_t tab_struct[nb_proc];
    memset(tab_struct,0,nb_proc*sizeof(dsm_proc_conn_t));
    for(int j = 0; j < nb_proc ; j++){
			dsm_proc_conn_t msgstruct;
			memset(&msgstruct,0,sizeof(dsm_proc_conn_t));
			if (recv(atoi(argv[argc-1]), &msgstruct, sizeof(dsm_proc_conn_t), 0) <= 0) {
			ERROR_EXIT("send");
      }
      tab_struct[j].fd = msgstruct.fd;
      tab_struct[j].fd_for_exit = msgstruct.fd_for_exit;
      tab_struct[j].port_num = msgstruct.port_num;
      tab_struct[j].rank = msgstruct.rank;
      strcpy(tab_struct[j].machine,msgstruct.machine);
      fprintf(stdout,"==> fd : %i\n==> fd_exit : %i\n==> Nom machine : %s\n==> Nombre port : %i\n==> Nombre rang : %i\n",msgstruct.fd,msgstruct.fd_for_exit,msgstruct.machine,msgstruct.port_num,msgstruct.rank);

    }
  fprintf(stdout,"==> Nombre de processus : %i\n==> Nombre rang : %i\n",nb_proc,nb_rank);
		
   return 0;
}
