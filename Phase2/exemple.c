#include "dsm.h"

int main(int argc, char **argv)
{
  char *pointer;

  pointer = dsm_init(argc,argv);

  *((int *) (pointer + DSM_NODE_ID*sizeof(int))) = DSM_NODE_ID+100;

  for(int i = 0; i < DSM_NODE_NUM; i++){
    printf("%i | %i\n", i, *((int *) (pointer + i*sizeof(int))));
  }

  *((int *) (pointer + DSM_NODE_ID*sizeof(int))) = DSM_NODE_ID+200;

  for(int i = 0; i < DSM_NODE_NUM; i++){
    printf("%i | %i\n", i, *((int *) (pointer + i*sizeof(int))));
  }

  dsm_finalize();

  return 0;
}
