/**
5.2016
Kernel Principal Component Analysis
Daniel Stodulka
daniel.stodulka.st@vsb.cz
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
Overloaded function for data transform
Either returns new values or saves to file
**/

Eigen::MatrixXd GetData(std::string);

void KRepresentationValues(Eigen::MatrixXd,int,std::string);      // arguments need rework
void KRepresentationVariance(Eigen::MatrixXd,double,std::string);
void KRepresentation(Eigen::MatrixXd,std::string);

Eigen::MatrixXd KRepresentationValues(Eigen::MatrixXd,int);
Eigen::MatrixXd KRepresentationVariance(Eigen::MatrixXd,double);
Eigen::MatrixXd KRepresentation(Eigen::MatrixXd);

#endif // KPCA_H_INCLUDED
