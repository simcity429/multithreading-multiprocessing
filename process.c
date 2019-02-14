#include "kmeans.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>




int main(void){
    int testCase;
    int iterNum;
    int clusterNum;
    int pointNum;
    int key = KEY;
    int status;
    int pipenum = 2*PROCNUM;
    int fd[2*PROCNUM];
    char* fifoName[2*PROCNUM];
    scanf("%d", &testCase);
    for (int i=0;i<pipenum;i++){
	char tmpbuf[5];
	sprintf(tmpbuf, "%d", i);
	fifoName[i] = (char*)malloc(sizeof(char)*20);
	strcpy(fifoName[i], "./");
	strcat(fifoName[i], tmpbuf);
    }

    for(int i=0;i<testCase;i++){
	scanf("%d", &iterNum);
	scanf("%d", &clusterNum);
	scanf("%d", &pointNum);
	if(pointNum<NOPARAL_NUM){/*calculate in noparallel way when the entered
				   pointNum is less then NOPARAL_NUL*/
	    noparallel_process(i, iterNum, clusterNum, pointNum);
	    continue;
	}

        for (int j=0;j<pipenum;j++){
	    remove(fifoName[j]); /*if there's already named fifo files exist,
				   remove them*/
	    mkfifo(fifoName[j], 0666); /*making named fifo file*/
	}
	int shm_point = shmget(key, sizeof(Point)*pointNum,
		IPC_CREAT|0666|IPC_EXCL);
	if (shm_point == -1){/*shared memory that can be accessed by the given key
			       is already exists! so failed to make new shared
			       memory*/
	    perror("fatal error!: ");
	}
	int shm_cluster = shmget(key+1, sizeof(Point)*clusterNum,
		IPC_CREAT|0666|IPC_EXCL);
	if (shm_cluster == -1){
	    perror("fatal error!: ");
	}
	Point* point_addr = shmat(shm_point, NULL, 0);
	Point* cluster_addr = shmat(shm_cluster, NULL, 0);
	for (int j=0;j<pointNum;j++){
	    scanf("%lf %lf", &(point_addr[j].xpos), &(point_addr[j].ypos));
	    point_addr[j].cluster = -1;
	}
	long before = getTime();
	for (int j=0;j<clusterNum;j++){
	    cluster_addr[j].xpos = point_addr[j].xpos;
	    cluster_addr[j].ypos = point_addr[j].ypos;
	    cluster_addr[j].cluster = -1;
	}
	for (int j=0;j<PROCNUM;j++){/*forking child processes that will execute
				      determine cluster function*/
	    int fn = fork();
	    if (fn == 0){
		char arg_key[100];
		char arg_psize[100];
		char arg_csize[100];
		char arg_index[100];
		char arg_iter[100];
		sprintf(arg_key,"%d",key);
		sprintf(arg_psize, "%d", pointNum);
		sprintf(arg_csize, "%d", clusterNum);
		sprintf(arg_index,"%d",j);
		sprintf(arg_iter, "%d", iterNum);
		execl("./process_child", "./process_child", arg_key, arg_psize,	arg_csize, arg_index, arg_iter, NULL);
	    }
	}
	for (int j=0;j<PROCNUM;j++){/*opening named fifo in parents file*/
	    fd[j] = open(fifoName[j], O_RDONLY);
	    fd[j+PROCNUM] = open(fifoName[j+PROCNUM], O_WRONLY);    
	}

	for (int j=0;j<iterNum;j++){
	    char myBuf[5] = "p";
	    for (int k=0;k<PROCNUM;k++){/*sending signal to child process to
					  start their work by writing on the
					  named fifo*/
		write(fd[PROCNUM+k], myBuf, sizeof(myBuf));
	    }
	    for (int k=0;k<PROCNUM;k++){
		read(fd[k],myBuf,sizeof(myBuf));/*recieving signal from
						  children*/
	    }
	    p_clusterRenewal(point_addr, cluster_addr, pointNum, clusterNum);
	}
	while(wait(&status)>0);
	long after = getTime();
	long consumedTime = after - before;
	printf("Test Case #%d\n", i);
	printf("%ld microseconds\n", consumedTime);
	for (int j=0;j<pointNum;j++){
	    printf("%d\n", point_addr[j].cluster);
	}
	printf("%dth test\n",i) ;
	printf("time: %ld\n", consumedTime);
	shmctl(shm_point, IPC_RMID, 0);/*make shared memory free*/
	shmctl(shm_cluster, IPC_RMID, 0);
	shmdt(point_addr);
	shmdt(cluster_addr);
	for (int j=0;j<2*PROCNUM;j++){/*closing named fifos*/
	   int cs =  close(fd[j]);
	}
    }
    for (int j=0;j<pipenum;j++){
        remove(fifoName[j]);/*removing remaining fifos*/
    }
}
