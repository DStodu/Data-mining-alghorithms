/*! \mainpage DBScan
 * Implementation of DBScan.<br>
 * Testing data various sizes and dimensions. <br>
 * Using STL containers, not that effective as speed goes, but surely does the job and is pretty straight forward.<br>
 * 
 */

/**
 * @file dbscan.cpp
 *
 * @author Daniel Stodulka, dstodu@gmail.com
 *
 * @date 2016
 *
 * @brief DBSCAN
 *
 * Implemented using STL containers.<br>
 * Consist of loading data (csv with a ;), finding neighbours within epsilon range and expanding.<br>
 * Since no centroids are used, distance matrix is used to store distances between points.<br>
 * 
 * 
 * @see https://en.wikipedia.org/wiki/DBSCAN#Algorithm
 */
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <random>
#include <cfloat>
#include <time.h>

using namespace std;

/**
 * @brief Euclidian distance
 *
 * 
 * @param v1 vector1
 * @param v2 vector2
 * @return Euclidian distance
 */

float Distance(std::vector<int> v1, std::vector<int> v2);

/**
 * @brief Creates distance matrix
 *
 * Matrix of distances between points
 * @param table points to be clustered
 * @return Distance matrix
 */	

std::vector<std::vector<float >> GetDistanceMatrix(std::vector<std::vector<int>> table);


/**
 * @brief DBSCAN
 *
 * Performs DBSCAN 
 * @param table points to be clustered
 * @param epsilon neighborhood range
 * @param minPT minimum number of points (within eps) to form a cluster
 * @return Clusters of points
 */	

std::vector<std::vector<std::vector<int>>> DBScan(std::vector<std::vector<int>> table, float epsilon,int minPT);

/**
 * @brief Neighbours of a given point
 *
 * Gets all the neighbours in neighborhood range given by epsilon
 * @param distanceMatrix Distance Matrix
 * @param epsilon neighborhood range
 * @param minPT minimum number of points (within eps) to form a cluster
 * @return Vector of neighbour indexes
 */	
std::vector<int> GetNeighbours(std::vector<std::vector<float>> distanceMatrix, float eps, int i);

/**
 * @brief Sample DBSCAN usage
 *
 * DBSCAN usage with data loading and outputs number of points in every cluster
 *
 * @return status code
 */	


int main()
{
    std::vector<std::vector <int>> table;

    fstream file;
    int value;

    srand(time(NULL));

    file.open("dim/dim5.txt", ios::in);
    if(!file)
        cout << "file not found etc...\n";

    while (file)
    {
        string s;
        if (!getline( file, s )) break;

        istringstream ss( s );
        vector <int> record;

        while (ss)
        {
          string s;
          if (!getline( ss, s, ';' )) break;
          value = atoi(s.c_str());
          if(value != 0)
            record.push_back( value );
        }

        table.push_back( record );
    }

    if (!file.eof())
    {
        cerr << "Fooey!\n";
    }

     std::vector<std::vector<std::vector<int>>> clusters = DBScan(table,50000,15);

     /** 50k for 10 dim **/

     for(unsigned int i = 0; i < clusters.size(); i++)
     {
            std::cout << clusters[i].size() << "\t" << i << std::endl;
     }

    file.close();

    return 0;
}


float Distance(std::vector<int > a, std::vector<int> b)     
{
    float distance = 0;
    unsigned int vSize = a.size();
    for(unsigned int i = 0; i < vSize; i++)
    {
        float foo = a[i] - b[i];
        foo*= foo;
        distance += foo;
    }
    return sqrt(distance);
}

