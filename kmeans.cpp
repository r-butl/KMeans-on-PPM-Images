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

    // Generate random indices
    int new_number = 0;
    while (indices.size() < clusters){
        new_number = distrib(gen);
        indices.insert(new_number);
    }

    // Select using indices and place them in the centroids vector
    for(auto idx: indices){
        centroids.push_back(data[idx]);
    }

    return centroids;
}

template <typename T>
double KMeans::calc_distance(const std::vector<T> &p1, const std::vector<T> &p2){
    // Calculate distance between two feature sets

    if (p1.size() != p2.size()){
        std::cerr << "ERROR: calc_distance: input vectors are not the same dimension.\n";
        exit(1);
    }

    double distance_sq_sum = 0;
    double difference = 0;

    for (int i = 0; i < p1.size(); ++i){
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

        Operating on the data in place because these vectors will be huge.
    */

    if (distances.size() != data_points.size() || distances[0].size() != centroids.size() || data_points[0].size() != centroids[0].size()){
        std::cerr << "ERROR: calc_all_distance: input vector dimensions are incompatible.\n";
        exit(1);
    }

    // Calculuate the distances between each centroid and data point
    for (int d = 0; d < data_points.size(); ++d){
        for (int c = 0; c < centroids.size(); ++c){
            distances[d][c] = calc_distance(data_points[d], centroids[c]);
        }
    }
}

void KMeans::choose_clusters_from_distances(const std::vector<std::vector<double>> &distances, std::vector<int> &cluster_assignment_index){
    // Assign data points to clusters based on smallest distance

    if (distances.size() != cluster_assignment_index.size()){
        std::cerr << "ERROR: choose_clusters_from_distances: input vectors are incompatible.\n";
    }

    for (int i = 0; i < distances.size(); ++i){
        int best_idx = 0;
        double closest_value = __DBL_MAX__;

        // find smallest distance
        for (int d = 0; d < distances[i].size(); ++d){
            double curr_distance = distances[i][d];
            if (curr_distance < closest_value){
                closest_value = curr_distance;
                best_idx = d; 
            }
        }

        cluster_assignment_index[i] = best_idx;
    }
}

void KMeans::calc_cluster_centroids(const std::vector<std::vector<double>> &data_points, const std::vector<int> &cluster_assignment_index, std::vector<std::vector<double>> &centroids){
    // Calculate the centroids of each cluster

    if (data_points.size() != cluster_assignment_index.size() || centroids[0].size() != data_points[0].size()){
        std::cerr << "ERROR: calc_cluster_centroids: input vectors are incompatable.\n";
        exit(1);
    }

    int data_size = data_points.size();
    int feature_size = data_points[0].size();
    int num_clusters = centroids.size();

    // Calculate the mean of each centroid
    std::vector<int> cluster_label_counts(num_clusters);

    // Initialize the new centroids and label counts to zero
    for (int c = 0; c < num_clusters; ++c){
        cluster_label_counts[c] = 0;
        for (int f = 0; f < feature_size; ++f){
            centroids[c][f] = 0;
        }
    }

    // Sum up the centroids and labels for each point in the dataset
    for (int d = 0; d < data_size; ++d){   // For each point

        int assigned_cluster = cluster_assignment_index[d];     // Get the data point's label
        ++cluster_label_counts[assigned_cluster];               // Add to the sum of data points with this centroid label

        // For each feature add up the data point value to the centroid sum
        for (int f = 0; f < feature_size; ++f){
            centroids[assigned_cluster][f] += data_points[d][f];
        }
    }

    // Calculate the mean for each centroid feature
    for (int c = 0; c < num_clusters; ++c){

        for (int f = 0; f < feature_size; ++f){
            if (cluster_label_counts[c] != 0){
                centroids[c][f] /= cluster_label_counts[c];
            } else {
                std::cout << "WARNING: Centroid has 0 labels!\n";
            }
        }
    }   
}

bool KMeans::has_converged(const std::vector<std::vector<double>> &p1, const std::vector<std::vector<double>> &p2, const double &epsilon){
    // Check if all distances between features are below the epsilon value

    if (p1.size() != p2.size() || p1[0].size() != p2[0].size()){
        std::cerr << "ERROR: detect_move: vectors are not the same dimensions." << std::endl;
    }

    bool converged = true;;

    for (int d = 0; d < p1.size(); d++){
        if (calc_distance(p1[d], p2[d]) > epsilon)
            converged = false;
            break;
    }

    return converged;
}

std::vector<std::vector<double>> KMeans::run(std::vector<std::vector<double>> &data, int num_clusters, int iterations, double epsilon) {

    int iter_count = 0;
    int dim = data[0].size();
    
    std::vector<std::vector<double>> centroids_curr(num_clusters, std::vector<double>(dim));
    std::vector<std::vector<double>> centroids_prev(num_clusters, std::vector<double>(dim));
    std::vector<int> cluster_assignments_curr(data.size());
    std::vector<int> cluster_assignments_prev(data.size());
    std::vector<std::vector<double>> distances(data.size(), std::vector<double>(num_clusters));

    // Initialize Distances and Cluster assignments
    centroids_curr = initialize_centroids(data, num_clusters);
    centroids_prev = centroids_curr;

    while (iter_count < iterations){

        calc_all_distance(data, centroids_curr, distances);
        choose_clusters_from_distances(distances, cluster_assignments_curr);
        calc_cluster_centroids(data, cluster_assignments_curr, centroids_curr);

        // If no changes were made in the labels, we have converged
        if (has_converged(centroids_curr, centroids_prev, epsilon)){
            centroids_curr = centroids_prev;
            cluster_assignments_curr = cluster_assignments_prev;
            printf("Converged in %d iterations...\n", iter_count);
            break;
        }

        // Still more learning to do
        cluster_assignments_prev = cluster_assignments_curr;
        centroids_prev = centroids_curr;
        iter_count++;
    }

    // Apply Centroids to data points to cluster the text
    std::vector<std::vector<double>> return_data = data;    // Create a copy of the data and override value with assigned cluster centroid values
    for (int d = 0; d < return_data.size(); d++) {

        return_data[d] = centroids_curr[cluster_assignments_curr[d]];
    }

    return return_data;
}