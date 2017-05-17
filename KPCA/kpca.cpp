/**
2.2017
Kernel Principal Component Analysis
Daniel Stodulka
daniel.stodulka.st@vsb.cz
Eigen (http://eigen.tuxfamily.org/index.php?title=Main_Page) library used for matrix operations
**/

#include "kpca.h"

Eigen::MatrixXd GetData(std::string filename)  /**  Loads data into Eigen matrix, data formated as .csv with semicolon as a separator **/
{
    std::ifstream file;

    file.open(filename.c_str(), std::ios::in);
    if(!file)
    {
        std::cerr << "File not found!\n";
        exit(-1);
    }

    int lines = 0;

    lines = std::count(std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>(), '\n');
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

Eigen::MatrixXd TransformData(Eigen::MatrixXd gKernel, Eigen::MatrixXd eigenVectors, int components)  /** Returns new data representation based on selected vectors **/
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

void KRepresentationVariance(Eigen::MatrixXd gKernel, double variance, std::string filename)  /** Returns new data representation, eigenvectors are selected by specified variance  **/
{
    if(variance > 1 || variance < 0)
    {
        std::cerr << "Variance > 1 or < 0!" << std::endl;
        exit(-2);
    }

    Eigen::EigenSolver<Eigen::MatrixXd> ces;
    ces.compute(gKernel);

    Eigen::MatrixXd eigenVectors;
    Eigen::VectorXd eigenValues;

    eigenVectors = ces.eigenvectors().real();
    eigenValues = ces.eigenvalues().real();

    float sum = 0;

    for(int i = 0; i < eigenValues.size(); i++)
    {
        sum+=std::abs(eigenValues(i));
    }

    int suggCount = 0;
    double dVariance = 0;

    for(int i = 0; i < eigenValues.size(); i++)
    {
        dVariance+=std::abs(eigenValues(i))/sum;
        suggCount++;
        if(dVariance >= variance)
            break;
    }

    std::cout << "Number of eigenvalues with a variance of at least " << variance*100 << "% : " << suggCount << std::endl;

    int components = suggCount;

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


void KRepresentation(Eigen::MatrixXd gKernel, std::string filename)  /** Returns new data representation, number of eigenvectors is suggested based on 90% variance **/
{
    Eigen::EigenSolver<Eigen::MatrixXd> ces;
    ces.compute(gKernel);

    Eigen::MatrixXd eigenVectors;
    Eigen::VectorXd eigenValues;

    eigenVectors = ces.eigenvectors().real();
    eigenValues = ces.eigenvalues().real();

    float sum = 0;

    for(int i = 0; i < eigenValues.size(); i++)
    {
        sum+=std::abs(eigenValues(i));
    }

    float variance = 0;
    int suggCount = 0;
    int counter = 0;
    int components = 0;

    for(int i = 0; i < eigenValues.size(); i++)
    {
        variance+=std::abs(eigenValues(i))/sum;
        suggCount++;
        counter++;
        if(variance >= 0.9)
            break;
    }

    float suggPlusOne, suggPlusTwo;

    suggPlusOne = eigenValues(counter)/sum;
    suggPlusTwo = eigenValues(counter+1)/sum;

    std::cout << "Suggested number of eigenvalues with a variance of at least 90% :" << suggCount << std::endl;
    std::cout << "Another value would change the variance by: " << suggPlusOne << std::endl;
    std::cout << "Another value would change the variance by: " << suggPlusTwo << std::endl;

    std::cout << "Enter the desired number of eigenvalues: ";
    std::cin >> components;

    if(components <= 0)
    {
        std::cerr << "Negative or zero number of components!" << std::endl;
        exit(-2);
    }


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


void KRepresentationValues(Eigen::MatrixXd gKernel, int components, std::string filename)  /** Returns new data representation, number of used eigenvectors is taken from the function parameter -> user input **/
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

Eigen::MatrixXd KRepresentation(Eigen::MatrixXd gKernel)
{
    Eigen::EigenSolver<Eigen::MatrixXd> ces;
    ces.compute(gKernel);

    Eigen::MatrixXd eigenVectors;
    Eigen::VectorXd eigenValues;

    eigenVectors = ces.eigenvectors().real();
    eigenValues = ces.eigenvalues().real();

    float sum = 0;

    for(int i = 0; i < eigenValues.size(); i++)
    {
        sum+=std::abs(eigenValues(i));
    }

    float variance = 0;
    int suggCount = 0;
    int counter = 0;
    int components = 0;

    for(int i = 0; i < eigenValues.size(); i++)
    {
        variance+=std::abs(eigenValues(i))/sum;
        suggCount++;
        counter++;
        if(variance >= 0.9)
            break;
    }

    float suggPlusOne, suggPlusTwo;

    suggPlusOne = eigenValues(counter)/sum;
    suggPlusTwo = eigenValues(counter+1)/sum;

    std::cout << "Suggested number of eigenvalues with a variance of at least 90% :" << suggCount << std::endl;
    std::cout << "Another value would change the variance by: " << suggPlusOne << std::endl;
    std::cout << "Another value would change the variance by: " << suggPlusTwo << std::endl;

    std::cout << "Enter the desired number of eigenvalues: ";
    std::cin >> components;

    if(components <= 0)
    {
        std::cerr << "Negative or zero number of components!" << std::endl;
        exit(-2);
    }

    Eigen::MatrixXd transformed = Eigen::MatrixXd::Zero(gKernel.rows(),components);
    transformed = TransformData(gKernel,eigenVectors,components);

    return transformed;
}

Eigen::MatrixXd KRepresentationVariance(Eigen::MatrixXd gKernel, double variance)
{
    if(variance > 1 || variance < 0)
    {
        std::cerr << "Variance > 1 or < 0!" << std::endl;
        exit(-2);
    }

    std::vector<std::vector<double>> eVectors;

    Eigen::EigenSolver<Eigen::MatrixXd> ces;
    ces.compute(gKernel);

    Eigen::MatrixXd eigenVectors;
    Eigen::VectorXd eigenValues;

    eigenVectors = ces.eigenvectors().real();
    eigenValues = ces.eigenvalues().real();

    float sum = 0;

    for(int i = 0; i < eigenValues.size(); i++)
    {
        sum+=std::abs(eigenValues(i));
    }

    int suggCount = 0;
    double dVariance = 0;

    for(int i = 0; i < eigenValues.size(); i++)
    {
        dVariance+=std::abs(eigenValues(i))/sum;
        suggCount++;
        if(dVariance >= variance)
            break;
    }

    std::cout << "Number of eigenvalues with a variance of at least " << variance*100 << "% : " << suggCount << std::endl;

    int components = suggCount;

    Eigen::MatrixXd transformed = Eigen::MatrixXd::Zero(gKernel.rows(),components);

    transformed = TransformData(gKernel,eigenVectors,components);

    return transformed;

}
