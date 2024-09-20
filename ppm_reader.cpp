#include "ppm_reader.h"

PPMReader::PPMReader() {}

PPMReader::~PPMReader() {}

std::vector<std::vector<uint8_t>> PPMReader::read_file(std::string file,  int &x, int &y, int &max) {
    /*
        Helpful Articles:

        https://my.eng.utah.edu/~cs5610/ppm.html#:~:text=%22P6%22%20image%20files%20are%20obviously,bottom%2C%20left%20to%20right%20order.
    
    */

    std::ifstream image_file;
    image_file.exceptions( std::ifstream::failbit | std::ifstream::badbit);
    bool binary_mode = false;   // Default text mode
    std::string line;
   
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
    try{
        std::getline(image_file, line);
        if (line != "P6" && line != "P3") {
            std::cerr << "Error: Unsupported PPM format" << std::endl;
            exit(1);
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
    } catch (std::exception &e){
        std::cerr << "Error parsing header: " << e.what() << std::endl;
        exit(1);
    }

    // Switch file modes if necessary
    try{

        if (binary_mode){
            std::streampos stream_position = image_file.tellg();
            image_file.close();
            image_file.open(file, std::ios::in | std::ios::binary);    
            image_file.seekg(stream_position);

            if (!image_file.is_open()){
                std::cerr << "File could not open in binary mode." << std::endl;
                exit(1);
            }
        }

    } catch (std::exception &e){
        std::cerr << "Error switching to binary read mode: " << e.what() << std::endl;
        exit(1);
    }

    std::vector<std::vector<uint8_t>> result(x * y, std::vector<uint8_t>(PIXEL_DATA_WIDTH_BYTES));
    int expected_bytes = x * y * PIXEL_DATA_WIDTH_BYTES;
    int bytes_read = 0;

    // Read in the data
    if (binary_mode){

        unsigned char* buffer = new unsigned char[expected_bytes];

        do{
            image_file.read(reinterpret_cast<char*>(buffer) + bytes_read, expected_bytes - bytes_read);
            int result = image_file.gcount();
            if (result == 0){
                break;
            }
            bytes_read += result;
        } while(bytes_read < expected_bytes);

        std::cout << "Bytes read: " << bytes_read << std::endl;

        // count how many bytes were left at the end of the file
        if (!image_file.eof()) {

            std::streampos current_pos = image_file.tellg();
            image_file.seekg(0, std::ios::end);
            std::streampos end_pos = image_file.tellg();
            std::streamoff bytes_remaining = end_pos - current_pos;

            std::cout << "Bytes remaining: " << bytes_remaining << std::endl;
        }

        image_file.close();
        std::cout << "Closing File" << std::endl;

        // Translate the values into a vector
        for (int i = 0; i < x * y; i++){    // For each pixel
            // For each byte in the pixel
            for (int j = 0; j < PIXEL_DATA_WIDTH_BYTES; j++){
                result[i][j] = static_cast<uint8_t>(buffer[i*PIXEL_DATA_WIDTH_BYTES + j]);
            }
       }
        
        delete[] buffer;    // Deallocate buffer
        
    }else{

        // Read in the full string
        std::string buffer((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());

        std::cout << buffer << std::endl;
    }

    return result;

}

