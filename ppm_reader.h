#ifndef PPM_READER
#define PPM_READER

#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#define PIXEL_DATA_WIDTH_BYTES 3

class PPMReader {
    // Constructor

    public: 
        PPMReader();

        ~PPMReader();

        std::vector<std::vector<uint8_t>> read_file(std::string file, int &x, int &y, int &max);
    
    private:

        
};  

#endif