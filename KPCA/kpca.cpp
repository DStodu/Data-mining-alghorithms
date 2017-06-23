#include "kpca.h"

Eigen::MatrixXd GetData(std::string filename)  
{
    std::fstream file(filename.c_str(), std::fstream::in);
    if(!file)
    {
        std::cerr << "File not found!\n";
        exit(-1);
    }
    int lines = 0;

    lines = std::count(std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>(), '\n'); // count lines and dimension
    file.seekg(0, std::ios::beg);

    int dimension = 0;
    std::string so;

    getline(file,so);
    dimension = std::count(so.begin(), so.end(), ';') + 1;

    file.seekg(0, std::ios::beg);

    Eigen::MatrixXd input(lines,dimension);
    int row = 0;
    while (file)
    {
        std::string s;
        int col = 0;
        if (!getline( file, s )) break;

        std::istringstream ss( s );

        while (ss)
        {
            std::string s;
            if (!getline( ss, s, ';' )) break;
            double value = std::strtod(s.c_str(), NULL);
            input(row,col) = value;
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

Eigen::MatrixXd TransformData(Eigen::MatrixXd gKernel, Eigen::MatrixXd eigenVectors, int components) 
{
    Eigen::MatrixXd transformed = Eigen::MatrixXd::Zero(gKernel.rows(),components);

    int rows = gKernel.rows();

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < components; j++)
        {
            for (int k = 0; k < rows; k++)
            {
                transformed(i,j) += gKernel(i,k) * eigenVectors(k,j);
		 	}
		}
	}
    return transformed;
}


void KRepresentationValues(Eigen::MatrixXd gKernel, int components, std::string filename)  
{
    Eigen::EigenSolver<Eigen::MatrixXd> ces;
    ces.compute(gKernel);						

    Eigen::MatrixXd eigenVectors;
    Eigen::VectorXd eigenValues;

    eigenVectors = ces.eigenvectors().real();
    eigenValues = ces.eigenvalues().real();

    Eigen::MatrixXd transformed = TransformData(gKernel,eigenVectors,components);

    std::fstream file;
    file.open(filename.c_str(),std::ios::out | std::ios::trunc);

    for(int i = 0; i < transformed.rows() ; i++)
    {
        for(int j = 0; j < transformed.cols(); j++)
        {
            file << transformed(i,j) << ";";
        }
        file << std::endl;
    }
    file.close();
}

Eigen::MatrixXd KRepresentationValues(Eigen::MatrixXd gKernel, int components) 
{
    Eigen::EigenSolver<Eigen::MatrixXd> ces;
    ces.compute(gKernel);

    Eigen::MatrixXd eigenVectors;
    Eigen::VectorXd eigenValues;

    eigenVectors = ces.eigenvectors().real();
    eigenValues = ces.eigenvalues().real();

    Eigen::MatrixXd transformed = Eigen::MatrixXd::Zero(gKernel.rows(),components);
    transformed = TransformData(gKernel,eigenVectors,components);

    return transformed;
}