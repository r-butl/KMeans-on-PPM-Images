test: test_ppm_kmeans.cpp ppm_reader.cpp
	g++ -o ppm_kmeans test_ppm_kmeans.cpp ppm_reader.cpp kmeans.cpp -Wall -pedantic -std=c++17
