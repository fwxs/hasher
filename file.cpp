#include "file.hpp"

struct stat File::st_buf;

bool File::is_directory()
{
    return (st_buf.st_mode & S_IFMT) == S_IFDIR;
}

bool File::exists(const std::string &pathname)
{
    std::memset(&st_buf, 0, sizeof (struct stat));
    return (stat(pathname.c_str(), &st_buf) == 0);
}

std::ifstream File::open(const std::string &pathname) noexcept(false)
{
    if(!File::exists(pathname)){
        int errsv = errno;
        throw File::Exception(std::strerror(errsv), errsv);
    }

    std::ifstream file_stream(pathname, std::ifstream::in | std::ifstream::binary);
    if(!file_stream.is_open()){
        throw File::Exception(std::string("Can't open ") + pathname, EXIT_FAILURE);
    }

    return file_stream;
}

template<std::size_t SIZE>
std::string File::hash_to_hex(const std::array<unsigned char, SIZE> array, const std::size_t bytes_written)
{
    std::stringstream stream;
    for(unsigned int inx = 0; inx < bytes_written; inx++)
    {
        stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint>(array[inx]);
    }

    return stream.str();
}

std::string File::hash(std::ifstream &stream, const std::string &hash_type)
{
    auto md_type = std::make_unique<const EVP_MD *>(EVP_get_digestbyname(hash_type.c_str()));

    if(*md_type == nullptr)
        throw File::Exception("Unknown hash type", EXIT_FAILURE);

    const int block_size = EVP_MD_block_size(*md_type);
    auto md_ctx = std::make_unique<EVP_MD_CTX *>(EVP_MD_CTX_new());

    EVP_DigestInit(*md_ctx, *md_type);

    auto buffer = std::make_unique<char []>(static_cast<std::size_t>(block_size));

    while(stream.good())
    {
        stream.read(buffer.get(), block_size);
        EVP_DigestUpdate(*md_ctx, buffer.get(), static_cast<std::size_t>(stream.gcount()));
    }

    unsigned int bytes_written;
    std::array<unsigned char, EVP_MAX_MD_SIZE> hash_str;
    EVP_DigestFinal_ex(*md_ctx, hash_str.data(), &bytes_written);

    // Close
    EVP_MD_CTX_free(*md_ctx);
    buffer.reset(nullptr);

    return hash_to_hex(hash_str, bytes_written);
}

off_t File::size(const std::string &filename)
{
    struct stat st;
    std::memset(&st, 0, sizeof (struct stat));
    if(stat(filename.c_str(), &st) == -1){
        int errsv = errno;
        throw File::Exception(std::strerror(errsv), errsv);
    }

    return st.st_size;
}

void File::hash_directory(const std::string &path, const std::string &hash_type)
{
    auto dir = std::make_unique<DIR *>(opendir(path.c_str()));

    if(*dir == nullptr)
    {
        int errsv = errno;
        throw File::Exception(std::strerror(errsv), errsv);
    }

    struct dirent **dir_ent;

    int files = scandir(path.c_str(), &dir_ent, nullptr, alphasort);

    if(files == -1)
    {
        int errsv = errno;
        throw File::Exception(std::strerror(errsv), errsv);
    }

    std::string fullpath;
    std::cout << "[*] Hashing " << files - 2 << " files in " << path << '\n';

    while(files--){
        fullpath = path + '/' + dir_ent[files]->d_name;
        std::ifstream f_stream = File::open(fullpath);

        if(!(std::strcmp(dir_ent[files]->d_name, ".") == 0 || std::strcmp(dir_ent[files]->d_name, "..") == 0))
        {
            if(File::is_directory()){
                std::cout << "\t[-] " << dir_ent[files]->d_name << " is a directory." << '\n';
                //File::hash_directory(fullpath, hash_type);
                std::cout << std::endl;
                continue;
            }

            std::cout << "\t[+] Hashing " << dir_ent[files]->d_name;
            std::cout << "\tsize " << File::size(fullpath) << " bytes" << '\n';
            std::cout << "\t[-] Output: " << File::hash(f_stream, hash_type) << '\n';
            std::cout << std::endl;
        }
        f_stream.close();
    }
    closedir(*dir);
}

File::Exception::~Exception(){}
