#include "common_impl.h"
#include "dsmexec_utils.h"

int main(int argc, char **argv)
{   
   /* processus intermediaire pour "nettoyer" */
   /* la liste des arguments qu'on va passer */
   /* a la commande a executer finalement  */
   
   /* creation d'une socket pour se connecter au */
   /* au lanceur et envoyer/recevoir les infos */
   /* necessaires pour la phase dsm_init */   
   int sock_fd = -1;
	if (-1 == (sock_fd = socket_and_connect(argv[1], argv[2]))){
		printf("Could not create socket and connect properly\n");
		return 1;
	}
   char sock[MAX_STR];
   memset(sock,0,MAX_STR);
   sprintf(sock,"%d",sock_fd);
   setenv("DSMEXEC_FD",sock,0);
   
   char buff[MAX_STR];
   int length;
   /* Envoi du nom de machine au lanceur */             
   memset(buff,0,MAX_STR);
   if ( -1 == (gethostname(buff, MAX_STR))){
      perror("gethostname");
   }

   if (send(sock_fd, buff, MAX_STR, 0) <= 0) {
      ERROR_EXIT("send");
   }
   /* Envoi du pid au lanceur (optionnel) */
   pid_t pid_wrap = getpid();

   if (send(sock_fd, pid_wrap, sizeof(int), 0) <= 0) {
      ERROR_EXIT("send");
   }

   /* Creation de la socket d'ecoute pour les */
   /* connexions avec les autres processus dsm */
   int listen_fd = -1;
   int port;
   if (-1 == (listen_fd = socket_listen_and_bind(64,port))) {
      printf("Could not create, bind and listen properly\n");
      return 1;
   }

   memset(sock,0,MAX_STR);
   sprintf(sock,"%d",listen_fd);
   setenv("MASTER_FD",sock,0);
   /* Envoi du numero de port au lanceur */
   /* pour qu'il le propage à tous les autres */
   /* processus dsm */
 
   /* on execute la bonne commande */
   /* attention au chemin à utiliser ! */

   /************** ATTENTION **************/
   /* vous remarquerez que ce n'est pas   */
   /* ce processus qui récupère son rang, */
   /* ni le nombre de processus           */
   /* ni les informations de connexion    */
   /* (cf protocole dans dsmexec)         */
   /***************************************/
  
   return 0;
}
