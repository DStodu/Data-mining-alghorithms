#include "kpca.h"
#include <Eigen/Dense>

using namespace std;


int main()
{
    Eigen::MatrixXd foo = GetData("circles.txt");
    Eigen::MatrixXd kernel = Gaussian_Kernel(foo,0.01,1);
    KRepresentationValues(kernel,1,"out.csv");

    return 0;
}

