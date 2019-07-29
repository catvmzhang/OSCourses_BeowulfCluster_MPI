# OSCourses_BeowulfCluster_MPI

In this project, we are going to practice how to build the Beowulf cluster, using it to execute a
program in parallel, and we will write our own parallel program with MPICH. 

## Beowulf cluster
Beowulf cluster which use mpi as the communication channel. 
1.  Network File System in each nodes
2.  Create a directory in master and share the directory to slaves
3. Setup ssh configurations in each node to make sure that master can access slaves without password-checking. 
4.  Install MPICH in each node. 
here is a good tutorial: http://mpitutorial.com/tutorials/running-an-mpi-cluster-within-a-lan/
* mpicc (compiler for mpich)
  * compile: mpicc mpi_hello.c -o mpi_hello 
  * execute: mpiexec -n <total cpu number> -host master,slave1,slave2 ./mpi_hello
    *  -n means the number of cores to ask for. 
  
## Pi calculation
algorithm: Monte Carlo method.
concept: using random numbers and the formula of the area. 

Suppose there is a square with a side length of two, and a circle insides the square with a radius of one.
Suppose the number of the darts we toss is N(Darts will always hit the square), and the number of darts inside the circle is C. So we can have the following equation:
```
C:N = Pi:4 => Pi = 4 * C / N 
```
tutorial: https://openhome.cc/Gossip/AlgorithmGossip/MathPI.htm
