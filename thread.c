#include <stdio.h>
#include <stdlib.h>	
#include <pthread.h>
#include "kmeans.h"

int main(void){
    pthread_t tid[THNUM];
    int testCase;
    int iterNum;
    int clusterNum;
    int pointNum;
    void* (*myfunc) (void*) = startRoutine;
    scanf("%d", &testCase);
    for (int i=0;i<testCase;i++){
	scanf("%d", &iterNum);
	scanf("%d", &clusterNum);
	scanf("%d", &pointNum);
	if(pointNum<NOPARAL_NUM){
	    noparallel_process(i, iterNum, clusterNum, pointNum);
	    continue;
	}
	Point** pointArr = (Point**)malloc(sizeof(Point*)*pointNum);
	Point** clusterArr = (Point**)malloc(sizeof(Point*)*clusterNum);
	for (int j=0;j<pointNum;j++){
	    pointArr[j] = (Point*)malloc(sizeof(Point));
	    scanf("%lf %lf", &pointArr[j]->xpos, &pointArr[j]->ypos);
	}
	long before = getTime();
	for (int j=0;j<clusterNum;j++){
	    clusterArr[j] = (Point*)malloc(sizeof(Point));
	    clusterArr[j]->xpos = pointArr[j]->xpos;
	    clusterArr[j]->ypos = pointArr[j]->ypos;
	}
	for (int j=0;j<iterNum;j++){
	    for (int k=0;k<THNUM;k++){
         	PassingArgs* pa =
		    (PassingArgs*)malloc(sizeof(PassingArgs));/*structure for
								passing
								argument for
								thread*/
		pa->threadIndex = k;
		pa->psize = pointNum;
		pa->csize = clusterNum;
		pa->pArr = pointArr;
		pa->cArr = clusterArr;
		pthread_create(&(tid[k]), NULL, myfunc, (void*)(pa));/*making
								       threads
								       by
								       passing
								       arguments
								       using
								       argument
								       structure*/
								       
	    }
	    for (int k=0;k<THNUM;k++){
		pthread_join(tid[k], NULL);/*waiting for all thread to be
					     done*/
	    }
	    clusterRenewal(pointArr, clusterArr, pointNum, clusterNum);
	}
	long after = getTime();
	long consumedTime = after-before;
	printf("Test Case #%d\n", i);
	printf("%ld microseconds\n", consumedTime);
	for (int j=0;j<pointNum;j++){
	    printf("%d\n", pointArr[j]->cluster);
	}
	printf("time: %ld\n", consumedTime);
	for(int j=0;j<pointNum;j++){
	    free(pointArr[j]);
	}
	for(int j=0;j<clusterNum;j++){
	    free(clusterArr[j]);
	}
	free(pointArr);
	free(clusterArr);
    }
}
