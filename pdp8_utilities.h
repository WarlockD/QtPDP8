#ifndef PDP8_UTILITIES_H
#define PDP8_UTILITIES_H

#include <pdp8i.h>

namespace PDP8 {
    void LoadRim(std::istream& i ,PDP8_State& s, size_t loc =0200);
    void LoadRim(const std::string& filename, PDP8_State& s, size_t origin =0200);
    void LoadBin(std::istream& i ,PDP8_State& s, size_t loc =0200);
    void LoadBin(const std::string& filename, PDP8_State& s, size_t origin =0200);

    void LoadPaperTape(const std::string& filename, PDP8_State& s, size_t origin =0200);

}
#endif // PDP8_UTILITIES_H
