/**
2.2017
Kernel Principal Component Analysis
Daniel Stodulka
daniel.stodulka.st@vsb.cz
Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page) library used for matrix operations
**/

#include "kernel_functions.h"
#include <iostream>


//zmena 1

double DotProduct(Eigen::VectorXd vec1, Eigen::VectorXd vec2)
{
    double sum = 0;
    for(int i = 0; i < vec1.rows(); i++)
        sum+=vec1(i)*vec2(i);

    return sum;
}

//zmena 2
double PointDistanceSquared(Eigen::VectorXd vec1, Eigen::VectorXd vec2)
{
    double sum = 0;
    int vecSize = vec1.rows();
    for(int i = 0; i < vecSize; i++)
    {
        sum+= pow(vec1(i)-vec2(i),2);
    }
    return sum;
}

Eigen::MatrixXd Center_Kernel(Eigen::MatrixXd gKernel, int dimensions)
{
    Eigen::MatrixXd oneN = Eigen::MatrixXd::Constant(gKernel.rows(), gKernel.rows(), 1.0/dimensions);
    Eigen::MatrixXd centeredK = gKernel - (oneN*gKernel) - (gKernel*oneN) + (oneN*gKernel)*oneN;
    oneN.resize(0,0);
    gKernel.resize(0,0);
    return centeredK;
}

Eigen::MatrixXd Linear_Kernel(Eigen::MatrixXd dataArray, double constant, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            double sum = 0;
            for(int i = 0; i < dataArray.row(i).rows(); i++)
                sum+=dataArray.row(i)(i)*dataArray.row(j)(i);
            gKernel(i,j) = sum + constant;
        }
    }
    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Gaussian_Kernel(Eigen::MatrixXd dataArray, double sigma, bool center)
{
    sigma*=-1.0;
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            gKernel(i,j) = std::exp(sigma*(PointDistanceSquared(dataArray.row(i),dataArray.row(j))));
        }
    }
    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}


Eigen::MatrixXd Laplacian_Kernel(Eigen::MatrixXd dataArray, double sigma, bool center)
{
    sigma*=-1.0;
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            gKernel(i,j) = std::exp(-1*(std::sqrt(PointDistanceSquared(dataArray.row(i),dataArray.row(j)))/sigma));
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}


Eigen::MatrixXd Polynomial_Kernel(Eigen::MatrixXd dataArray, double constant, double degree, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            gKernel(i,j) = std::pow(DotProduct(dataArray.row(i),dataArray.row(j)) + constant,degree);
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;

}

Eigen::MatrixXd Sigmoid_Kernel(Eigen::MatrixXd dataArray, double constant, double alpha, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            gKernel(i,j) = std::tanh(alpha*DotProduct(dataArray.row(i),dataArray.row(j)) + constant);
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Rational_Quadratic_Kernel(Eigen::MatrixXd dataArray, double constant, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            double fooDist = PointDistanceSquared(dataArray.row(i),dataArray.row(j));
            gKernel(i,j) = 1.0 - (fooDist/(fooDist+constant));
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Multiquadratic_Kernel(Eigen::MatrixXd dataArray, double constant, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            gKernel(i,j) = (sqrt(PointDistanceSquared(dataArray.row(i),dataArray.row(j))+(constant*constant)));
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Inverse_Multiquadratic_Kernel(Eigen::MatrixXd dataArray, double constant, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);


    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            gKernel(i,j) = 1.0/(std::sqrt(PointDistanceSquared(dataArray.row(i),dataArray.row(j))+(constant*constant)));
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Circular_Kernel(Eigen::MatrixXd dataArray, double sigma, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            double fooDist = std::sqrt(PointDistanceSquared(dataArray.row(i),dataArray.row(j)));

            if(fooDist > sigma)
                gKernel(i,j) = 0;
            else
                gKernel(i,j) = (2.0/M_PI)*std::acos(-fooDist/sigma) - (2.0/M_PI)*(fooDist/sigma)*sqrt(1.0 - pow(fooDist/sigma,2));
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Spherical_Kernel(Eigen::MatrixXd dataArray, double sigma, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            double fooDist = std::sqrt(PointDistanceSquared(dataArray.row(i),dataArray.row(j)));

            if(fooDist > sigma)
                gKernel(i,j) = 0;
            else
                gKernel(i,j) = 1 - 1.5*(fooDist/sigma)+ 0.5*std::pow((fooDist/sigma),3);
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Power_Kernel(Eigen::MatrixXd dataArray, double degree, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            gKernel(i,j) = -1.0 * std::pow(std::sqrt(PointDistanceSquared(dataArray.row(i),dataArray.row(j))),degree);
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Log_Kernel(Eigen::MatrixXd dataArray, double degree, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
                gKernel(i,j) = -1.0 * std::log(std::pow(std::sqrt(PointDistanceSquared(dataArray.row(i),dataArray.row(j))),degree) + 1);
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
} 

Eigen::MatrixXd Cauchy_Kernel(Eigen::MatrixXd dataArray,double sigma, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            gKernel(i,j) = 1.0 / (1.0 + (PointDistanceSquared(dataArray.row(i),dataArray.row(j))/ (sigma*sigma)));
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Spline_Kernel(Eigen::MatrixXd dataArray, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            double prod = 1;
            for(int k = 0; k < dataArray.cols(); k++)
            {
                double fooMin = std::min(dataArray(i,k),dataArray(j,k));
                prod*= (1 + (dataArray(i,k)*dataArray(j,k)) + dataArray(i,k)*dataArray(j,k)*fooMin) - ((dataArray(i,k)+dataArray(j,k))/2)*pow(fooMin,2) + pow(fooMin,3)/3;
            }

            gKernel(i,j) = prod;
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}


Eigen::MatrixXd Chi_Square(Eigen::MatrixXd dataArray, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);


    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            double sum = 0;
            for(int k = 0; k < dataArray.cols(); k++)
                sum+= (2*(dataArray(i,k)*dataArray(j,k)))/(dataArray(i,k)+dataArray(j,k));

            gKernel(i,j) = sum;
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}


Eigen::MatrixXd Histogram_Intersection_Kernel(Eigen::MatrixXd dataArray, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            double sum = 0;
            for(int k = 0; k < dataArray.cols(); k++)
                sum+= std::min(dataArray(i,k),dataArray(j,k));

            gKernel(i,j) = sum;
        }
    }

    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}

Eigen::MatrixXd Generalized_Histogram_Intersection_Kernel(Eigen::MatrixXd dataArray, double alpha, double beta, bool center)
{
    int rows = dataArray.rows();
    Eigen::MatrixXd gKernel(rows,rows);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            double sum = 0;
            for(int k = 0; k < dataArray.cols(); k++)
                sum+= std::min(std::pow(std::abs(dataArray(i,k)),alpha),std::pow(std::abs(dataArray(j,k)),beta));

            gKernel(i,j) = sum;
        }
    }
    if(center)
    {
        Eigen::MatrixXd ret = Center_Kernel(gKernel,dataArray.rows());
        return ret;
    }
    else
        return gKernel;
}
