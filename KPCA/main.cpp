#include "kpca.h"
#include <Eigen/Dense>

using namespace std;


//zmena 2
//zmena 1
//zmena2//zmena1
//zmena 1



int main()
{
    Eigen::MatrixXd foo = GetData("circles.txt");
    Eigen::MatrixXd kernel = Linear_Kernel(foo,1,1);
    KRepresentationVariance(kernel,90,"out.csv");

    return 0;
}

//zmena 2
//zmena 2

