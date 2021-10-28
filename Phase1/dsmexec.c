#include "dsmexec_utils.h"
#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common_impl.h"

/* variables globales */

/* un tableau gerant les infos d'identification */
/* des processus dsm */
dsm_proc_t *proc_array = NULL; 

/* le nombre de processus effectivement crees */
volatile int num_procs_creat = 0;

void usage(void)
{
  fprintf(stdout,"Usage : dsmexec machine_file executable arg1 arg2 ...\n");
  fflush(stdout);
  exit(EXIT_FAILURE);
}

void sigchld_handler(int sig)
{
   /* on traite les fils qui se terminent */
   /* pour eviter les zombies */
}

/*******************************************************/
/*********** ATTENTION : BIEN LIRE LA STRUCTURE DU *****/
/*********** MAIN AFIN DE NE PAS AVOIR A REFAIRE *******/
/*********** PLUS TARD LE MEME TRAVAIL DEUX FOIS *******/
/*******************************************************/

int main(int argc, char *argv[])
{
  if (argc < 3){
    usage();
  } else {    
       
     dsm_proc_t *dsm_procs;
     pid_t pid;
     int num_procs = 0;
     int i;
     char buff[MAX_STR];
     int length;
     int port; 
     
     /* Mise en place d'un traitant pour recuperer les fils zombies*/      
     /* XXX.sa_handler = sigchld_handler; */
     
     /* lecture du fichier de machines */
     /* 1- on recupere le nombre de processus a lancer */
     /* 2- on recupere les noms des machines : le nom de */
     /* la machine est un des elements d'identification */
     char *machine_name;
	  num_procs = read_machine_names(argv[1], &machine_name);

		for(int i = 0; i<num_procs; i++){
			printf("machine name : %s\n", machine_name+MAX_STR*i);
		}


     free(machine_name);
     
     /* creation de la socket d'ecoute */
     /* + ecoute effective */ 
      int listen_fd = -1;
      if (-1 == (listen_fd = socket_listen_and_bind(num_procs,port))) {
         printf("Could not create, bind and listen properly\n");
         return 1;
      }
     
     /* creation des fils */
     for(i = 0; i < num_procs ; i++) {
	
	/* creation du tube pour rediriger stdout */
	
	/* creation du tube pour rediriger stderr */
	
	pid = fork();
	if(pid == -1) ERROR_EXIT("fork");
	
	if (pid == 0) { /* fils */	
	   
	   /* redirection stdout */	      
	   
	   /* redirection stderr */	      	      
	   
	   /* Creation du tableau d'arguments pour le ssh */ 
	   
	   /* jump to new prog : */
	   /* execvp("ssh",newargv); */

	} else  if(pid > 0) { /* pere */		      
	   /* fermeture des extremites des tubes non utiles */
	   num_procs_creat++;	      
	}
     }
   
     for(i = 0; i < num_procs ; i++){
	
	/* on accepte les connexions des processus dsm */
				struct sockaddr_in csin;
				socklen_t size = sizeof(csin);
				if (-1 == (dsm_procs[i].connect_info.fd = accept(listen_fd, (struct sockaddr *)&csin, &size))) {
					perror("Accept");
				}
	
	/*  On recupere le nom de la machine distante */	
	/* 1- puis la chaine elle-meme */
            memset(buff,0,MAX_STR);
				if (recv(dsm_procs[i].connect_info.fd , buff, MAX_STR, 0) <= 0) {
					ERROR_EXIT("revc");
				}
       
	/* On recupere le pid du processus distant  (optionnel)*/
            pid_t pid_proc = 0;
            if (recv(dsm_procs[i].connect_info.fd , &pid_proc, sizeof(int), 0) <= 0) {
					ERROR_EXIT("revc");
				}
            dsm_procs[i].pid = pid_proc;
	/* On recupere le numero de port de la socket */
	/* d'ecoute des processus distants */
        /* cf code de dsmwrap.c */  
            int port_proc = 0;
            if (recv(dsm_procs[i].connect_info.fd , port_proc, sizeof(int), 0) <= 0) {
					ERROR_EXIT("revc");
				}
     }

     /***********************************************************/ 
     /********** ATTENTION : LE PROTOCOLE D'ECHANGE *************/
     /********** DECRIT CI-DESSOUS NE DOIT PAS ETRE *************/
     /********** MODIFIE, NI DEPLACE DANS LE CODE   *************/
     /***********************************************************/
     
     /* 1- envoi du nombre de processus aux processus dsm*/
     /* On envoie cette information sous la forme d'un ENTIER */
     /* (IE PAS UNE CHAINE DE CARACTERES */
     
     /* 2- envoi des rangs aux processus dsm */
     /* chaque processus distant ne reçoit QUE SON numéro de rang */
     /* On envoie cette information sous la forme d'un ENTIER */
     /* (IE PAS UNE CHAINE DE CARACTERES */
     
     /* 3- envoi des infos de connexion aux processus */
     /* Chaque processus distant doit recevoir un nombre de */
     /* structures de type dsm_proc_conn_t égal au nombre TOTAL de */
     /* processus distants, ce qui signifie qu'un processus */
     /* distant recevra ses propres infos de connexion */
     /* (qu'il n'utilisera pas, nous sommes bien d'accords). */

     /***********************************************************/
     /********** FIN DU PROTOCOLE D'ECHANGE DES DONNEES *********/
     /********** ENTRE DSMEXEC ET LES PROCESSUS DISTANTS ********/
     /***********************************************************/
     
     /* gestion des E/S : on recupere les caracteres */
     /* sur les tubes de redirection de stdout/stderr */     
     /* while(1)
         {
            je recupere les infos sur les tubes de redirection
            jusqu'à ce qu'ils soient inactifs (ie fermes par les
            processus dsm ecrivains de l'autre cote ...)
       
         };
      */

     /* on attend les processus fils */
     
     /* on ferme les descripteurs proprement */
     
     /* on ferme la socket d'ecoute */
  }   
   exit(EXIT_SUCCESS);  
}

