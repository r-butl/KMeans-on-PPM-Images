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

struct PPMData {
    std::vector<std::vector<uint8_t>> data;
    int width;
    int height;
    int max_value;

    // Constructor to help initialize the vector
    PPMData(int w, int h, int m)
        : data(w * h, std::vector<uint8_t>(PIXEL_DATA_WIDTH_BYTES)), width(w), height(h), max_value(m) {}
};

class PPMReader {
    // Constructor

    public: 
        PPMReader();
        ~PPMReader();

        PPMData read_file(std::filesystem::path &file);

        bool write_file(PPMData picture, std::filesystem::path file_name, bool binary);
    
    private:

        
};  

#endif