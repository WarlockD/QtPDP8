#ifndef PDP8_UTILITIES_H
#define PDP8_UTILITIES_H

#include <pdp8i.h>


namespace PDP8 {

    void LoadRim(std::istream& i ,PDP8_State& s, size_t loc =0200);
    void LoadRim(const std::string& filename, PDP8_State& s, size_t origin =0200);
    void LoadBin(std::istream& i ,PDP8_State& s, size_t loc =0200);
    void LoadBin(const std::string& filename, PDP8_State& s, size_t origin =0200);

    void LoadPaperTape(const std::string& filename, PDP8_State& s, size_t origin =0200);


    class HistoryMaker {
        static const int HIST_PC = 0x40000000;
        static const int HIST_MIN = 64;
        static const int HIST_MAX = 65536;
        static const int HIST_COUNT = 16;
        static const int HIST_MASK = HIST_COUNT-1;
        InstHistory hst[HIST_COUNT];
        int32_t hst_p = 0;                                        /* history pointer */
       public:
        HistoryMaker() : hst_p(0) {}
        void push(PDP8_State& s) {
            InstHistory& n = hst[hst_p];
            n.pc = s.ma | HIST_PC;
            n.ir = s.ir;
            n.lac = s.ac;
            n.mq = s.mq;
            n.opnd = s.mb;
            hst_p=( hst_p+1 )&HIST_MASK;
        }
        InstHistory& operator[](int i) { return hst[i & HIST_MASK]; }

    };



    std::string dsam8(const PDP8_State& s, bool comment, size_t loc);
    std::string dsam8(const PDP8_State& s, bool comment=false);
    std::string dsam8(const InstHistory& s, uint16_t* mem, bool comment=false);
    void simple_dsam8(const InstHistory& s, uint16_t* mem,  std::string& disbuf, std::string&combuf);


}
#endif // PDP8_UTILITIES_H
