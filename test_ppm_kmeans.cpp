#include "ppm_reader.h"
#include "kmeans.h"
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

    int test_mode = 1;
    bool print_vectors = false;
    // 1 - test header parser

    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i], "--test-header-parser")){
            test_mode = 1;
        }else if(strcmp(argv[i], "--print-vectors")){
            print_vectors = true;
        }
    }

    // Print modes:
    std::cout << "Testing mode: " << test_mode << std::endl;
    std::cout << "Print Vectors: " << print_vectors << std::endl;

    std::string base_path= "/Users/lucasbutler/Desktop/Fall24Materials/581-MachineLearning/Excercises/two/kmeans_imple/images";
    std::vector<std::string> headers = {
        //"bricks.ppm",
        "sun-image.ppm",
        //"test-p3.ppm"
    };

    PPMReader ppm_module;
    KMeans kmeans_module;

    if (test_mode == 0){

        for (const std::string& header : headers) {

            std::filesystem::path full_path = std::filesystem::path(base_path) / header;
                
            // Read in the file as a vector
            int x, y, max = 0;
            std::vector<std::vector<uint8_t>> image_data = ppm_module.read_file(full_path, x, y, max);
            
            std::cout << std::endl;

            if (print_vectors){
                for (auto rgb_triplet : image_data) {
                    // Loop through each element in the inner vector (columns/individual RGB values)
                    std::cout << static_cast<int>(rgb_triplet[0]) << " " 
                    << static_cast<int>(rgb_triplet[1]) << " " 
                    << static_cast<int>(rgb_triplet[2]) << std::endl;
                }   
            }
        }

    }else if (test_mode == 1){

        std::filesystem::path full_path = std::filesystem::path(base_path) / headers[0];

        int x, y, max = 0;
        std::vector<std::vector<uint8_t>> image_data = ppm_module.read_file(full_path, x, y, max);

        std::vector<std::vector<double>> kmeans_data(image_data.size(), std::vector<double>(image_data[0].size()));

        // Convert each element from int to float
        for (size_t i = 0; i < image_data.size(); ++i) {
            for (size_t j = 0; j < image_data[i].size(); ++j) {
                kmeans_data[i][j] = static_cast<double>(image_data[i][j]);
            }
        }

        // Test Calculate distance

        double distance = kmeans_module.calc_distance(kmeans_data[0], kmeans_data[1]);
        for(int i = 0; i < kmeans_data[0].size(); i++){
            std::cout << "p1: " << kmeans_data[0][i] << " p1: " << kmeans_data[1][i] << std::endl;
        }
        std::cout << "Distance: " << distance << std::endl;

        // Verify Cluster init
        printf("Cluster init.\n");
        int clusters = 4;
        std::vector<std::vector<double>> centroids = kmeans_module.initialize_centroids(kmeans_data, clusters);
        for(int i = 0; i < centroids.size(); i++){
            std::cout << " " << centroids[i][0] << " " << centroids[i][1] << " " << centroids[i][2] <<  std::endl;
        }

        // Test Distance Calculation
        printf("Distance calc.\n");
        std::vector<std::vector<double>> distances(kmeans_data.size(), std::vector<double>(centroids.size()));
        kmeans_module.calc_all_distance(kmeans_data, centroids, distances);
        for(int i = 0; i < 10; i++){
            std::cout << " " << distances[i][0] << " " << distances[i][1] << " " << distances[i][2] <<  std::endl;
        }

        // Test Closest Labels
        printf("Assigned labels.\n");
        std::vector<int> cluster_assignments_curr(kmeans_data.size());
        kmeans_module.choose_clusters_from_distances(distances, cluster_assignments_curr);
        for(int i = 0; i < cluster_assignments_curr.size(); i++){
            std::cout << cluster_assignments_curr[i] << " ";

            if (i % 10 == 0)
                std::cout << std::endl;
        }
    }

    return 0;
}