#include "pdp8i.h"
#include <iomanip>

static const int endian_int = 0x12345678;

/*
constexpr bool is_little_endian()
{
   // constexpr int a = ((int)0x12345678);
  //  constexpr unsigned char *c = (unsigned char*)(&endian_int);
   // if (*c == 0x78)
  //      return true;

    return *(unsigned char*)(&endian_int) == 0x78;
}

constexpr bool is_big_endian()
{
    constexpr int a = 0x12345678;
    constexpr unsigned char *c = (unsigned char*)(&a);
    if (*c == 0x12)
        return true;

    return false;
}
*/
 std::mutex main_thread_mtx;
namespace PDP8 {
    std::string RegesterHistory::printDisam(size_t count) const{
        std::stringstream ss;

        return ss.str();
    }
    std::string Cpu::printHistory(size_t count)  {
             std::stringstream ss;
            uint16_t o;
            size_t pos = hst.begin();
             for(int i=0;i<count && hst.count()>0;i++,pos--) {
                 if(pos == hst.end()) break;
                 const Regesters& rs = hst[pos];
                 switch(rs.state) {
                 case State::Fetch:
                     //ss  << octzero(r.pc) << " " << octzero(r.opcode) << ": ";
                     o = m[rs.pc];
                     ss << dsam8(o, rs.pc, &m[0], true);
                     ss << std::endl;
                     break;
                 default:
                     count++; // skip the diffrent states for right now
                     break;
                     /*
                 case State::Defer:
                     ss << "Defer  " << octzero(r.ma) << " " << octzero(r.ma) << ":";
                     ss << std::endl;
                     break;
                 case State::Execute:
                     ss << "Execute " << octzero(r.ma) << " " << octzero(r.ma) << ":";
                     ss << std::endl;
                     break;
                     */
                 }
             }
             return ss.str();
         }
}
