#include "ppm_reader.h"
#include "kmeans.h"
#include <string>
#include <vector>
#include <filesystem>
#include <cstring>

#define DEBUG 1

uint8_t convertDoubleToUInt8(double value) {
    // Clamp the value to the range [0, 255]
    if (value < 0) {
        return 0;
    } else if (value > 255) {
        return 255;
    }
    return static_cast<uint8_t>(std::round(value)); // Use round to avoid truncation
}

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

    int test_mode = 4;
    bool print_vectors = false;
    int max_iters = 50;
    int num_clusters = 5;
    float epsilon = 0;

    // Print modes:
    std::cout << "Testing mode: " << test_mode << std::endl;
    std::cout << "Print Vectors: " << print_vectors << std::endl;

    std::string base_path= "/Users/lucasbutler/Desktop/Fall24Materials/581-MachineLearning/Excercises/two/kmeans_imple/images";
    std::vector<std::string> headers = {
        "bricks.ppm",
        "sun-image.ppm",
        "test-p3.ppm"
    };

    PPMReader ppm_module;
    KMeans kmeans_module;

    if (test_mode == 0){

        for (const std::string& header : headers) {

            std::filesystem::path full_path = std::filesystem::path(base_path) / header;
                
            // Read in the file
            PPMData image_data = ppm_module.read_file(full_path);
            
            std::cout << std::endl;

            if (print_vectors){
                for (auto rgb_triplet : image_data.data) {
                    // Loop through each element in the inner vector (columns/individual RGB values)
                    std::cout << static_cast<int>(rgb_triplet[0]) << " " 
                    << static_cast<int>(rgb_triplet[1]) << " " 
                    << static_cast<int>(rgb_triplet[2]) << std::endl;
                }   
            }
        }

    }else if (test_mode == 1){

        std::filesystem::path full_path = std::filesystem::path(base_path) / headers[0];

        PPMData image_data = ppm_module.read_file(full_path);

        std::vector<std::vector<double>> kmeans_data(image_data.data.size(), std::vector<double>(image_data.data[0].size()));

        // Convert each element from int to float
        for (size_t i = 0; i < image_data.data.size(); ++i) {
            for (size_t j = 0; j < image_data.data[i].size(); ++j) {
                kmeans_data[i][j] = static_cast<double>(image_data.data[i][j]);
            }
        }

        // Test Calculate distance

        double distance = kmeans_module.calc_distance(kmeans_data[0], kmeans_data[1]);
        for(int i = 0; i < kmeans_data[0].size(); i++){
            std::cout << "p1: " << kmeans_data[0][i] << " p1: " << kmeans_data[1][i] << std::endl;
        }
        std::cout << "Distance: " << distance << std::endl << std::endl;

        // Verify Cluster init
        printf("Cluster init.\n");
        int clusters = 4;
        std::vector<std::vector<double>> centroids = kmeans_module.initialize_centroids(kmeans_data, clusters);
        for(int i = 0; i < centroids.size(); i++){
            std::cout << " " << centroids[i][0] << " " << centroids[i][1] << " " << centroids[i][2] <<  std::endl;
        }
        std::cout << std::endl << std::endl;

        // Test Distance Calculation
        printf("Distance calc.\n");
        std::vector<std::vector<double>> distances(kmeans_data.size(), std::vector<double>(centroids.size()));
        kmeans_module.calc_all_distance(kmeans_data, centroids, distances);
        for(int i = 0; i < 10; i++){
            std::cout << " " << distances[i][0] << " " << distances[i][1] << " " << distances[i][2] <<  std::endl;
        }
        std::cout << std::endl << std::endl;

        // Test Closest Labels
        printf("Assigned labels.\n");
        std::vector<int> cluster_assignments_curr(kmeans_data.size());
        kmeans_module.choose_clusters_from_distances(distances, cluster_assignments_curr);
        for(int i = 0; i < 100; i++){
            if (i % 10 == 9)
                std::cout << std::endl; std::cout << " ";

            std::cout << cluster_assignments_curr[i] << " ";

        }
        
        std::cout << std::endl << std::endl;


        // Run KMeans
        std::vector<std::vector<double>> clustered_data = kmeans_module.run(kmeans_data, 4, 50, 0);
        for(int i = 0; i < 10; i++){
            std::cout << " " << clustered_data[i][0] << " " << clustered_data[i][1] << " " << clustered_data[i][2] <<  std::endl;
        }

    } else if (test_mode == 2){


        PPMData test(12, 23, 12);

        ppm_module.write_file(test, "test.ppm", true);      



    } else if (test_mode == 3){

        std::filesystem::path file = "test.ppm";
        PPMData test = ppm_module.read_file(file);

        std::cout << test.height << " " << test.width << " " << test.max_value << std::endl;

    } else if (test_mode == 4){

        std::filesystem::path file = "images/bricks.ppm";
        PPMData image = ppm_module.read_file(file);
                
        std::vector<std::vector<double>> kmeans_data(image.data.size(), std::vector<double>(image.data[0].size()));

        // Convert each element from int to float
        for (size_t i = 0; i < image.data.size(); ++i) {
            for (size_t j = 0; j < image.data[i].size(); ++j) {
                kmeans_data[i][j] = static_cast<double>(image.data[i][j]);
            }
        }

        // Run KMeans
        std::vector<std::vector<double>> clustered_data = kmeans_module.run(kmeans_data, num_clusters, max_iters, epsilon);

        // Convert back to uint_t
        PPMData out_image(image.width, image.height, image.max_value);

        for (size_t i = 0; i < clustered_data.size(); ++i) {
            for (size_t j = 0; j < clustered_data[i].size(); ++j) {
                out_image.data[i][j] = convertDoubleToUInt8(clustered_data[i][j]);
            }
        }

        ppm_module.write_file(out_image, "clustered_image.ppm", true);

    }

    return 0;
}