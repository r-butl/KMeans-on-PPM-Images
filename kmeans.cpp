#include "kmeans.h"

KMeans::KMeans() {};

KMeans::~KMeans() {};

std::vector<std::vector<double>> KMeans::initialize_centroids(const std::vector<std::vector<double>> &data, int clusters){
    
    // Eventually implement Kmeans++, for now, choose (clusters) amount of random points from the og dataset
    std::vector<std::vector<double>> centroids;

    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, data.size() - 1);
    std::set<int> indices;  

    int count = 0;
    int new_number = 0;
    while (indices.size() < clusters){
        new_number = distrib(gen);
        indices.insert(new_number);
    }

    // Select the indices and place them in the centroids vector
    for(auto idx: indices){
        centroids.push_back(data[idx]);
    }

    return centroids;
}

double KMeans::calc_distance(const std::vector<double> &p1, const std::vector<double> &p2){

    double distance_sq_sum = 0;
    double difference = 0;
    for (int i = 0; i < p1.size(); i++){
        difference = p1[i] - p2[i];
        distance_sq_sum += difference * difference;
    }

    return distance_sq_sum;
}

void KMeans::calc_all_distance(const std::vector<std::vector<double>> &data_points, const std::vector<std::vector<double>> &centroids, std::vector<std::vector<double>> &distances){
    /*
        data points = (number of data points) * (feature size of 1 data point)
        centroids = (number of centroids) * (feature size of 1 data point)
        distances =  (number of data_points) * (number of centroids)
    */
    if (distances.size() != data_points.size() || distances[0].size() != centroids.size()){
        throw std::runtime_error("Distance vector (n * m) must be n = datapoints.size and m = centroids.size");
    }

    for (int data = 0; data < data_points.size(); data++){
        for (int centroid = 0; centroid < centroids.size(); centroid++){
            distances[data][centroid] = calc_distance(data_points[data], centroids[centroid]);
        }
    }
}

void KMeans::choose_clusters_from_distances(const std::vector<std::vector<double>> &distances, std::vector<int> cluster_assignment_index){

}


std::vector<std::vector<double>> KMeans::run(std::vector<std::vector<double>> &data, int clusters) {
    int dim = data[0].size();
    int data_points = data.size();
    std::cout << "Dimensionality of data: " << dim << std::endl;

    std::vector<std::vector<double>> centroids = initialize_centroids(data, clusters);

    // data, centroids
    std::vector<std::vector<double>> distances(data.size(), std::vector<double>(centroids.size()));

    
}