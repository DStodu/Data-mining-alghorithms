/**
 * @file kernel_functions.h
 *
 * @author Daniel Stodulka, dstodu@gmail.com
 *
 * @date 2016
 *
 * @brief Kernel functions and necessary operations
 *
 * 17 Kernel functions implemented + necessary operations, eigen library used for matrix structures. <br>
 * It is possible to execute all kernels in a simple parallel loop, make outer for parallel - easy implementation in OpenMP for example.
 * 
 * @see http://crsouza.com/2010/03/17/kernel-functions-for-machine-learning-applications/
 * @see http://eigen.tuxfamily.org/index.php?title=Main_Page
 */

#ifndef KERNEL_FUNCTIONS_H_INCLUDED
#define KERNEL_FUNCTIONS_H_INCLUDED
#include <Eigen/Dense>

/**
 * @brief Vectors dot product
 * Computes dot product of two vectors.
 * @param v1 vector 1
 * @param v2 vector 2
 */

double DotProduct(Eigen::VectorXd v1, Eigen::VectorXd v2);

/**
 * @brief Squared Euclidean distance
 * Computes Squared Euclidean distance of two vectors.
 * @param v1 vector 1
 * @param v2 vector 2
 */
 
double PointDistanceSquared(Eigen::VectorXd v1, Eigen::VectorXd v2);

/**
 * @brief Matrix multiplication
 * Computes matrix multiplication, eigen library supports this function  of course, but for some odd reason does not work.
 * @param m1 matrix 1
 * @param m2 matrix 2
 */

Eigen::MatrixXd MX(Eigen::MatrixXd m1, Eigen::MatrixXd m2);

/**
 * @brief Centering kernel
 * Center kernel matrix, to make it zero sum of elements, centered around the origin. <br>
 * Ck = Kernel - OneN*Kernel - KernelOneN - OneNKernelOneN <br>
 * OneN = n*n matrix, where values = 1/n, n = data dimension <br>
 * @param kernel kernel matrix to be centered
 * @param dimension data dimension
 */

Eigen::MatrixXd Center_Kernel(Eigen::MatrixXd kernel,int dimension);

/**
 * @brief Various kernel functions
 * Kernel functions implementation, all follow the same pattern. <br>
 * Every function is different, some of them belong to the similar group. <br>
 * Choosing the right kernel is essential for correct decomposition. <br>
 * Tuning parameters is a very tedious work and can consume vast amount of time. <br>
 * @param  matrix data matrix
 * @param value kernel parameter - alpha, degree, etc.
 * @param center 1/0 to center kernel
 */

Eigen::MatrixXd Linear_Kernel(Eigen::MatrixXd matrix, double value, bool center);
Eigen::MatrixXd Gaussian_Kernel(Eigen::MatrixXd,double, bool);
Eigen::MatrixXd Laplacian_Kernel(Eigen::MatrixXd,double, bool);
Eigen::MatrixXd Polynomial_Kernel(Eigen::MatrixXd,double,double, bool);

Eigen::MatrixXd Sigmoid_Kernel(Eigen::MatrixXd,double,double, bool);

Eigen::MatrixXd Rational_Quadratic_Kernel(Eigen::MatrixXd,double, bool);
Eigen::MatrixXd Multiquadratic_Kernel(Eigen::MatrixXd,double, bool);
Eigen::MatrixXd Inverse_Multiquadratic_Kernel(Eigen::MatrixXd,double, bool);

Eigen::MatrixXd Circular_Kernel(Eigen::MatrixXd,double, bool);
Eigen::MatrixXd Spherical_Kernel(Eigen::MatrixXd,double, bool);
Eigen::MatrixXd Power_Kernel(Eigen::MatrixXd,double, bool);
Eigen::MatrixXd Log_Kernel(Eigen::MatrixXd,double, bool);

Eigen::MatrixXd Cauchy_Kernel(Eigen::MatrixXd,double, bool);

Eigen::MatrixXd Spline_Kernel(Eigen::MatrixXd, bool);

Eigen::MatrixXd Chi_Square(Eigen::MatrixXd, bool);

Eigen::MatrixXd Histogram_Intersection_Kernel(Eigen::MatrixXd, bool);
Eigen::MatrixXd Generalized_Histogram_Intersection_Kernel(Eigen::MatrixXd,double,double, bool);

#endif // KERNEL_FUNCTIONS_H_INCLUDED
