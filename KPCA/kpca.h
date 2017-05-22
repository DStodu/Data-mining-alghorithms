/**
5.2016
Kernel Principal Component Analysis
Daniel Stodulka
dstodu@gmail.com
Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page) library used for matrix operations
**/

#ifndef KPCA_H_INCLUDED
#define KPCA_H_INCLUDED

#include "kernel_functions.h"
#include <fstream>
#include <cmath>
#include <sstream>
#include <iostream>
#include <string>

/**

Loading data
Either returns new values or saves to file
**/

Eigen::MatrixXd GetData(std::string);

void KRepresentationValues(Eigen::MatrixXd,int,std::string);

Eigen::MatrixXd KRepresentationValues(Eigen::MatrixXd,int);

#endif // KPCA_H_INCLUDED