std::vector<std::vector<float>> GetDistanceMatrix(std::vector<std::vector<int>> table)        /** DISTANCE MATRIX NOT REALLY FANCY BUT'LL HAVE TO DO **/
{
    std::vector<std::vector<float>> adjMatrix;
    adjMatrix.resize(table.size());

    unsigned int tSize = table.size();
    unsigned int mSize = adjMatrix.size();

    for(unsigned int i = 0; i < mSize; i++)
    {
        adjMatrix[i].resize(tSize);
        for(unsigned int j = 0; j < mSize; j++)
        {
            adjMatrix[i][j] = (Distance(table[i],table[j]));
        }
    }
    return adjMatrix;
}

std::vector<std::vector<std::vector<int>>> DBScan(std::vector<std::vector<int>> table, float eps, int minPts)
{
    std::vector<std::vector<std::vector<int>>> clusters;                     /** FINAL CLUSTERS **/
    std::vector<std::vector<float>> adjMatrix = GetDistanceMatrix(table);      /** DISTANCE MATRIX **/

    std::cout << "data done!\n";

    std::vector<int> neighbours;                                              /** NEI INDEXES **/
    std::vector<int> pClusters;                                               /** ALL POINTS IN CLUSTER - JUST INDEXES **/

    unsigned int tSize = table.size();

    std::vector<std::pair<bool,bool>> visited (tSize);                   /** VISITED + NOISE **/

    for(unsigned int i = 0; i < tSize; i++)
    {
        std::vector<int> foo;                                   /** ONE CLUSTER INDEXES **/

        if(visited[i].first)
            continue;

        visited[i].first = true;                                 /** VISITED? GET NEI **/
        neighbours = GetNeighbours(adjMatrix,eps,i);

        if(neighbours.size() < (unsigned)minPts)                    /** NOISE? **/
            visited[i].second = true;

        if(!visited[i].second)                                   /** NOT NOISE? PUSH THE BASTARD IN **/
        {
            foo.push_back(i);
            pClusters.push_back(i);
        }
        else
        {
            std::cout << "noise: " << i << std::endl;
            std::cout << neighbours.size() << std::endl;
        }

        for(unsigned int j = 0; j < neighbours.size(); j++)         /** EVEN MORE STUFF **/
        {
            if(!visited.at(neighbours[j]).first)                 /** NOT VISITED? EXPAND! **/
            {
                visited.at(neighbours[j]).first = true;
                std::vector<int> neighbours2 = GetNeighbours(adjMatrix,eps,neighbours[j]);     /** EVERYONE GET IN HERE! **/
                if(neighbours2.size() >= (unsigned)minPts)
                {
                    for(unsigned int k = 0; k < neighbours2.size(); k++)                            /** BIG ENOUGH? NOT ALREADY IN NEIG? **/
                    {
                        if(std::find(neighbours.begin(),neighbours.end(),neighbours2[k]) == neighbours.end())
                            neighbours.push_back(neighbours2[k]);                                /** PUSH THE BASTARD **/
                    }
                }
            }

            /** Add p' to cluster if not already in one **/
            if(find(pClusters.begin(),pClusters.end(),neighbours[j]) == pClusters.end())         /** NOT IN CLUSTER? PUSH THE BASTARD **/
            {
                foo.push_back(neighbours[j]);
                pClusters.push_back(neighbours[j]);
            }
        }
        std::vector<std::vector<int>> fooClusters;
        clusters.push_back(fooClusters);
        unsigned int cSize = foo.size();
        for(unsigned int j = 0; j < cSize; j++)                                                /** TAKE ALL INDEXES AND PUSH THEM INTO ONE CLUSTER **/
        {
            clusters[clusters.size()-1].push_back(table[foo[j]]);
        }
    }
    return clusters;
}

std::vector<int> GetNeighbours(std::vector<std::vector<float>> adjMatrix, float eps, int pIndex)  /** EVERYONE GET IN HERE! **/
{
    std::vector<int> neiIndex;
    unsigned int mSize = adjMatrix[pIndex].size();
    for(unsigned int i = 0; i < mSize; i++)
    {
        if(adjMatrix[pIndex][i] <= eps)
            neiIndex.push_back(i);
    }
    return neiIndex;
}
