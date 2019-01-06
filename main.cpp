#include "file.hpp"

void usage [[noreturn]](const std::string &prog_name)
{
    std::cerr << "Usage: " << prog_name << " <file> <hash_type> -r" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

    int exit_code = EXIT_SUCCESS;

    try
    {
        if(argc == 4 && std::strncmp(argv[3], "-r", 2) == 0){
            File::hash_directory(argv[1], argv[2]);
        }
        else if(argc == 3){
            std::ifstream file_stream = File::open(argv[1]);
            if(File::is_directory())
                throw File::Exception(std::strerror(EISDIR), EISDIR);

            std::cout << "[*] Hashing " << File::size(argv[1]) << " bytes." << '\n';
            std::string hex_digest = File::hash(file_stream, argv[2]);
            std::cout << "[*] Output: " << hex_digest << std::endl;
        }
        else{
            usage(argv[0]);
        }
    }
    catch (File::Exception &f_ex)
    {
        std::cerr << "Error (" << f_ex.code() << "): " << f_ex.what() << std::endl;
        exit_code = f_ex.code();
    }
    catch(std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit_code = EXIT_FAILURE;
    }

    return exit_code;
}
