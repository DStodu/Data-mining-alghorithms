/**
 * @file func.h
 *
 * @author Daniel Stodulka, dstodu@gmail.com
 *
 * @date 2017
 *
 * @brief Data loading, Generalized histogram intersection kernel cpu implementation
 *  
 * @see http://crsouza.com/2010/03/17/kernel-functions-for-machine-learning-applications/
 */

#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

/**
 * @brief Loads data into vector
 *
 * Loads data into vector of vectors - matrix, accepts csv format with ; as a delimiter.
 *
 * @param filename Source data file filename
 */

std::vector<std::vector<double>> GetData(std::string filename);


/**
 * @brief Generalized histogram intersection kernel
 *
 * Performs kernel function on input matrix, cpu implementation
 *
 * @param matrix
 * @param alpha 
 * @param beta
 */

std::vector<std::vector<double>> GHI(std::vector<std::vector<double>> matrix, double alpha, double beta);