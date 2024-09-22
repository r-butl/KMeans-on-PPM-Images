#include "ppm_reader.h"
#include "kmeans.h"
#include <string>
#include <vector>
#include <filesystem>
#include <cstring>
#include <getopt.h>
#include <cstdlib>
#include <iostream>

uint8_t convertDoubleToUInt8(double value) {
    // Clamp the value to the range [0, 255]
    if (value < 0) {
        return 0;
    } else if (value > 255) {
        return 255;
    }
    return static_cast<uint8_t>(std::round(value)); // Use round to avoid truncation
}

// Struct to hold the parsed command-line arguments
struct Arguments {
    std::filesystem::path file;
    int num_clusters = 0;
    int max_iters = 50;
    double epsilon = 0.0;
};

// Function to parse command-line arguments
bool parse_arguments(int argc, char** argv, Arguments& args) {
    const char* optstring = "f:n:e:i:";  // -f for file, -n for num_clusters, -e for epsilon, -i for max_iters
    int option;

    // Parse options using getopt
    while ((option = getopt(argc, argv, optstring)) != -1) {
        switch (option) {
            case 'f':
                args.file = optarg;  // File path
                break;
            case 'n':
                args.num_clusters = std::atoi(optarg);  // Number of clusters
                break;
            case 'e':
                args.epsilon = std::atof(optarg);  // Epsilon
                break;
            case 'i':
                args.max_iters = std::atoi(optarg);  // Max iterations
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " -f <file> -n <num_clusters> [-e <epsilon>] [-i <max_iters>]" << std::endl;
                return false;
        }
    }

    // Ensure required arguments are provided
    if (args.file.empty() || args.num_clusters == 0) {
        std::cerr << "Error: file and num_clusters are required." << std::endl;
        std::cerr << "Usage: " << argv[0] << " -f <file> -n <num_clusters> [-e <epsilon>] [-i <max_iters>]" << std::endl;
        return false;
    }

    return true;
}

std::string generate_output_filename(const std::filesystem::path& input_file) {
    // Change the output file name
    return "CLUSTERED_" + input_file.filename().string();
}

int main(int argc, char** argv){


    PPMReader ppm_module;
    KMeans kmeans_module;

    // Struct to hold the parsed arguments
    Arguments args;

    // Parse the command-line arguments
    if (!parse_arguments(argc, argv, args)) {
        return EXIT_FAILURE;
    }

    // Output the parsed values
    std::cout << "File: " << args.file << "\n"
              << "Number of Clusters: " << args.num_clusters << "\n"
              << "Epsilon: " << args.epsilon << "\n"
              << "Max Iterations: " << args.max_iters << std::endl;

    PPMData image = ppm_module.read_file(args.file);

    std::vector<std::vector<double>> kmeans_data(image.data.size(), std::vector<double>(image.data[0].size()));

    // Convert each element from int to float
    for (size_t i = 0; i < image.data.size(); ++i) {
        for (size_t j = 0; j < image.data[i].size(); ++j) {
            kmeans_data[i][j] = static_cast<double>(image.data[i][j]);
        }
    }

    // Run KMeans
    std::vector<std::vector<double>> clustered_data = kmeans_module.run(kmeans_data, args.num_clusters, args.max_iters, args.epsilon);

    // Convert back to uint_t
    PPMData out_image(image.width, image.height, image.max_value);

    for (size_t i = 0; i < clustered_data.size(); ++i) {
        for (size_t j = 0; j < clustered_data[i].size(); ++j) {
            out_image.data[i][j] = convertDoubleToUInt8(clustered_data[i][j]);
        }
    }

    // Write the data to a file
    std::string output_filename = generate_output_filename(args.file);
    std::cout << "Output File: " << output_filename << std::endl;
    ppm_module.write_file(out_image, output_filename, true);


    return 0;
}