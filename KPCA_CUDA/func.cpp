#include "func.h"

std::vector<std::vector<double>> GetData(std::string filename)
{
	std::ifstream file;

	file.open(filename.c_str(), std::ios::in);
	if (!file)
	{
		std::cerr << "File not found!\n";
		exit(-1);
	}

	int lines = 0;

	lines = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
	file.seekg(0, std::ios::beg);

	int dimension = 0;
	std::string so;

	getline(file, so);
	dimension = std::count(so.begin(), so.end(), ';') + 1;

	file.seekg(0, std::ios::beg);

	std::vector<std::vector<double>> input;
	input.resize(lines, std::vector<double>(dimension));

	int row = 0;
	while (file)
	{
		if(row % 100 == 0)
			std::cout << "row: " << row << std::endl;
		std::string s;
		int col = 0;
		if (!getline(file, s)) break;

		std::istringstream ss(s);

		while (ss)
		{
			std::string s;
			if (!getline(ss, s, ';')) break;
			double value = std::strtod(s.c_str(), NULL);
			input[row][col] = value;
			col++;
		}
		row++;
	}

	if (!file.eof())
	{
		std::cerr << "Wrong file format!\n";
		exit(-1);
	}

	file.close();

	return input;

}


std::vector<std::vector<double>> GHI(std::vector<std::vector<double>> dataArray, double alpha, double beta)
{
	int rows = dataArray.size();
	std::vector<std::vector<double>> gKernel;
	gKernel.resize(rows, std::vector<double>(rows));

	for (int i = 0; i < rows; i++)
	{
		if(i% 100 == 0)
		std::cout << i << std::endl;
		for (int j = 0; j < rows; j++)
		{
			double sum = 0;
			for (int k = 0; k < dataArray[0].size(); k++)
				sum += std::min(std::pow(std::abs(dataArray[i][k]), alpha), std::pow(std::abs(dataArray[j][k]), beta));

			gKernel[i][j] = sum;
		}
	}

	return gKernel;
}