#ifndef PPM_KMEANS
#define PPM_KMEANS

#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#define PIXEL_DATA_WIDTH_BYTES 3

class PPMKmeans {
    // Constructor

    public: 
        PPMKmeans();

        ~PPMKmeans();

        std::vector<std::vector<uint8_t>> read_file(std::string file, int &x, int &y, int &max);

    
}; 

#endif