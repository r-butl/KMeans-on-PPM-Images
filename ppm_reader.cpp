#include "ppm_reader.h"

PPMReader::PPMReader() {}

PPMReader::~PPMReader() {}

PPMData PPMReader::read_file(std::filesystem::path &file) {
    /*
        Helpful Articles:

        https://my.eng.utah.edu/~cs5610/ppm.html#:~:text=%22P6%22%20image%20files%20are%20obviously,bottom%2C%20left%20to%20right%20order.
    
    */

    std::ifstream image_file;
    image_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    bool binary_mode = false;   // Default text mode
    std::string line;
   
    ///////////////////////////////////// Open the File
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

    ///////////////////////////////////// Parse header and create PPM file

    int x, y, max_value = 0;

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
            // Skip comment
            if (line[0] == '#'){
                std::cout << "Comment: " << line << std::endl;
            } else {
                if (element_counter == 0){
                    // Width and height
                    std::istringstream dimensions(line);
                    dimensions >> x >> y;
                    std::cout << x << " " << y << " " << std::endl;
                    element_counter++;
                }
                else if (element_counter == 1){
                    // Max intensity
                    std::istringstream intensity(line);
                    intensity >> max_value;
                    std::cout << max_value << std::endl;
                    break;
                }
            }
        }
    } catch (std::exception &e){
        std::cerr << "Error parsing header: " << e.what() << std::endl;
        exit(1);
    }

    PPMData ppm_file(x, y, max_value);

    ///////////////////////////////////// Switch file modes if necessary
    try{

        if (binary_mode){
            std::streampos stream_position = image_file.tellg();
            image_file.close();
            image_file.open(file, std::ios::in | std::ios::binary);    
            image_file.seekg(stream_position);

            if (!image_file.is_open()){
                std::cerr << "ERROR: File could not open in binary mode." << std::endl;
                exit(1);
            }
        }

    } catch (std::exception &e){
        std::cerr << "ERROR: Switching to binary read mode: " << e.what() << std::endl;
        exit(1);
    }

    ///////////////////////////////////// Read in the data

    int image_area = ppm_file.width * ppm_file.height;
    int expected_bytes = image_area * PIXEL_DATA_WIDTH_BYTES;
    int bytes_read = 0;

    if (binary_mode){

        unsigned char* buffer = new unsigned char[expected_bytes];

        do{
            image_file.read(reinterpret_cast<char*>(buffer) + bytes_read, expected_bytes - bytes_read);
            int bytes = image_file.gcount();
            if (bytes == 0){
                break;
            }
            bytes_read += bytes;
        } while(bytes_read < expected_bytes);

        // count how many bytes were left at the end of the file
        if (!image_file.eof()) {

            std::streampos current_pos = image_file.tellg();
            image_file.seekg(0, std::ios::end);
            std::streampos end_pos = image_file.tellg();
            std::streamoff bytes_remaining = end_pos - current_pos;

            std::cerr << "ERROR READING: Bytes remaining: " << bytes_remaining << std::endl;
        }

        image_file.close();

        ///////////////////////////////////// Translate the values into a vector
        for (int i = 0; i < image_area; i++){ 

            // For each byte in the pixel
            for (int j = 0; j < PIXEL_DATA_WIDTH_BYTES; j++){
                ppm_file.data[i][j] = static_cast<uint8_t>(buffer[i*PIXEL_DATA_WIDTH_BYTES + j]);
            }
       }
        
        delete[] buffer;    // Deallocate buffer
        
    }else{

        // Read in the full string
        std::string buffer((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());

        std::cout << buffer << std::endl;
    }

    return ppm_file;

}

bool PPMReader::write_file(PPMData picture, std::filesystem::path file_name, bool binary){

    std::ofstream out_file;

    // Write the header in ascii
    out_file.open(file_name, std::ios::out);

    // Write the data mode
    if (binary){
        out_file << "P6" << std::endl;
    }else{
        out_file << "P3" << std::endl;
    }

    // Write the image dimensions to the file
    out_file << std::to_string(picture.width) << " " << std::to_string(picture.height) << std::endl;
    out_file << std::to_string(picture.max_value) << std::endl << std::endl;

    // Write the 
    if (binary){
        // Open in binary output append mode
        out_file.open(file_name, std::ios::binary | std::ios::out | std::ios::app);

        for (auto pixel: picture.data){
            for (auto byte: pixel){
                out_file.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
            }
        }

        out_file.close();
        return true;
    }

}
