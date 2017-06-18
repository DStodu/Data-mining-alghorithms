/*

KPCA Daniel Stodulka

DISCLAIMER
Haven't found sufficient eigen library - CULA not free (dense eigensolver), cuSOLVER too complicated for my taste - no snipets/examples available, to sum it up no eigen vectors!


KPCA - read data - two examples provided - famous iris data set, variation on swissroll
kernel application - generalized histogram intersection kernel - proved to be quite efficient when dealing with pictures/similar data
kernel center - to achieve zero mean across matrix elements - cublas for matrix mult
all matrices flattened to 1D, 1D grids


tuned for gtm525m - 2 sms, 1536 threads per sm, 8 blocks per sm - 192 threads per block with minimum of 16 blocks for full occupancy 

*/




#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cublas_v2.h>
#include "func.h"
#include <stdio.h>

__global__ void GHI_Kernel(float *r, const float* a, unsigned int size, unsigned int dim, int alfa, int beta)
{

	/*
	 flat matrix 
	 suited for data with low dimensionality
	 every thread performs one step in ghi kernel - loops over all other points, sum of min from every "column"
	 num of rows in matrix = num of threads
	*/

	int i = (threadIdx.x + blockDim.x*blockIdx.x)*dim;
	
	int gridSize = gridDim.x*blockDim.x*dim;
	while (i < size*dim)
	{
		for (int k = 0; k < size*dim; k=k+dim)
		{
			float sum = 0;
			for (int l = 0; l < dim; l++)
			{
				sum += std::fmin(std::pow(std::abs(a[i+l]), alfa), std::pow(std::abs(a[k+l]), beta));
			}
			r[(i / dim)*size + k / dim] = sum;
		}
		
		i+= gridSize;
	}
}

__global__ void MxSubs(float* r, const float* a, const float* b, int size) // matrix sub and add
{
	int i = threadIdx.x + blockDim.x*blockIdx.x;
	while (i < size*size)
	{
		r[i] = a[i] - b[i];
		i += gridDim.x*blockDim.x;
	}
}

__global__ void MxAdd(float* r, const float* a, const float* b, int size)
{
	int i = threadIdx.x + blockDim.x*blockIdx.x;
	while (i < size*size)
	{
		r[i] = a[i] + b[i];
		i += gridDim.x*blockDim.x;
	}
}


void Center(float*,float*,int);
void ApplyKernel(std::vector<std::vector<double>>,double,double,float*);

int main()
{
	cudaDeviceReset();
	std::vector<std::vector<double>> foo = GetData("swiss2.dat");
	float* kernel = new float[foo.size()*foo.size()];
	float* ctKernel = new float[foo.size()*foo.size()];
	ApplyKernel(foo, 2, 2, kernel);
	cudaDeviceReset();
	Center(kernel, ctKernel, foo.size());
	cudaDeviceReset();
	
    
    return 0;
}

void Center(float* gKernel, float* ctKernel, int kSize)
{
	/*** centering kernel - zero sum of elements in kernel matrix ***/
	// ctKernel = gKernel - (OneG) - (gOne) + (OneGone) 
	// gKernel/G/g = kernel matrix
	// One = n*n matrix, values = 1/n

	float* flatOneN = new float[kSize*kSize];
	
	std::fill(flatOneN,flatOneN+kSize*kSize, 1.0 / kSize);

	float* dFlatOneN, *dFlatKernel, *dOneG, *dGOne, *dOneGOne;
	int memSize = sizeof(float)*kSize*kSize;
	const float alpha = 1.0f;
	const float beta = 1.0f;


	cudaMalloc((void**) &dFlatKernel,memSize); 
	cudaMalloc((void**) &dFlatOneN, memSize);
	cudaMemcpy(dFlatKernel, gKernel, memSize, cudaMemcpyHostToDevice);
	cudaMemcpy(dFlatOneN, flatOneN, memSize, cudaMemcpyHostToDevice);

	cudaMalloc((void**) &dOneG, memSize); // results
	cudaMalloc((void**) &dGOne, memSize);
	cudaMalloc((void**) &dOneGOne, memSize);

	cublasHandle_t handle;
	cublasCreate(&handle);

	//cublas matrix mx
	cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, kSize, kSize, kSize, &alpha, dFlatKernel, kSize, dFlatOneN, kSize, &beta , dOneG,kSize);
	cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, kSize, kSize, kSize, &alpha, dFlatOneN, kSize, dFlatKernel, kSize, &beta, dGOne, kSize);
	cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, kSize, kSize, kSize, &alpha, dFlatOneN, kSize, dOneG, kSize, &beta, dOneGOne, kSize);

	int nThreads = 192;
	int nBlocks = (kSize*kSize + nThreads) / nThreads;
	if (nBlocks > 65535)
		nBlocks = 65536;
	if (nBlocks < 16)
		nBlocks = 16;

	MxSubs <<< nBlocks, nThreads >>> (dOneG, dOneG, dGOne, kSize);
	MxSubs <<< nBlocks, nThreads >>> (dFlatKernel, dFlatKernel, dOneG, kSize);
	MxAdd <<< nBlocks, nThreads >>> (dFlatKernel, dFlatKernel, dOneG, kSize);

	cudaMemcpy(ctKernel,dFlatKernel,kSize*kSize,cudaMemcpyDeviceToHost);
}

void ApplyKernel(std::vector<std::vector<double>> dataArray, double alpha, double beta, float* kernel)
{

	float* flatData;
	
	int kSize = dataArray.size();
	int mSize = dataArray[0].size();
	flatData = new float[kSize*mSize];

	float* ptrK = flatData;
	for (int i = 0; i < kSize; i++)
	{
		std::copy(dataArray[i].begin(), dataArray[i].end(), ptrK);
		ptrK += dataArray[i].size();
	}

	float* dGKernel;
	float* dFlatData;
	
	cudaError_t t;
	t = cudaMalloc((void**)&dGKernel, sizeof(float)*kSize*kSize);

	if (t != cudaSuccess)
	{
		std::cout << "Result device alloc\n" << std::endl;
	}

	t = cudaMalloc((void**)&dFlatData,sizeof(float)*kSize*mSize);
	if (t != cudaSuccess)
	{
		std::cout << "flatdata device alloc\n" << std::endl;
	}

	t = cudaMemcpy(dFlatData, flatData, sizeof(float) * kSize * mSize, cudaMemcpyHostToDevice);
	if (t != cudaSuccess)
	{
		std::cout << "Flat memcpy\n" << std::endl;
	}

	int nThreads = 192;
	int nBlocks = (kSize*mSize + nThreads)/nThreads;
	if (nBlocks > 65535)
		nBlocks = 65536;
	if (nBlocks < 16)
		nBlocks = 16;

	GHI_Kernel <<< nBlocks, nThreads >>> (dGKernel,dFlatData,kSize,mSize,alpha,beta);

	t = cudaGetLastError();
	if (t != cudaSuccess)
	{
		std::cout << "GHI failed: " << cudaGetErrorString(t);
	}

	t = cudaMemcpy(kernel, dGKernel, sizeof(float) * kSize*kSize, cudaMemcpyDeviceToHost);
	if (t != cudaSuccess)
	{
		std::cout << "Result memcpy failed!" << std::endl;
	}
	delete [] flatData;
}