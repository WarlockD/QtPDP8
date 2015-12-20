#include "pdp11_cpu.h"
#include <iomanip>
void writedebug(const char* fmt,...) {
    char buffer[256];
    va_list va;
    va_start(va,fmt);
    vsnprintf(buffer,256,fmt,va);
    va_end(va);
    qDebug() << QString::fromLocal8Bit(buffer);
}
void writedebug(const std::string& sr) {
    qDebug() << QString::fromStdString(sr);
}
std::ostream& operator<<(std::ostream& s, const ostr& o){
    std::ios::fmtflags f(s.flags());
    s << std::setfill('0') << std::setw(o.n) << std::oct << o.z;
    s.flags(f);
    return s;
}
