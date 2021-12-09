#include "dsm.h"
#include "common_impl.h"
#include "socket_IO.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>




/* indique l'adresse de debut de la page de numero numpage */
static char *num2address( int numpage )
{ 
   char *pointer = (char *)(BASE_ADDR+(numpage*(PAGE_SIZE)));
   
   if( pointer >= (char *)TOP_ADDR ){
      fprintf(stderr,"[%i] Invalid address !\n", DSM_NODE_ID);
      return NULL;
   }
   else return pointer;
}

/* cette fonction permet de recuperer un numero de page */
/* a partir  d'une adresse  quelconque */
static int address2num( char *addr )
{
  return (((intptr_t)(addr - BASE_ADDR))/(PAGE_SIZE));
}

/* cette fonction permet de recuperer l'adresse d'une page */
/* a partir d'une adresse quelconque (dans la page)        */
static char *address2pgaddr( char *addr )
{
  return  (char *)(((intptr_t) addr) & ~(PAGE_SIZE-1)); 
}

/* fonctions pouvant etre utiles */
static void dsm_change_info( int numpage, dsm_page_state_t state, dsm_page_owner_t owner)
{
   if ((numpage >= 0) && (numpage < PAGE_NUMBER)) {	
	if (state != NO_CHANGE )
	table_page[numpage].status = state;
      if (owner >= 0 )
	table_page[numpage].owner = owner;
      return;
   }
   else {
	fprintf(stderr,"[%i] Invalid page number !\n", DSM_NODE_ID);
      return;
   }
}

static dsm_page_owner_t get_owner( int numpage)
{
   return table_page[numpage].owner;
}

static dsm_page_state_t get_status( int numpage)
{
   return table_page[numpage].status;
}

