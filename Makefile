test: test_ppm_kmeans.cpp ppm_kmeans.cpp
	g++ -o ppm_kmeans test_ppm_kmeans.cpp ppm_kmeans.cpp -Wall -pedantic -std=c++17
