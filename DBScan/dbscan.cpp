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

float Distance(std::vector<int>, std::vector<int>);
std::vector<std::vector<float >> GetDistanceMatrix(std::vector<std::vector<int>>);
std::vector<std::vector<std::vector<int>>> DBScan(std::vector<std::vector<int>>,float,int);
std::vector<int> GetNeighbours(std::vector<std::vector<float >>,float,int);

int main()
{
    std::vector < std::vector <int> > table;

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

    //file.open("output.csv", ios::out | ios::trunc);

    //for(unsigned int i = 0; i < clusters.size(); i++) /** OUTPUT FOR CLUSTERWIZ **/
   /* {
             for(unsigned int j = 0; j < clusters[i].size(); j++)
             {
                 for(unsigned int k = 0; k < clusters[i][j].size(); k++)
                 {
                     file << clusters[i][j].at(k) << ";";
                 }
                 file << i << std::endl;
             }
     }*/

    return 0;
}


float Distance(std::vector<int > a, std::vector<int> b)      /** KEEP THEM AT ARM'S LENGHT LEL **/
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
        if(i%200 == 0)
            std::cout << i << std::endl;
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

    /*fstream file;
    file.open("distance.csv", ios::out | ios::trunc);
    std::cout << "writing to a file\n";
    for(unsigned int i = 0; i < adjMatrix.size(); i++)
    {
        //std::cout << i << std::endl;
        for(unsigned int j = 0; j < adjMatrix[i].size(); j++)
        {
            file << adjMatrix[i][j] << " ";
        }
        file << std::endl;
    }

    file.close();*/

    std::cout << "data done!\n";

    std::vector<int> neighbours;                                              /** NEI INDEXES **/
    std::vector<int> pClusters;                                               /** ALL POINTS IN CLUSTER - JUST INDEXES **/

    unsigned int tSize = table.size();

    std::vector<std::pair<bool,bool>> visited (tSize);                   /** VISITED + NOISE **/

    for(unsigned int i = 0; i < tSize; i++)
    {
        std::vector<int> foo;                                   /** ONE CLUSTER INDEXES **/
        //std::cout << i << std::endl;

        if(visited[i].first)
            continue;

        visited[i].first = true;                                 /** VISITED? GET NEI **/
        neighbours = GetNeighbours(adjMatrix,eps,i);
        //std::cout << neighbours.size() << std::endl;
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
               // std::cout << neighbours.size() << std::endl;
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
