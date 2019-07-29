#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#if (DEBUG)
	#define LOGD(...) printf(__VA_ARGS__)
#else
	#define LOGD(...)  
#endif

struct nodeData{
	int *X;
	int *Y;
	int size;
	unsigned long long result;
};

void init_node_data(struct nodeData** node, int world_rank, int world_size, int tosses){
	*node = (struct nodeData*)malloc(sizeof(struct nodeData));
	(*node)->size = tosses/world_size + ( world_rank!=world_size-1?0:tosses%world_size );
	(*node)->X = (int*)malloc(sizeof(int) * (*node)->size);
	(*node)->Y = (int*)malloc(sizeof(int) * (*node)->size);

	int offset = tosses/world_size;
	int counter=0, currentDataForRank=0;
	for(int i=0;i<tosses;i++){
		int x = rand();
		int y = rand();

		if(world_rank == currentDataForRank){
			(*node)->X[counter] = x;
			(*node)->Y[counter] = y;
		}
		
		counter++;
		if(counter==offset && currentDataForRank!=world_size-1){
			currentDataForRank++;
			counter = 0;
		}
	}
}

void MonteCarlo(struct nodeData* node){
	for(int i=0;i<node->size;i++){
		double x = (double)node->X[i] * 2 / RAND_MAX + (-1);
		double y = (double)node->Y[i] * 2 / RAND_MAX + (-1);
		double dis_sqr = x*x + y*y;
		if(dis_sqr <= 1) node->result++;
	}
}

int main(int argc, char** argv){
	int world_rank, world_size;
	int seed, tosses;
	int seed_constant = 756107;
	int flag_result=0;
	unsigned long long totalCircleCount = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	if(argc < 2){
		if(world_rank==0)printf("Usage: mpiexec -n <total process number> ./<program name> <number of tosses>\n");
		return 0;
	}else{
		tosses = atoi(argv[1]);
		seed = world_size * seed_constant;
		#if (DEBUG)
			seed = 123;
		#endif
		srand(seed);
	}

	struct nodeData *node;
	init_node_data(&node,world_rank,world_size,tosses);
	
	double start, end;
	if(world_rank==0)start = MPI_Wtime();

	MonteCarlo(node);
	LOGD("[rank:%d], size:%d, result:%llu\n",world_rank,node->size,node->result);

	if(world_rank == 0){
		totalCircleCount += node->result;
		unsigned long long otherNodeResult;
		for(int i=1;i<world_size;i++){
			MPI_Recv(&otherNodeResult, 1, MPI_UNSIGNED_LONG_LONG, i, flag_result, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			totalCircleCount += otherNodeResult;
		}
		double pi = ((double)totalCircleCount / (double)tosses)*4;
		end = MPI_Wtime();
		LOGD("cnt = %llu\n",totalCircleCount);
		printf("pi = %lf duration = %lf\n",pi,end-start);
	}else{
		MPI_Send(&node->result, 1, MPI_UNSIGNED_LONG_LONG, 0,  flag_result, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	return 0;
}
