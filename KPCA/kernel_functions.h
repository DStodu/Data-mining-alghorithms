/**
2.2017
Kernel Principal Component Analysis
Daniel Stodulka
dstodu@gmail.com
Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page) library used for matrix operations
Implemented kernel function can be found here http://crsouza.com/2010/03/17/kernel-functions-for-machine-learning-applications/#source_code
**/

#ifndef KERNEL_FUNCTIONS_H_INCLUDED
#define KERNEL_FUNCTIONS_H_INCLUDED
#include <Eigen/Dense>

/**

Kernel functions implementation + necessary operations

**/

double DotProduct(Eigen::VectorXd,Eigen::VectorXd);
double PointDistanceSquared(Eigen::VectorXd,Eigen::VectorXd);
Eigen::MatrixXd MX(Eigen::MatrixXd, Eigen::MatrixXd);

Eigen::MatrixXd Center_Kernel(Eigen::MatrixXd,int);

Eigen::MatrixXd Linear_Kernel(Eigen::MatrixXd,double, bool);
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
