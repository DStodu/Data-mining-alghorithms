#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

std::vector<std::vector<double>> GetData(std::string);

std::vector<std::vector<double>> GHI(std::vector<std::vector<double>>, double, double);