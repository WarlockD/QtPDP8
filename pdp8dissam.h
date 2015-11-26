#ifndef PDP8DISSAM_H
#define PDP8DISSAM_H

#include <string>
#include <cstdint>
#include <unordered_map>

namespace PDP8 {
    void simple_dsam8(uint16_t op, uint16_t pc, const uint16_t* mem,  std::string& disbuf, std::string&combuf) ;
    std::string dsam8(uint16_t op, uint16_t pc, const uint16_t* mem, bool comment=false);

    class Dissasembler {
    public:
        struct Line {
            std::string line;
            std::string comment;
            std::uint16_t addr;
            std::uint16_t data;
            /*
            Line() : line(""), comment(""), addr(0), data(0) {}
            Line(const Line& l) : line(l.line), comment(l.comment), addr(l.addr), data(l.data){}
            Line(Line&& l) : line(std::move(l.line)), comment(std::move(l.comment)), addr(l.addr), data(l.data) {}
            */
        };
    protected:
        std::vector<Line> _disamLines;
        std::unordered_map<std::size_t,std::size_t> _addrToLine;
        const uint16_t * mem;
    public:
        Dissasembler(const uint16_t * mem);
        const Line& atAddr(std::size_t i) ;
    };
}

#endif // PDP8DISSAM_H
