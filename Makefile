test: test_ppm_kmeans.cpp ppm_reader.cpp
	g++ -o test_ppm_kmeans test_ppm_kmeans.cpp ppm_reader.cpp kmeans.cpp -Wall -pedantic -std=c++17

main: main.cpp ppm_reader.cpp kmeans.cpp
	g++ -o cluster_image main.cpp ppm_reader.cpp kmeans.cpp -Wall -pedantic -std=c++17
