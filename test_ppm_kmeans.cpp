#include "ppm_kmeans.h"
#include <string>
#include <vector>
#include <filesystem>
#include <cstring>

#define DEBUG 1

int main(int argc, char** argv){
    /*
    std::string base_path= "/Users/lucasbutler/Desktop/Fall24Materials/581-MachineLearning/Excercises/two/kmeans_imple/test_headers";
    std::vector<std::string> headers = {
        "example1.txt",
        "example2.txt",
        "example3.txt",
        "example4.txt",
        "example5.txt",
    };
    */

    int test_mode = 0;
    bool print_vectors = false;
    // 1 - test header parser

    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i], "--test-header-parser")){
            test_mode = 1;
        }
        else if(strcmp(argv[i], "--print-vectors")){
            print_vectors = true;
        }
    }

    // Print modes:
    std::cout << "Testing mode" << test_mode << std::endl;
    std::cout << "Print Vectors" << print_vectors << std::endl;

    std::string base_path= "/Users/lucasbutler/Desktop/Fall24Materials/581-MachineLearning/Excercises/two/kmeans_imple/images";
    std::vector<std::string> headers = {
        "bricks.ppm",
        "sun-image.ppm"
    };


    PPMKmeans module;
    for (const std::string& header : headers) {

        std::filesystem::path full_path = std::filesystem::path(base_path) / header;
            
        // Read in the file as a vector
        int x, y, max = 0;
        std::vector<std::vector<int>> image_data;

        bool read_status = module.read_file(full_path, image_data, x, y, max);
        std::cout << std::endl;

        if (print_vectors){
            for (const auto& rgb_triplet : image_data) {
                // Loop through each element in the inner vector (columns/individual RGB values)
                for (const auto& value : rgb_triplet) {
                    std::cout << value << " "; // Print each value separated by a space
                }
                std::cout << std::endl; // New line after each RGB triplet
            }   
        }
        std::cout << "Pixels match image size: " << read_status << std::endl;
    }

    return 0;
}