/* Allocation d'une nouvelle page */
static void dsm_alloc_page( int numpage )
{
   char *page_addr = num2address( numpage );
   mmap(page_addr, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
   return ;
}

/* Changement de la protection d'une page */
static void dsm_protect_page( int numpage , int prot)
{
   char *page_addr = num2address( numpage );
   mprotect(page_addr, PAGE_SIZE, prot);
   return;
}

static void dsm_free_page( int numpage )
{
   char *page_addr = num2address( numpage );
   munmap(page_addr, PAGE_SIZE);
   return;
}

static void *dsm_comm_daemon( void *arg)
{  
   while(1)
     {
	/* a modifier */
	printf("[%i] Waiting for incoming reqs \n", DSM_NODE_ID);
	sleep(2);
     }
   return NULL;
}

static int dsm_send(int dest,void *buf,size_t size)
{
   /* a completer */
   char * buff = (char *)buf;
   int ret = 0, offset = 0;
	while (offset != size) {
		if (-1 == (ret = send(proc_conn_info[dest].fd, buff + offset, size - offset,MSG_NOSIGNAL))) {
			perror("send");
			return -1;
		}
		offset += ret;
	}
	return offset;
}

static int dsm_recv(int from,void *buf,size_t size)
{
   /* a completer */
   char * buff = (char *)buf;
   int ret = 0;
	int offset = 0;
	while (offset != size) {
		ret = recv(proc_conn_info[from].fd, buff + offset, size - offset,MSG_NOSIGNAL);
		if (-1 == ret) {
			perror("recv");
         return -1;
		}
		offset += ret;
	}
	return offset;
}

static void dsm_handler(int page_num)
{

   /* A modifier */
   printf("[%i] FAULTY  ACCESS !!! \n",DSM_NODE_ID);
   dsm_page_owner_t owner_rank = get_owner(page_num);

   /* send request type */
   dsm_req_type_t req_type = DSM_REQ;
   dsm_send(owner_rank,&req_type,sizeof(dsm_req_type_t));

   /*send struct info*/
   dsm_req_t dsm_req;
   dsm_req.page_num = page_num;
   dsm_req.source = DSM_NODE_ID;
   dsm_send(owner_rank,&dsm_req,sizeof(dsm_req_t));

   /*recv page*/
   

}

/* traitant de signal adequat */
static void segv_handler(int sig, siginfo_t *info, void *context)
{
   /* A completer */
   /* adresse qui a provoque une erreur */
   void  *addr = info->si_addr;   
  /* Si ceci ne fonctionne pas, utiliser a la place :*/
  /*
   #ifdef __x86_64__
   void *addr = (void *)(context->uc_mcontext.gregs[REG_CR2]);
   #elif __i386__
   void *addr = (void *)(context->uc_mcontext.cr2);
   #else
   void  addr = info->si_addr;
   #endif
   */
   /*
   pour plus tard (question ++):
   dsm_access_t access  = (((ucontext_t *)context)->uc_mcontext.gregs[REG_ERR] & 2) ? WRITE_ACCESS : READ_ACCESS;   
  */   
   /* adresse de la page dont fait partie l'adresse qui a provoque la faute */
   void  *page_addr  = (void *)(((unsigned long) addr) & ~(PAGE_SIZE-1));

   if ((addr >= (void *)BASE_ADDR) && (addr < (void *)TOP_ADDR))
     {
	dsm_handler(address2num(page_addr));
     }
   else
     {
	/* SIGSEGV normal : ne rien faire*/
     }
}

/* Seules ces deux dernieres fonctions sont visibles et utilisables */
/* dans les programmes utilisateurs de la DSM                       */
char *dsm_init(int argc, char *argv[])
{   
   struct sigaction act;
   int index;

   /* Récupération de la valeur des variables d'environnement */
   /* DSMEXEC_FD et MASTER_FD                                 */
   int dsmexec_fd = atoi(getenv("DSMEXEC_FD"));
   int master_fd = atoi(getenv("MASTER_FD"));
   
   /* reception du nombre de processus dsm envoye */
   /* par le lanceur de programmes (DSM_NODE_NUM) */
	if (recv(dsmexec_fd, &DSM_NODE_NUM, sizeof(int), 0) <= 0) {
		ERROR_EXIT("send");
	}	
   
   /* reception de mon numero de processus dsm envoye */
   /* par le lanceur de programmes (DSM_NODE_ID)      */
	if (recv(dsmexec_fd, &DSM_NODE_ID, sizeof(int), sizeof(int)) < 0) { // le sock_fd est le dernier argument
		ERROR_EXIT("DSM_NODE_ID. send");
	}
   
   /* reception des informations de connexion des autres */
   /* processus envoyees par le lanceur :                */
   /* nom de machine, numero de port, etc.               */
   proc_conn_info = malloc(DSM_NODE_NUM * sizeof(dsm_proc_conn_t));

   receive_data(dsmexec_fd, (char *) proc_conn_info, DSM_NODE_NUM*sizeof(dsm_proc_conn_t));

   /*
   memset(proc_conn_info,0,DSM_NODE_NUM*sizeof(dsm_proc_conn_t));
   if (recv(dsmexec_fd, proc_conn_info, DSM_NODE_NUM*sizeof(dsm_proc_conn_t), 0) < DSM_NODE_NUM*sizeof(dsm_proc_conn_t)) {
		ERROR_EXIT("connect info. send");
   }
   */

   //display_connect_info(proc_conn_info, DSM_NODE_NUM);

   printf("connect/accept with all process.\n");
   /* initialisation des connexions              */ 
   /* avec les autres processus : connect/accept */

   /* il faut éviter les doubles connect de la part de deux processus*/

   /* on accepte les connexions des autres processus dsm de rang inférieur */
   for(int j = 0; j < DSM_NODE_ID; j++){
      struct sockaddr_in csin;
      socklen_t size = sizeof(csin);
      int sock_fd = -1;
      if (-1 == (sock_fd = accept(master_fd, (struct sockaddr *)&csin, &size))) {
         ERROR_EXIT("Accept");
      }

      //csin.sin_addr n'est pas un char *.
      //fprintf(stdout,"addr :%s\n",csin.sin_addr); 

      /* recevoir le rang */
      int rank;        
      if (recv(sock_fd, &rank, sizeof(int), MSG_NOSIGNAL) <= 0) {
         ERROR_EXIT("recv");
      }

      proc_conn_info[conn_info_get_index_by_rank(rank)].fd = sock_fd;

      fprintf(stdout,"proc with rank %d was accepted\n", rank);
   }

   /* on se connecte avec les autres processus dsm de rang supérieur*/
   for(int k = DSM_NODE_ID+1; k < DSM_NODE_NUM; k++){
      char hostname[MAX_STR];
      char port_str[MAX_STR];

      rank2hostname(proc_conn_info,k,DSM_NODE_NUM,hostname);
      rank2port(proc_conn_info,k,DSM_NODE_NUM,port_str);

      fprintf(stdout,"connecting to : %s:%s\n", hostname, port_str);

      if (-1 == (proc_conn_info[k].fd = socket_and_connect(hostname,  port_str))){
         fprintf(stderr, "Could not create socket and connect properly\n");
         ERROR_EXIT("socket_and_connect");
      }
      /* Envoi du rang */            
      if (send(proc_conn_info[k].fd, &DSM_NODE_ID, sizeof(int), MSG_NOSIGNAL) <= 0) {
         ERROR_EXIT("send");
      }

   }

   /*test*/
   /*
   for (int j = 0; j < DSM_NODE_NUM;j++){
      fprintf(stdout,"socket accepte %i\n",proc_conn_info[j].fd);
   }
   fprintf(stdout,"\n");
   */

   printf("Connection phase completed.\n");

   
   /* Allocation des pages en tourniquet */
   for(index = 0; index < PAGE_NUMBER; index ++){	
     if ((index % DSM_NODE_NUM) == DSM_NODE_ID)
       dsm_alloc_page(index);	     
     dsm_change_info( index, WRITE, index % DSM_NODE_NUM);
   }
   
   /* mise en place du traitant de SIGSEGV */
   act.sa_flags = SA_SIGINFO; 
   act.sa_sigaction = segv_handler;
   //sigaction(SIGSEGV, &act, NULL);
   
   /* creation du thread de communication           */
   /* ce thread va attendre et traiter les requetes */
   /* des autres processus                          */
   //pthread_create(&comm_daemon, NULL, dsm_comm_daemon, NULL);
   
   /* Adresse de début de la zone de mémoire partagée */
   return ((char *)BASE_ADDR);
}

void dsm_finalize( void )
{
   fflush(stdout);
   /* fermer proprement les connexions avec les autres processus */

   /*fermer les sockets entre les processus distants*/
   for(int j = 0; j < DSM_NODE_NUM; j++){

      /* Pour éviter de fermer n'importe quoi*/
      if (j != DSM_NODE_ID){ 
         close(proc_conn_info[j].fd);
      }
   }
   close(dsmexec_fd);
   close(master_fd);

   /* terminer correctement le thread de communication */
   /* on a pas besoin de la valeur de retour pour le moment on a fait :   */
   pthread_detach(comm_daemon); 

   /* libérer les mémoires allouées */
   free(proc_conn_info);
   
  return;
}