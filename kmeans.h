#ifndef KMEANS_IMPL
#define KMEANS_IMPL

#include <vector>
#include <set>
#include <random>
#include <cfloat>
#include <iostream>

class KMeans{

    public:
        KMeans();
        ~KMeans();

        std::vector<std::vector<double>> initialize_centroids(const std::vector<std::vector<double>> &data, int clusters);

        double calc_distance(const std::vector<double> &p1, const std::vector<double> &p2);
        
        void calc_all_distance(const std::vector<std::vector<double>> &data_points, const std::vector<std::vector<double>> &centroids, std::vector<std::vector<double>> &distances);

        void choose_clusters_from_distances(const std::vector<std::vector<double>> &distances, std::vector<int> &cluster_assignment_index);
        
        std::vector<std::vector<double>> run(std::vector<std::vector<double>> &data, int clusters);

    private:
}; 

#endif