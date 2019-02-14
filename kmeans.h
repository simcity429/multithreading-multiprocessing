#define THNUM 5
#define PROCNUM 5
#define KEY 429
#define NOPARAL_NUM 10

typedef struct{
    double xpos;
    double ypos;
    int cluster;
}Point;

typedef struct{
    int threadIndex;
    int psize;
    int csize;
    Point** pArr;
    Point** cArr;
}PassingArgs;

void determineCluster(Point *p, Point *cluster[], int arrSize);
void clusterRenewal(Point* pointArr[], Point* clusterArr[], int pointArrSize,
	int clusterArrSize);

void p_determine(Point* p, Point* cluster, int arrSize);

void p_determineCluster(Point* p, Point* cluster, int startIndex, int
	lastIndex, int clusterSize);

void p_clusterRenewal(Point* pointArr, Point* clusterArr, int pointArrSize, int
	clusterArrSize);

void parentsWait(int* flagArr, int size);

long getTime(void);

void* startRoutine(void *arg);

void noparallel_process(int testCase, int iterNum, int clusterNum, int pointNum);
