#include "kmeans.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>



int main(int argc, char* argv[]){
    int arg_key = atoi(argv[1]);
    int arg_psize = atoi(argv[2]);
    int arg_csize = atoi(argv[3]);
    int arg_index = atoi(argv[4]);
    int arg_iter = atoi(argv[5]);
    int start_index, end_index;
    int tmp = arg_psize/PROCNUM;
    start_index = tmp*arg_index;
    char wfifoName[10];
    char rfifoName[10];
    char buf[5];
    strcpy(wfifoName, "./");
    sprintf(buf, "%d", arg_index);
    strcat(wfifoName, buf);
    strcpy(rfifoName, "./");
    sprintf(buf, "%d", arg_index+PROCNUM);
    strcat(rfifoName, buf);

    if (arg_index==PROCNUM-1){
	end_index = arg_psize-1;
    }else {
	end_index = (tmp*(arg_index+1))-1;
    }
    int shm_point = shmget(KEY, 0, 0);
    int shm_cluster = shmget(KEY+1, 0, 0);
    Point* point_addr = shmat(shm_point, NULL, 0);
    Point* cluster_addr = shmat(shm_cluster, NULL, 0);
    int fdw = open(wfifoName, O_WRONLY);
    int fdr = open(rfifoName, O_RDONLY);
    
    for (int i=0;i<arg_iter;i++){
	read(fdr, buf, sizeof(buf));/*receiving signal from parents, after
				      recieving, starts it's own task*/
	p_determineCluster(point_addr, cluster_addr, start_index, end_index,
		arg_csize);
	write(fdw, buf, sizeof(buf));/*sending signal to parents to inform his
				       own task was all done*/
    }
    close(fdr);
    close(fdw);
    shmdt(point_addr);
    shmdt(cluster_addr);
}
