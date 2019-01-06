#ifndef FILE_HPP
#define FILE_HPP
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <openssl/evp.h>
#include <sstream>
#include <string>
#include <sys/stat.h>

namespace File{
    extern struct stat st_buf;

    bool is_directory();
    bool exists(const std::string &);
    std::ifstream open(const std::string &);

    template<std::size_t SIZE>
    std::string hash_to_hex(const std::array<unsigned char, SIZE>, const std::size_t);
    std::string hash(std::ifstream&, const std::string &);
    off_t size(const std::string &);

    inline std::string get_filename (const std::string pathname){
        ulong inx = pathname.find_last_of('/');
        return (inx != pathname.npos) ? pathname.substr(inx + 1) : pathname;
    }

    void hash_directory(const std::string &, const std::string &);

    class Exception : public std::exception
    {
        private:
            const std::string msg;
            const int err_code;

        public:
            Exception(const std::string &m, const int e) : msg(m), err_code(e) {}

            inline virtual const char* what() const noexcept
            {
                return msg.c_str();
            }
            inline virtual int code() const noexcept
            {
                return err_code;
            }
            virtual ~Exception();
    };
}

#endif // FILE_HPP
