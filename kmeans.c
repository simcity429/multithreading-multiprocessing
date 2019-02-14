#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sched.h>
#include <sys/time.h>
#include "kmeans.h"
void determineCluster(Point *p, Point *cluster[], int arrSize){
    double distance;
    double min = -1;
    int ret = -1;
    for (int i=0;i<arrSize;i++){
	distance = pow(p->xpos-cluster[i]->xpos,
		2.0)+pow(p->ypos-cluster[i]->ypos, 2.0);
	if (ret == -1){
	    min = distance;
	    ret = i;
	}else{
	    if (min > distance){
		min = distance;
		ret = i;
	    }
	}
    }
    p->cluster = ret;
}

void clusterRenewal(Point* pointArr[], Point* clusterArr[], int pointArrSize,
	int clusterArrSize){
    int tmp = -1;
    for (int i=0;i<clusterArrSize;i++){
	clusterArr[i]->xpos = 0;
	clusterArr[i]->ypos = 0;
	clusterArr[i]->cluster = 0;
    }
    for (int i=0;i<pointArrSize;i++){
	tmp = pointArr[i]->cluster;
	clusterArr[tmp]->xpos += pointArr[i]->xpos;
	clusterArr[tmp]->ypos += pointArr[i]->ypos;
	clusterArr[tmp]->cluster += 1;
    }
    for (int i=0;i<clusterArrSize;i++){
	if (clusterArr[i]->cluster==0){
	    clusterArr[i]->xpos = 100000;
	    clusterArr[i]->ypos = 100000;
	    continue;
	}
	clusterArr[i]->xpos /= clusterArr[i]->cluster;
	clusterArr[i]->ypos /= clusterArr[i]->cluster;
    }
}

void p_determine(Point* p, Point *cluster, int arrSize){
    double distance;
    double min = -1;
    int ret = -1;
    for (int i=0;i<arrSize;i++){
	distance = pow(p->xpos-cluster[i].xpos, 2.0) +
	    pow(p->ypos-cluster[i].ypos, 2.0);
	if (ret == -1){
	    min = distance;
	    ret = i;
	} else {
	    if (min > distance){
		min = distance;
		ret = i;
	    }
	}
    }
    p->cluster = ret;   
}


void p_determineCluster(Point* p, Point* cluster, int startIndex, int lastIndex, int clusterSize){
    for (int i=startIndex;i<=lastIndex;i++){
	p_determine(&(p[i]), cluster, clusterSize);
    }
}

void p_clusterRenewal(Point* pointArr, Point* clusterArr, int pointArrSize,
	int clusterArrSize){
    int tmp = -1;
    for (int i=0;i<clusterArrSize;i++){
	clusterArr[i].xpos = 0;
	clusterArr[i].ypos = 0;
	clusterArr[i].cluster = 0;
    }
    for (int i=0;i<pointArrSize;i++){
	tmp = pointArr[i].cluster;
	clusterArr[tmp].xpos += pointArr[i].xpos;
	clusterArr[tmp].ypos += pointArr[i].ypos;
	clusterArr[tmp].cluster += 1;
    }
    for (int i=0;i<clusterArrSize;i++){
	if (clusterArr[i].cluster ==0){
	    clusterArr[i].xpos = 100000;
	    clusterArr[i].ypos = 100000;
	    continue;
	}
	clusterArr[i].xpos /= clusterArr[i].cluster;
	clusterArr[i].ypos /= clusterArr[i].cluster;
    }
}

void parentsWait(int* flagArr, int size){
    long cnt = 0;
    while(1){
	int sum = 0;
	for (int i=0;i<size;i++){
	    sum += flagArr[i];
	}
	if (sum==size){
	    for (int i=0;i<size;i++){
		flagArr[i] = 0;
	    }
	    break;
	}
	if (cnt > 10000){
	    printf("arr: %d %d %d %ld\n",flagArr[0], flagArr[1], flagArr[2], cnt);
	}
	cnt++;
	sched_yield();
    }
}

long getTime(void){
    struct timeval tv;
    long ret;
    gettimeofday(&tv, 0);
    ret = tv.tv_usec + tv.tv_sec*1000000;
    return ret;
}

void* startRoutine(void *arg){
    PassingArgs* myArg = arg;
    int threadIndex = myArg->threadIndex;
    int psize = myArg->psize;
    int csize = myArg->csize;
    Point** pArr = myArg->pArr;
    Point** cArr = myArg->cArr;
    int startIndex, endIndex, tmp;
    tmp = psize/THNUM;
    startIndex = tmp*threadIndex;
    if (threadIndex == THNUM-1){
	endIndex = psize-1;
    }else{
	endIndex = tmp*(threadIndex+1)-1;
    }
    for (int i=startIndex;i<=endIndex;i++){
	determineCluster(pArr[i], cArr, csize);
    }
    free(myArg);
}

void noparallel_process(int testCase, int iterNum, int clusterNum, int pointNum){
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
	for (int k=0;k<pointNum;k++){
	    determineCluster(pointArr[k], clusterArr, clusterNum);
	}
	clusterRenewal(pointArr, clusterArr, pointNum, clusterNum);
    }
    long after = getTime();
    long consumedTime = after - before;
    printf("Test Case #%d\n",testCase);
    printf("%ld microseconds\n", consumedTime);
    for (int j=0;j<pointNum;j++){
	printf("%d\n", pointArr[j]->cluster);
    }
    printf("time: %ld\n", consumedTime);
    for (int j=0;j<pointNum;j++){
	free(pointArr[j]);
    }
    for (int j=0;j<clusterNum;j++){
	free(clusterArr[j]);
    }
    free(pointArr);
    free(clusterArr);
}

