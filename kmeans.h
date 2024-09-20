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

        template <typename T>
        double calc_distance(const std::vector<T> &p1, const std::vector<T> &p2);
        
        void calc_all_distance(const std::vector<std::vector<double>> &data_points, const std::vector<std::vector<double>> &centroids, std::vector<std::vector<double>> &distances);

        void choose_clusters_from_distances(const std::vector<std::vector<double>> &distances, std::vector<int> &cluster_assignment_index);
        
        std::vector<std::vector<double>> calc_cluster_centroids(const std::vector<std::vector<double>> &data_points, const std::vector<int> &cluster_assignment_index, int num_clusters);

        bool has_converged(const std::vector<std::vector<double>> &p1, const std::vector<std::vector<double>> &p2, const double &epsilon);

        std::vector<std::vector<double>> run(std::vector<std::vector<double>> &data, int clusters, int iterations, double epsilon);


    private:
}; 

#endif