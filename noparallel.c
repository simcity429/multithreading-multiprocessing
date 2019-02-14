#include <stdio.h>
#include <stdlib.h>
#include "kmeans.h"



int main(void){
    int testCase;
    int iterNum;
    int clusterNum;
    int pointNum;
    Point** pointArr;
    Point** clusterArr;
    scanf("%d", &testCase);
    for(int i=0;i<testCase;i++){
	scanf("%d", &iterNum);
	scanf("%d", &clusterNum);
	scanf("%d", &pointNum);
	noparallel_process(i, iterNum, clusterNum, pointNum);
    }
    return 0;
}
