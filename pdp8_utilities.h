#ifndef PDP8_UTILITIES_H
#define PDP8_UTILITIES_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <functional>
#include <chrono>
#include <future>
//#include <cstdio>
#include "pdp8state.h"

namespace PDP8 {
      std::string to_octal(int num,const char* fmt="%04o");

    class TwelveBit { // right now just for adder tester
        short _d  : 12;
    public:
        template<typename T> inline TwelveBit(T v) : _d(static_cast<short>(v)) {}
    #define AOPR(b) template<typename T> inline TwelveBit operator b(T a) { _d b static_cast<short>(a); return *this; }  inline TwelveBit operator b(TwelveBit a) { _d b a._d; return *this; }
        AOPR(=) AOPR(+=)AOPR(-=) AOPR(|=) AOPR(&=) AOPR(^=)
    #undef AOPR


    };


    // make some better ones
    class PDP8_Exception : std::exception {
        std::string msg;
    public:
        PDP8_Exception(const std::string& msg) : msg(msg) {}
        const char* what() const _NOEXCEPT { return msg.c_str(); }

    };
    void LoadRim(std::istream& i ,CpuState& s, size_t loc =0200);
    void LoadRim(const std::string& filename, CpuState& s, size_t origin =0200);
    void LoadBin(std::istream& i ,CpuState& s, size_t loc =0200);
    void LoadBin(const std::string& filename, CpuState& s, size_t origin =0200);

    void LoadPaperTape(const std::string& filename, CpuState& s, size_t origin =0200);





    struct FileBuffer {
        std::vector<char> m_vec;
        size_t m_index;
        bool _open;
        void openFile(const char* name) {
            m_index = 0;
            m_vec.clear();
            FILE* f = fopen(name,"rb");
            fseek(f, 0, SEEK_END); // seek to end of file
            size_t size = ftell(f); // get current file pointer
            fseek(f, 0, SEEK_SET); // seek back to beginning of file

            m_vec.resize(size);
            fread(&m_vec[0],sizeof(char),size,f);
            fclose(f);
            _open = true;
        }
        operator bool() const { return _open; }
        void write(char* p, size_t size)
        {
            if(eof())
                throw std::runtime_error("Premature end of array!");

            if((m_index + size) > m_vec.size())
                throw std::runtime_error("Premature end of array!");

            std::memcpy(reinterpret_cast<void*>(&m_vec[m_index]), p, size);

            m_index += size;
        }
         template<typename T>
        void write(const T& t)
        {
            std::vector<char> vec(sizeof(T));
            std::memcpy(reinterpret_cast<void*>(&vec[0]), reinterpret_cast<const void*>(&t), sizeof(T));
            write(vec);
        }

        FileBuffer() : m_index(0) {}
           FileBuffer(const char * mem, size_t size)
           {
               open(mem, size);
           }
           FileBuffer(const std::vector<char>& vec)
           {
               m_index = 0;
               m_vec.clear();
               m_vec.reserve(vec.size());
               m_vec.assign(vec.begin(), vec.end());
           }
           void open(const char * mem, size_t size)
           {
               m_index = 0;
               m_vec.clear();
               m_vec.reserve(size);
               m_vec.assign(mem, mem + size);
           }
           void close()
           {
               m_vec.clear();
           }
           bool eof() const
           {
               return m_index >= m_vec.size();
           }
           std::ifstream::pos_type tellg()
           {
               return m_index;
           }
           bool seekg (size_t pos)
           {
               if(pos<m_vec.size())
                   m_index = pos;
               else
                   return false;

               return true;
           }
           bool seekg (std::streamoff offset, std::ios_base::seekdir way)
           {
               if(way==std::ios_base::beg && offset < m_vec.size())
                   m_index = offset;
               else if(way==std::ios_base::cur && (m_index + offset) < m_vec.size())
                   m_index += offset;
               else if(way==std::ios_base::end && (m_vec.size() + offset) < m_vec.size())
                   m_index = m_vec.size() + offset;
               else
                   return false;

               return true;
           }

           const std::vector<char>& get_internal_vec()
           {
               return m_vec;
           }

           template<typename T>
           void read(T& t)
           {
               if(eof())
                   throw std::runtime_error("Premature end of array!");

               if((m_index + sizeof(T)) > m_vec.size())
                   throw std::runtime_error("Premature end of array!");

               std::memcpy(reinterpret_cast<void*>(&t), &m_vec[m_index], sizeof(T));

               m_index += sizeof(T);
           }
           void readPDP8(unsigned short* p, size_t size) {
               if(eof())
                   throw std::runtime_error("Premature end of array!");

               if((m_index + size) > m_vec.size())
                   throw std::runtime_error("Premature end of array!");

               for(size_t i=0;i<size;i++)
                   p[i] = m_vec[m_index++];

           }

           void read(char* p, size_t size)
           {
               if(eof())
                   throw std::runtime_error("Premature end of array!");

               if((m_index + size) > m_vec.size())
                   throw std::runtime_error("Premature end of array!");

               std::memcpy(reinterpret_cast<void*>(p), &m_vec[m_index], size);

               m_index += size;
           }
    };



}
#endif // PDP8_UTILITIES_H
