#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv){
	
    if(argc < 3){
	  fprintf(2, "strace: Invalid number of inputs\n");
  	  exit(1);
	}else{
	    char *comm = argv[2];
        char **comms = &argv[2];
	    int num_m = atoi(argv[1]);
	    strace(num_m);
	    exec(comm, comms);

        return 0;
    }
}