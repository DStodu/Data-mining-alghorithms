/**
 * @file kpca.h
 *
 * @author Daniel Stodulka, dstodu@gmail.com
 *
 * @date 2016
 *
 * @brief Basic KPCA functions, loading data, eigen decomposition
 *
 * Implementation of Kernel Principal Component Analysis as described in Dimensionality reduction a comparative review.<br>
 * Eigen library is used for datastructres and eigen decomposition.
 *  
 * @see http://eigen.tuxfamily.org/index.php?title=Main_Page
 * @see https://lvdmaaten.github.io/publications/papers/TR_Dimensionality_Reduction_Review_2009.pdf
 */

#ifndef KPCA_H_INCLUDED
#define KPCA_H_INCLUDED

#include "kernel_functions.h"
#include <fstream>
#include <cmath>
#include <sstream>
#include <iostream>
#include <string>

/**
 * @brief Loading data into eigen matrix structure
 *
 * Loads data into eigen matrix structure, accepts csv format with ; as a delimiter.
 * @param filename Source data file filename
 */
Eigen::MatrixXd GetData(std::string filename);

/**
 * @brief Transforms data using selected number of eigenvalues/vectors
 * Returns new data representation projected on selected eigenvectors.
 * @param kernel Kernel matrix
 * @param vectors Eigenvectors
 * @param components number of vectors to be used
 */

Eigen::MatrixXd TransformData(Eigen::MatrixXd kernel, Eigen::MatrixXd vectors, int components);

/**
 * @brief Saves transformed data into file
 * Returns new data representation projected on selected number of eigenvectors, saving into file in a csv structure.
 * @param kernel Kernel matrix
 * @param components number of vectors to be used
 * @param filename output file name
 */

void KRepresentationValues(Eigen::MatrixXd kernel, int components, std::string filename);

/**
 * @brief Returns transformed data in eigen matrix structure
 *
 * @param kernel Kernel matrix
 * @param components number of vectors to be used
 * 
 */

Eigen::MatrixXd KRepresentationValues(Eigen::MatrixXd kernel, int components);


#endif // KPCA_H_INCLUDED
