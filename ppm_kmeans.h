#ifndef PPM_KMEANS
#define PPM_KMEANS

#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

class PPMKmeans {
    // Constructor

    public: 
        PPMKmeans();

        ~PPMKmeans();

        bool read_file(std::string file, std::vector<std::vector<int>> &image_data, int &x, int &y, int &max);

    
}; 

#endif