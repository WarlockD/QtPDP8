#ifndef PDP8DISSAM_H
#define PDP8DISSAM_H

#include <string>
#include <cstdint>

namespace PDP8 {
    void simple_dsam8(uint16_t op, uint16_t pc, const uint16_t* mem,  std::string& disbuf, std::string&combuf) ;
    std::string dsam8(uint16_t op, uint16_t pc, const uint16_t* mem, bool comment=false);
}

#endif // PDP8DISSAM_H
