#include "ppm_reader.h"

PPMReader::PPMReader() {}

PPMReader::~PPMReader() {}

PPMData PPMReader::parse_header(std::ifstream &fd, bool &binary_mode){

    int x, y, max_value = 0;
    std::string line;

    try{
        std::getline(fd, line);
        if (line != "P6" && line != "P3") {
            std::cerr << "Error: Unsupported PPM format" << std::endl;
            exit(1);
        }else{
            std::cout << "File mode: " << line << std::endl;
            if (line == "P6") binary_mode = true;
        }

        // Read the rest of the header for image format information
        int element_counter = 0;    // 0 = width and height line 1 = max intensity
        while (std::getline(fd, line)){
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

    return ppm_file;

}

void PPMReader::read_binary_ppm_data(std::ifstream &fd, PPMData &ppm_data){

    int image_area = ppm_data.width * ppm_data.height;
    int expected_bytes = image_area * PIXEL_DATA_WIDTH_BYTES;
    int bytes_read = 0;
    unsigned char* buffer = new unsigned char[expected_bytes];

    do{
        fd.read(reinterpret_cast<char*>(buffer) + bytes_read, expected_bytes - bytes_read);
        int bytes = fd.gcount();
        if (bytes == 0){
            break;
        }
        bytes_read += bytes;
    } while(bytes_read < expected_bytes);

    // count how many bytes were left at the end of the file
    std::streampos current_pos = fd.tellg();
    fd.seekg(0, std::ios::end);
    std::streampos end_pos = fd.tellg();
    std::streamoff bytes_remaining = end_pos - current_pos;
    
    if (bytes_remaining > 0) {

        std::cerr << "ERROR READING: Bytes remaining: " << bytes_remaining << std::endl;
    }

    fd.close();

    ///////////////////////////////////// Translate the values into a vector
    for (int i = 0; i < image_area; i++){ 

        // For each byte in the pixel
        for (int j = 0; j < PIXEL_DATA_WIDTH_BYTES; j++){
            ppm_data.data[i][j] = static_cast<uint8_t>(buffer[i*PIXEL_DATA_WIDTH_BYTES + j]);
        }
    }
    
    delete[] buffer;    // Deallocate buffer

}

void PPMReader::read_ascii_ppm_data(std::ifstream &fd, PPMData &ppm_data){

}

PPMData PPMReader::read_file(std::filesystem::path &file) {
    /*
        Helpful Articles:

        https://my.eng.utah.edu/~cs5610/ppm.html#:~:text=%22P6%22%20image%20files%20are%20obviously,bottom%2C%20left%20to%20right%20order.
    
    */

    std::ifstream image_file;
    image_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    bool binary_mode = false;   // Default text mode
   
    ///////////////////////////////////// Open the File

    try {
        // Attempt to open the file
        image_file.open(file, std::ios::in);
        
        // Check if the file was successfully opened
        if (!image_file) {
            std::cerr <<"Error: File could not be opened.\n";
            exit(1);
        }
    }
    catch (std::exception &e){
        std::cerr << "Error opening file: " << e.what() << std::endl;
    }

    ///////////////////////////////////// Parse header and create PPM file

    PPMData ppm_data = parse_header(image_file, binary_mode);

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



    if (binary_mode){

        read_binary_ppm_data(image_file, ppm_data);
        
    }else{


    }

    return ppm_data;

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
    out_file << std::to_string(picture.max_value) << std::endl;
    out_file.close();

    // Write the 
    if (binary){
        // Open in binary output append mode
        out_file.open(file_name, std::ios::binary | std::ios::out | std::ios::app);

        if (!out_file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return false;
        }

        for (size_t d = 0; d < picture.data.size(); d++) {
            for (size_t p = 0; p < picture.data[d].size(); p++) {
                out_file.write(reinterpret_cast<const char*>(&picture.data[d][p]), sizeof(picture.data[d][p]));
            }
        }

        out_file.close();
        return true;
    }

}
