#include "ppm_kmeans.h"

PPMKmeans::PPMKmeans() {}

PPMKmeans::~PPMKmeans() {}



bool PPMKmeans::read_file(std::string file, std::vector<std::vector<int>> &image_data, int &x, int &y, int &max) {
    /*
        Helpful Articles:

        https://my.eng.utah.edu/~cs5610/ppm.html#:~:text=%22P6%22%20image%20files%20are%20obviously,bottom%2C%20left%20to%20right%20order.
    
    */

    std::ifstream image_file;
    image_file.exceptions( std::ifstream::failbit | std::ifstream::badbit);
    bool binary_mode = false;   // Default text mode
    std::streampos stream_position = 0; // If we have to close the file and reload in binary mode, we will want to skip passed the header
    std::string line;
    int pixels_added = 0;
   
    try {
        // Attempt to open the file
        image_file.open(file, std::ios::in);
        
        // Check if the file was successfully opened
        if (!image_file) {
            throw std::runtime_error("Error: File could not be opened.");
        }
    }
    catch (std::exception &e){
        std::cerr << "Error opening file: " << e.what() << std::endl;

    }

    // Get Magic Number for ASCII or Binary information
    std::getline(image_file, line);
    if (line != "P6" && line != "P3") {
        std::cerr << "Error: Unsupported PPM format" << std::endl;
        return false;
    }else{
        std::cout << "File mode: " << line << std::endl;
        if (line == "P6") binary_mode = true;
    }

    // Read the rest of the header for image format information
    int element_counter = 0;    // 0 = width and height line 1 = max intensity
    while (std::getline(image_file, line)){
        if (line[0] == '#'){
            std::cout << "Comment: " << line << std::endl;
        } else {
            if (element_counter == 0){
                // Width and height
                std::istringstream dimensions(line);
                dimensions >> x >> y;
                std::cout << "Width: " << x <<  " Height: " << y << std::endl;
                element_counter++;
            }
            else if (element_counter == 1){
                // Max intensity
                std::istringstream intensity(line);
                intensity >> max;
                std::cout << "Max Intensity: " << max << std::endl;
                break;
            }
        }
    }

    // Switch file modes if necessary
    if (binary_mode){
        stream_position = image_file.tellg();
        image_file.close();
        image_file.open(file, std::ios::in | std::ios::binary);    
        image_file.seekg(stream_position);

        if (!image_file.is_open()){
            std::cerr << "File could not open in binary mode." << std::endl;
        }
    }

    std::cout << "Beginning read" << std::endl;

    // Read in the data
    if (binary_mode){
        // Read the file 3 bytes at a time
        while(!image_file.eof()){

            /*
                using unsigned char is often preferred when dealing with raw binary data because it 
                prevents the data from being misinterpreted as a signed value (negative numbers), 
                which could happen if you use char (which is typically signed by default on most platforms).
            */
            unsigned char r = 0, g = 0, b = 0;

            // read a byte per value
            image_file.read(reinterpret_cast<char*>(&r), sizeof(r));    // Read expects a pointer to a block 
            image_file.read(reinterpret_cast<char*>(&g), sizeof(g));    // of memory of type char, must recast 
            image_file.read(reinterpret_cast<char*>(&b), sizeof(b));    // from unsigned char* to char*
            
            // std::cout << static_cast<int>(r) << " " << static_cast<int>(g) << " " << static_cast<int>(b) << std::endl;

            // Add the pixel to the image vector as casted integers
            image_data.push_back({static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)}); 
            pixels_added++;   
        }
    }else{

        while(std::getline(image_file, line)){  // get line ignores potential new line character

            if (line.length() == 3){    // Same as up top, ensuring no partial reads
                image_data.push_back({  static_cast<int>(static_cast<unsigned char>(line[0])),  // first byte
                                        static_cast<int>(static_cast<unsigned char>(line[1])),  // second byte
                                        static_cast<int>(static_cast<unsigned char>(line[2])),  // third byte
                                    });
            }
            pixels_added++;   

        }
    }

    image_file.close();
    std::cout << "Closing File" << std::endl;


    std::cout << "Pixels counted: " << pixels_added << " Expected Pixels: " << x * y;
    return pixels_added == x * y;

}

