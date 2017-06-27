/*! \mainpage KPCA CUDA
 * Kernel function implementation on CUDA<br>
 * Haven't found sufficient eigen library - CULA not free (dense eigensolver), cuSOLVER too complicated for my taste - no snipets/examples available.<br>
 * Generalized Histogram Intersection kernel is used. <br>
 * 
 * Tuned for gtm525m - 2 sms, 1536 threads per sm, 8 blocks per sm - 192 threads per block with minimum of 16 blocks for full occupancy. 
 * 
 */


/**
 * @file kernel.cu
 *
 * @author Daniel Stodulka, dstodu@gmail.com
 *
 * @date 2017
 *
 * @brief Basic KPCA functions, loading data, eigen decomposition
 *
 * Kernel implementation on CUDA, eigen decomposition is not included, one kernel function - Generalized Histogram Intersection kernel.<br>
 * 
 * @see http://crsouza.com/2010/03/17/kernel-functions-for-machine-learning-applications/
 */

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cublas_v2.h>
#include "func.h"
#include <stdio.h>

/**
 * @brief GHI on CUDA kernel
 *
 * Computes kernel on CUDA.<br> 
 * Matrices flattened to 1D<br>
 * Simple grid-stride for loop, over all elements, every thread acts as a single point in computation.
 * Suited for data with low dimensionality.
 *
 * @param r result kernel matrix
 * @param a input data matrix
 * @param size data size
 * @param dim data dimension
 * @param alpha
 * @param beta
 */


__global__ void GHI_Kernel(float *r, const float* a, unsigned int size, unsigned int dim, int alpha, int beta)
{
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

/**
 * @brief Matrix substraction on CUDA
 *
 * @param r result matrix
 * @param a 
 * @param b 
 * @param size data size
 */

__global__ void MxSubs(float* r, const float* a, const float* b, int size)
{
	int i = threadIdx.x + blockDim.x*blockIdx.x;
	while (i < size*size)
	{
		r[i] = a[i] - b[i];
		i += gridDim.x*blockDim.x;
	}
}

/**
 * @brief Matrix addition on CUDA
 *
 * @param r result matrix
 * @param a 
 * @param b 
 * @param size data size
 */

__global__ void MxAdd(float* r, const float* a, const float* b, int size)
{
	int i = threadIdx.x + blockDim.x*blockIdx.x;
	while (i < size*size)
	{
		r[i] = a[i] + b[i];
		i += gridDim.x*blockDim.x;
	}
}


/**
 * @brief Kernel matrix centering
 * Center kernel matrix, to make it zero sum of elements, centered around the origin. <br>
 * Ck = Kernel - OneN*Kernel - KernelOneN - OneNKernelOneN <br>
 * OneN = n*n matrix, where values = 1/n, n = data dimension <br>
 * Uses cublas library for matrix multiplication.
 * 
 * @param r result matrix
 * @param a kernel matrix to center
 * @param size data size
 */

void Center(float* r,float* a, int size);

/**
 * @brief Applies kernel function on data
 *
 * Flattens input data, allocates space on device and applies kernel function.
 *
 * @param data data matrix
 * @param alpha 
 * @param beta 
 * @param r result matrix
 */

void ApplyKernel(std::vector<std::vector<double>> data,double alpha,double beta, float* r);

/**
 * @brief GHI kernel example usage
 *
 *	Loads data, performs GHI kernel and also kernel centering.
 *
 */

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

	cudaMalloc((void**) &dOneG, memSize);
	cudaMalloc((void**) &dGOne, memSize);
	cudaMalloc((void**) &dOneGOne, memSize);

	cublasHandle_t handle;
	cublasCreate(&handle);

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