#ifndef PDP11_CPU_H
#define PDP11_CPU_H

#include "includes.h"
#include <array>
const int bootrom[]{
    0042113,                        /* "KD" */
    0012706, 02000,                 /* MOV #boot_start, SP */
    0012700, 0000000,               /* MOV #unit, R0        ; unit number */
    0010003,                        /* MOV R0, R3 */
    0000303,                        /* SWAB R3 */
    0006303,                        /* ASL R3 */
    0006303,                        /* ASL R3 */
    0006303,                        /* ASL R3 */
    0006303,                        /* ASL R3 */
    0006303,                        /* ASL R3 */
    0012701, 0177412,               /* MOV #RKDA, R1        ; csr */
    0010311,                        /* MOV R3, (R1)         ; load da */
    0005041,                        /* CLR -(R1)            ; clear ba */
    0012741, 0177000,               /* MOV #-256.*2, -(R1)  ; load wc */
    0012741, 0000005,               /* MOV #READ+GO, -(R1)  ; read & go */
    0005002,                        /* CLR R2 */
    0005003,                        /* CLR R3 */
    0012704, 02020,                 /* MOV #START+20, R4 */
    0005005,                        /* CLR R5 */
    0105711,                        /* TSTB (R1) */
    0100376,                        /* BPL .-2 */
    0105011,                        /* CLRB (R1) */
    0005007                         /* CLR PC */
};
inline bool bxor(bool a, bool b) { return (a || b) && !(a && b); }




void writedebug(const char* fmt,...);

class PDP11;
class PDP11Device {
protected:
    PDP11& cpu;
public:
    PDP11Device(PDP11& cpu) : cpu(cpu) {}
    virtual int read16(int addr) = 0;
    virtual void write16(int addr, int data) =0;
    virtual void reset() = 0;
    virtual ~PDP11Device() {}
};




class PDP11 {
    class Trap : public std::exception {
        int _trap;
        int _data;
    public:
        Trap(int trap,int data) : std::exception(), _trap(trap),_data(data) {}
        Trap(int trap,int data,const char* msg) : std::exception(msg), _trap(trap),_data(data) {}
        int trap() const { return _trap;}
        int data() const { return _data;}
    };
    struct Page{
        int par;
        int pdr;
        int addr;
        int len;
        bool read;
        bool write;
        bool ed;
        Page(int par, int pdr) : par(par),pdr(pdr),addr(par&07777),len((pdr>>8) &0x7F),read((pdr&2)!=0),write((pdr&6)!=0),ed((pdr&8)!=0){}
    };
    struct Interrupt {
        int vec;
        int pri;
        Interrupt(int vec, int pri) : vec(vec), pri(pri) {}
        bool operator<(const Interrupt& i) const { return i.pri < pri && i.vec >= vec ;}
    };
    int R[8];
    int KSP,USP,PS,curPC,instr,tim1, tim2,ips,SR0,SR2,LKS,clkcounter;
    bool curuser,prevuser;
    bool waiting;
    PDP11Device* rk05;
    PDP11Device* cons;


    /*
     *
     *
        if(vec & 1) panic("Thou darst calling interrupt() with an odd vector number?");
        for(int i=0;i<interrupts.length;i++) {
            if(interrupts[i].pri < pri)
                break;
        }
        for(;i<interrupts.length;i++) {
            if(interrupts[i].vec >= vec)
                break;
        }
        interrupts.splice(i, 0, {vec: vec, pri: pri});
     */
    std::list<Interrupt> interrupts; // CHANGE THIS to prioity queue or something
    std::priority_queue<Interrupt> interruptsq;
    Page pages[16];
    enum Traps {
        INTBUS     = 0004,
        INTINVAL   = 0010,
        INTDEBUG   = 0014,
        INTIOT     = 0020,
        INTTTYIN   = 0060,
        INTTTYOUT  = 0064,
        INTFAULT   = 0250,
        INTCLOCK   = 0100,
        INTRK      = 0220
    };
    enum Flags{
         FLAGN = 8,
         FLAGZ = 4,
         FLAGV = 2,
         FLAGC = 1,
    };
public:
     std::array<int,128*1024> memory;
    void switchmode(bool newm)
    {
        prevuser = curuser;
        curuser = newm;
        if(prevuser) USP = R[6];
        else KSP = R[6];
        if(curuser) R[6] = USP;
        else R[6] = KSP;
        PS &= 0007777;
        if(curuser) PS |= (1<<15)|(1<<14);
        if(prevuser) PS |= (1<<13)|(1<<12);
    }
    int physread16(int a)
    {
        if(a & 1) throw Trap(INTBUS,a);// "read from odd address " + ostr(a,6));
        if(a < 0760000) return memory[a>>1];
        if(a == 0777546) return LKS;
        if(a == 0777570) return 0173030;
        if(a == 0777572) return SR0;
        if(a == 0777576) return SR2;
        if(a == 0777776) return PS;
        if((a & 0777770) == 0777560) return cons->read16(a);
        if((a & 0777760) == 0777400) return rk05->read16(a);
        if((a & 0777600) == 0772200 || (a & 0777600) == 0777600) return mmuread16(a);
        if(a == 0776000) std::runtime_error("lolwut");
        throw Trap(INTBUS,6); //, "read from invalid address " + ostr(a,6));
    }
    int physread8(int a)
    {
        int val= physread16(a & ~1);
        if(a & 1) return val >> 8;
        return val & 0xFF;
    }
    void physwrite8(int a,int v)
    {
        if(a < 0760000) {
            if(a & 1) {
                memory[a>>1] &= 0xFF;
                memory[a>>1] |= (v & 0xFF) << 8;
            } else {
                memory[a>>1] &= 0xFF00;
                memory[a>>1] |= v & 0xFF;
            }
        } else {
            if(a & 1) {
                physwrite16(a&~1, (physread16(a) & 0xFF) | ((v & 0xFF) << 8));
            } else {
                physwrite16(a&~1, (physread16(a) & 0xFF00) | (v & 0xFF));
            }
        }
    }
    void physwrite16(int a,int v)
    {
        if(a % 1) throw Trap(INTBUS,a); // "write to odd address " + ostr(a,6));
        if(a < 0760000) memory[a>>1] = v;
        else if(a == 0777776) {
            switch(v >> 14) {
            case 0: switchmode(false); break;
            case 3: switchmode(true); break;
            default: panic("invalid mode");
            }
            switch((v >> 12) & 3) {
            case 0: prevuser = false; break;
            case 3: prevuser = true; break;
            default: panic("invalid mode");
            }
            PS = v;
        }
        else if(a == 0777546) LKS = v;
        else if(a == 0777572) SR0 = v;
        else if((a & 0777770) == 0777560) cons->write16(a,v);
        else if((a & 0777700) == 0777400) rk05->write16(a,v);
        else if((a & 0777600) == 0772200 || (a & 0777600) == 0777600) mmuwrite16(a,v);
        else throw Trap(INTBUS,a); // "write to invalid address " + ostr(a,6));
    }
    int decode(int a,bool w,bool m)
    {
        int  block, disp;
        if(!(SR0 & 1)) {
            if(a >= 0170000) a += 0600000;
            return a;
        }
        int user = m ? 8 : 0;
        Page &p = pages[(a >> 13) + user];
        if(w && !p.write) {
            SR0 = (1<<13) | 1;
            SR0 |= (a >> 12) & ~1;
            if(user) SR0 |= (1<<5)|(1<<6);
            SR2 = curPC;
            throw Trap(INTFAULT,a);// "write to read-only page " + ostr(a,6));
        }
        if(!p.read) {
            SR0 = (1<<15) | 1;
            SR0 |= (a >> 12) & ~1;
            if(user) SR0 |= (1<<5)|(1<<6);
            SR2 = curPC;
            throw Trap(INTFAULT, a);//"read from no-access page " + ostr(a,6));
        }
        block = (a >> 6) & 0177;
        disp = a & 077;
        if(p.ed ? (block < p.len) : (block > p.len)) {
            SR0 = (1<<14) | 1;
            SR0 |= (a >> 12) & ~1;
            if(user) SR0 |= (1<<5)|(1<<6);
            SR2 = curPC;
            throw Trap(INTFAULT, a);//"page length exceeded, address " + ostr(a,6) + " (block " + ostr(block,3) + ") is beyond length " + ostr(p.len,3));
        }
        if(w) p.pdr |= 1<<6;
        return ((block + p.addr) << 6) + disp;
    }

    int mmuread16(int a)
    {
        int i;
        i = (a & 017)>>1;
        if((a >= 0772300) && (a < 0772320))
            return pages[i].pdr;
        if((a >= 0772340) && (a < 0772360))
            return pages[i].par;
        if((a >= 0777600) && (a < 0777620))
            return pages[i+8].pdr;
        if((a >= 0777640) && (a < 0777660))
            return pages[i+8].par;
        throw Trap(INTBUS, a); //"invalid read from " + ostr(a,6));
    }
    void mmuwrite16(int a, int v)
    {
        int i = (a & 017)>>1;
        if((a >= 0772300) && (a < 0772320)) {
            pages[i] = Page(pages[i].par, v);
            return;
        }
        if((a >= 0772340) && (a < 0772360)) {
            pages[i] = Page(v, pages[i].pdr);
            return;
        }
        if((a >= 0777600) && (a < 0777620)) {
            pages[i+8] = Page(pages[i+8].par, v);
            return;
        }
        if((a >= 0777640) && (a < 0777660)) {
            pages[i+8] = Page(v, pages[i+8].pdr);
            return;
        }
        throw Trap(INTBUS, a); //"write to invalid address " + ostr(a,6));
    }
    int
    read8(int a)
    {
        return physread8(decode(a, false, curuser));
    }

    int
    read16(int a)
    {
        return physread16(decode(a, false, curuser));
    }

    void
    write8(int a,int v)
    {
        return physwrite8(decode(a, true, curuser),v);
    }

    void
    write16(int a,int v)
    {
        return physwrite16(decode(a, true, curuser),v);
    }
    int
    fetch16()
    {
        int val = read16(R[7]);
        R[7] += 2;
        return val;
    }

    void
    push(int v)
    {
        R[6] -= 2;
        write16(R[6], v);
    }

    int
    pop()
    {
        int val = read16(R[6]); //, v);
        R[6] += 2;
        return val;
    }
    std::string&& disam(int addr) {
        std::string ret("BUG");
        return std::move(ret);
    }

    std::string printstate()
    {
        std::stringstream ss;
        ss  << "R0 " << ostr(R[0]) << " "
            << "R1 " << ostr(R[1]) << " "
            << "R2 " << ostr(R[2]) << " "
            << "R3 " << ostr(R[3]) << " "
            << "R4 " << ostr(R[4]) << " "
            << "R5 " << ostr(R[5]) << " "
            << "R6 " << ostr(R[6]) << " "
            << "R7 " << ostr(R[7]) << std::endl;
        ss << "[";
        ss << prevuser ? "u" : "k";
        ss << curuser ? "U" : "K";
        ss << (PS & FLAGN) ? "N" : "-";
        ss << (PS & FLAGZ) ? "Z" : "-";
        ss << (PS & FLAGV) ? "V" : "-";
        ss << (PS & FLAGC) ? "C" : "-";
        ss << "]";
        ss << "   instr " << ostr(curPC) << ": " << ostr(instr) << "    ";
        try {
            ss << disam(decode(curPC,false,curuser));
        } catch(const std::exception& e) {
            (void)e;
        }
        ss << std::endl;
        return ss.str();
    }
    void panic(const char*msg)
    {
        writedebug(msg);
        writedebug(printstate().c_str());
        //stop();
        throw std::runtime_error(msg);
    }
    void stop() {
        throw std::runtime_error("HALT");
    }

    void interrupt(int vec, int pri)
    {
        if(vec & 1) panic("Thou darst calling interrupt() with an odd vector number?");
        auto i =interrupts.begin();
        for(;i!=interrupts.end();i++) {
            if(i->pri < pri)
                break;
        }
         for(;i!=interrupts.end();i++) {
            if(i->vec >= vec)
                break;
        }
         interrupts.emplace(i,vec,pri);
         interruptsq.emplace(vec,pri);
    }
    void handleinterrupt(int vec)
    {
        int prev = 0;
        try {
            prev = PS;
            switchmode(false);
            push(prev);
            push(R[7]);
        } catch(const Trap& e) {
             trapat(e.trap(), e.what());
        } catch(const std::exception& e) {
            throw e;
        }

        R[7] = memory[vec>>1];
        PS = memory[(vec>>1)+1];
        if(prevuser) PS |= (1<<13)|(1<<12);
        waiting = false;
    }
    void trapat(int vec,const char* msg)
    {
        int prev = 0;
        if(vec & 1) panic("Thou darst calling trapat() with an odd vector number?");
        writedebug("trap %6o occured: %s",vec,msg);
        writedebug(printstate().c_str());
        try {
            prev = PS;
            switchmode(false);
            push(prev);
            push(R[7]);
        } catch(const Trap& e) {
            (void)e; // important to know we though it and not the os
            writedebug("red stack trap!");
            memory[0] = R[7];
            memory[1] = prev;
            vec = 4;

        } catch(const std::exception& e) {
            throw e;
        }
        R[7] = memory[vec>>1];
        PS = memory[(vec>>1)+1];
        if(prevuser) PS |= (1<<13)|(1<<12);
        waiting = false;
    }
    int aget(int v,int l)
    {
        int addr;
        if((v & 7) >= 6 || (v & 010)) l = 2;
        if((v & 070) == 000) {
            return -(v + 1);
        }
        switch(v & 060) {
        case 000:
            v &= 7;
            addr = R[v & 7];
            break;
        case 020:
            addr = R[v & 7];
            R[v & 7] += l;
            break;
        case 040:
            R[v & 7] -= l;
            addr = R[v & 7];
            break;
        case 060:
            addr = fetch16();
            addr += R[v & 7];
            break;
        }
        addr &= 0xFFFF;
        if(v & 010) {
            addr = read16(addr);
        }
        return addr;
    }
    int memread(int a,int l)
    {
        if(a < 0) {
            if(l == 2)
                return R[-(a + 1)];
            else
                return R[-(a + 1)] & 0xFF;
        }
        if(l == 2)
            return read16(a);
        return read8(a);
    }
    void memwrite(int a,int l,int v)
    {
        if(a < 0) {
            if(l == 2)
                R[-(a + 1)] = v;
            else {
                R[-(a + 1)] &= 0xFF00;
                R[-(a + 1)] |= v;
            }
        } else if(l == 2)
            write16(a, v);
        else
            write8(a, v);
    }
    void branch(int o)
    {
        if(o & 0x80) o = -(((~o)+1)&0xFF);
        o <<= 1;
        R[7] += o;
    }
    void step()
    {
        int val, val1, val2, ia, da, sa, d, s, l,  o, max, maxp, msb;
        ips++;
        if(waiting) return;
        curPC = R[7];
        ia = decode(R[7], false, curuser);
        R[7] += 2;
       // lastPCs = lastPCs.slice(0,100);
      //  lastPCs.splice(0, 0, ia);
        instr = physread16(ia);
        d = instr & 077;
        s = (instr & 07700) >> 6;
        l = 2 - (instr >> 15);
        o = instr & 0xFF;
        if(l == 2) {
            max = 0xFFFF;
            maxp = 0x7FFF;
            msb = 0x8000;
        }
        else {
            max = 0xFF;
            maxp = 0x7F;
            msb = 0x80;
        }
        switch(instr & 0070000) {
        case 0010000: // MOV
            sa = aget(s, l); val = memread(sa, l);
            da = aget(d, l);
            PS &= 0xFFF1;
            if(val & msb) PS |= FLAGN;
            if(val == 0) PS |= FLAGZ;
            if(da < 0 && l == 1) {
                l = 2;
                if(val & msb) val |= 0xFF00;
            }
            memwrite(da, l, val);
            return;
        case 0020000: // CMP
            sa = aget(s, l); val1 = memread(sa, l);
            da = aget(d, l); val2 = memread(da, l);
            val = (val1 - val2) & max;
            PS &= 0xFFF0;
            if(val == 0) PS |= FLAGZ;
            if(val & msb) PS |= FLAGN;
            if(((val1 ^ val2) & msb) && !((val2 ^ val) & msb)) PS |= FLAGV;
            if(val1 < val2) PS |= FLAGC;
            return;
        case 0030000: // BIT
            sa = aget(s, l); val1 = memread(sa, l);
            da = aget(d, l); val2 = memread(da, l);
            val = val1 & val2;
            PS &= 0xFFF1;
            if(val == 0) PS |= FLAGZ;
            if(val & msb) PS |= FLAGN;
            return;
        case 0040000: // BIC
            sa = aget(s, l); val1 = memread(sa, l);
            da = aget(d, l); val2 = memread(da, l);
            val = (max ^ val1) & val2;
            PS &= 0xFFF1;
            if(val == 0) PS |= FLAGZ;
            if(val & msb) PS |= FLAGN;
            memwrite(da, l, val);
            return;
        case 0050000: // BIS
            sa = aget(s, l); val1 = memread(sa, l);
            da = aget(d, l); val2 = memread(da, l);
            val = val1 | val2;
            PS &= 0xFFF1;
            if(val == 0) PS |= FLAGZ;
            if(val & msb) PS |= FLAGN;
            memwrite(da, l, val);
            return;
        }
        switch(instr & 0170000) {
        case 0060000: // ADD
            sa = aget(s, 2); val1 = memread(sa, 2);
            da = aget(d, 2); val2 = memread(da, 2);
            val = (val1 + val2) & 0xFFFF;
            PS &= 0xFFF0;
            if(val == 0) PS |= FLAGZ;
            if(val & 0x8000) PS |= FLAGN;
            if(!((val1 ^ val2) & 0x8000) && ((val2 ^ val) & 0x8000)) PS |= FLAGV;
            if(val1 + val2 >= 0xFFFF) PS |= FLAGC;
            memwrite(da, 2, val);
            return;
        case 0160000: // SUB
            sa = aget(s, 2); val1 = memread(sa, 2);
            da = aget(d, 2); val2 = memread(da, 2);
            val = (val2 - val1) & 0xFFFF;
            PS &= 0xFFF0;
            if(val == 0) PS |= FLAGZ;
            if(val & 0x8000) PS |= FLAGN;
            if(((val1 ^ val2) & 0x8000) && !((val2 ^ val) & 0x8000)) PS |= FLAGV;
            if(val1 > val2) PS |= FLAGC;
            memwrite(da, 2, val);
            return;
        }
        switch(instr & 0177000) {
        case 0004000: // JSR
            val = aget(d, l);
            if(val < 0) break;
            push(R[s & 7]);
            R[s & 7] = R[7];
            R[7] = val;
            return;
        case 0070000: // MUL
            val1 = R[s & 7];
            if(val1 & 0x8000) val1 = -((0xFFFF^val1)+1);
            da = aget(d, l); val2 = memread(da, 2);
            if(val2 & 0x8000) val2 = -((0xFFFF^val2)+1);
            val = val1 * val2;
            R[s & 7] = (val & 0xFFFF0000) >> 16;
            R[(s & 7)|1] = val & 0xFFFF;
            PS &= 0xFFF0;
            if(val & 0x80000000) PS |= FLAGN;
            if((val & 0xFFFFFFFF) == 0) PS |= FLAGZ;
            if(val < (1<<15) || val >= ((1<<15)-1)) PS |= FLAGC;
            return;
        case 0071000: // DIV
            val1 = (R[s & 7] << 16) | R[(s & 7) | 1];
            da = aget(d, l); val2 = memread(da, 2);
            PS &= 0xFFF0;
            if(val2 == 0) {
                PS |= FLAGC;
                return;
            }
            if((val1 / val2) >= 0x10000){
                PS |= FLAGV;
                return;
            }
            R[s & 7] = (val1 / val2) & 0xFFFF;
            R[(s & 7) | 1] = (val1 % val2) & 0xFFFF;
            if(R[s & 7] == 0) PS |= FLAGZ;
            if(R[s & 7] & 0100000) PS |= FLAGN;
            if(val1 == 0) PS |= FLAGV;
            return;
        case 0072000: // ASH
            val1 = R[s & 7];
            da = aget(d, 2); val2 = memread(da, 2) & 077;
            PS &= 0xFFF0;
            if(val2 & 040) {
                val2 = (077 ^ val2) + 1;
                if(val1 & 0100000) {
                    val = 0xFFFF ^ (0xFFFF >> val2);
                    val |= val1 >> val2;
                } else
                    val = val1 >> val2;
                if(val1 & (1 << (val2 - 1))) PS |= FLAGC;
            } else {
                val = (val1 << val2) & 0xFFFF;
                if(val1 & (1 << (16 - val2))) PS |= FLAGC;
            }
            R[s & 7] = val;
            if(val == 0) PS |= FLAGZ;
            if(val & 0100000) PS |= FLAGN;
            if(bxor(val & 0100000, val1 & 0100000)) PS |= FLAGV;
            return;
        case 0073000: // ASHC
            val1 = (R[s & 7] << 16) | R[(s & 7) | 1];
            da = aget(d, 2); val2 = memread(da, 2) & 077;
            PS &= 0xFFF0;
            if(val2 & 040) {
                val2 = (077 ^ val2) + 1;
                if(val1 & 0x80000000) {
                    val = 0xFFFFFFFF ^ (0xFFFFFFFF >> val2);
                    val |= val1 >> val2;
                } else
                    val = val1 >> val2;
                if(val1 & (1 << (val2 - 1))) PS |= FLAGC;
            } else {
                val = (val1 << val2) & 0xFFFFFFFF;
                if(val1 & (1 << (32 - val2))) PS |= FLAGC;
            }
            R[s & 7] = (val >> 16) & 0xFFFF;
            R[(s & 7)|1] = val & 0xFFFF;
            if(val == 0) PS |= FLAGZ;
            if(val & 0x80000000) PS |= FLAGN;
            if(bxor(val & 0x80000000, val1 & 0x80000000)) PS |= FLAGV;
            return;
        case 0074000: // XOR
            val1 = R[s & 7];
            da = aget(d, 2); val2 = memread(da, 2);
            val = val1 ^ val2;
            PS &= 0xFFF1;
            if(val == 0) PS |= FLAGZ;
            if(val & 0x8000) PS |= FLAGZ;
            memwrite(da, 2, val);
            return;
        case 0077000: // SOB
            if(--R[s & 7]) {
                o &= 077;
                o <<= 1;
                R[7] -= o;
            }
            return;
        }
        switch(instr & 0077700) {
        case 0005000: // CLR
            PS &= 0xFFF0;
            PS |= FLAGZ;
            da = aget(d, l);
            memwrite(da, l, 0);
            return;
        case 0005100: // COM
            da = aget(d, l);
            val = memread(da, l) ^ max;
            PS &= 0xFFF0; PS |= FLAGC;
            if(val & msb) PS |= FLAGN;
            if(val == 0) PS |= FLAGZ;
            memwrite(da, l, val);
            return;
        case 0005200: // INC
            da = aget(d, l);
            val = (memread(da, l) + 1) & max;
            PS &= 0xFFF1;
            if(val & msb) PS |= FLAGN | FLAGV;
            if(val == 0) PS |= FLAGZ;
            memwrite(da, l, val);
            return;
        case 0005300: // DEC
            da = aget(d, l);
            val = (memread(da, l) - 1) & max;
            PS &= 0xFFF1;
            if(val & msb) PS |= FLAGN;
            if(val == maxp) PS |= FLAGV;
            if(val == 0) PS |= FLAGZ;
            memwrite(da, l, val);
            return;
        case 0005400: // NEG
            da = aget(d, l);
            val = (-memread(da, l)) & max;
            PS &= 0xFFF0;
            if(val & msb) PS |= FLAGN;
            if(val == 0) PS |= FLAGZ;
            else PS |= FLAGC;
            if(val == 0x8000) PS |= FLAGV;
            memwrite(da, l, val);
            return;
        case 0005500: // ADC
            da = aget(d, l);
            val = memread(da, l);
            if(PS & FLAGC) {
                PS &= 0xFFF0;
                if((val + 1) & msb) PS |= FLAGN;
                if(val == max) PS |= FLAGZ;
                if(val == 0077777) PS |= FLAGV;
                if(val == 0177777) PS |= FLAGC;
                memwrite(da, l, (val+1) & max);
            } else {
                PS &= 0xFFF0;
                if(val & msb) PS |= FLAGN;
                if(val == 0) PS |= FLAGZ;
            }
            return;
        case 0005600: // SBC
            da = aget(d, l);
            val = memread(da, l);
            if(PS & FLAGC) {
                PS &= 0xFFF0;
                if((val - 1) & msb) PS |= FLAGN;
                if(val == 1) PS |= FLAGZ;
                if(val) PS |= FLAGC;
                if(val == 0100000) PS |= FLAGV;
                memwrite(da, l, (val-1) & max);
            } else {
                PS &= 0xFFF0;
                if(val & msb) PS |= FLAGN;
                if(val == 0) PS |= FLAGZ;
                if(val == 0100000) PS |= FLAGV;
                PS |= FLAGC;
            }
            return;
        case 0005700: // TST
            da = aget(d, l);
            val = memread(da, l);
            PS &= 0xFFF0;
            if(val & msb) PS |= FLAGN;
            if(val == 0) PS |= FLAGZ;
            return;
        case 0006000: // ROR
            da = aget(d, l);
            val = memread(da, l);
            if(PS & FLAGC) val |= max+1;
            PS &= 0xFFF0;
            if(val & 1) PS |= FLAGC;
            if(val & (max+1)) PS |= FLAGN;
            if(!(val & max)) PS |= FLAGZ;
            if(bxor(val & 1, val & (max+1))) PS |= FLAGV;
            val >>= 1;
            memwrite(da, l, val);
            return;
        case 0006100: // ROL
            da = aget(d, l);
            val = memread(da, l) << 1;
            if(PS & FLAGC) val |= 1;
            PS &= 0xFFF0;
            if(val & (max+1)) PS |= FLAGC;
            if(val & msb) PS |= FLAGN;
            if(!(val & max)) PS |= FLAGZ;
            if((val ^ (val >> 1)) & msb) PS |= FLAGV;
            val &= max;
            memwrite(da, l, val);
            return;
        case 0006200: // ASR
            da = aget(d, l);
            val = memread(da, l);
            PS &= 0xFFF0;
            if(val & 1) PS |= FLAGC;
            if(val & msb) PS |= FLAGN;
            if(bxor(val & msb, val & 1)) PS |= FLAGV;
            val = (val & msb) | (val >> 1);
            if(val == 0) PS |= FLAGZ;
            memwrite(da, l, val);
            return;
        case 0006300: // ASL
            da = aget(d, l);
            val = memread(da, l);
            PS &= 0xFFF0;
            if(val & msb) PS |= FLAGC;
            if(val & (msb >> 1)) PS |= FLAGN;
            if((val ^ (val << 1)) & msb) PS |= FLAGV;
            val = (val << 1) & max;
            if(val == 0) PS |= FLAGZ;
            memwrite(da, l, val);
            return;
        case 0006700: // SXT
            da = aget(d, l);
            if(PS & FLAGN) {
                memwrite(da, l, max);
            } else {
                PS |= FLAGZ;
                memwrite(da, l, 0);
            }
            return;
        }
        switch(instr & 0177700) {
        case 0000100: // JMP
            val = aget(d, 2);
            if(val < 0) {
                break;
            }
            R[7] = val;
            return;
        case 0000300: // SWAB
            da = aget(d, l);
            val = memread(da, l);
            val = ((val >> 8) | (val << 8)) & 0xFFFF;
            PS &= 0xFFF0;
            if((val & 0xFF) == 0) PS |= FLAGZ;
            if(val & 0x80) PS |= FLAGN;
            memwrite(da, l, val);
            return;
        case 0006400: // MARK
            R[6] = R[7] + (instr & 077) << 1;
            R[7] = R[5];
            R[5] = pop();
            break;
        case 0006500: // MFPI
            da = aget(d, 2);
            if(da == -7)
                val = (curuser == prevuser) ? R[6] : (prevuser ? USP : KSP);
            else if(da < 0)
                panic("invalid MFPI instruction");
            else
                val = physread16(decode(da, false, prevuser));
            push(val);
            PS &= 0xFFF0; PS |= FLAGC;
            if(val == 0) PS |= FLAGZ;
            if(val & 0x8000) PS |= FLAGN;
            return;
        case 0006600: // MTPI
            da = aget(d, 2);
            val = pop();
            if(da == -7) {
                if(curuser == prevuser) R[6] = val;
                else if(prevuser) USP = val;
                else KSP = val;
            } else if(da < 0)
                panic("invalid MTPI instrution");
            else {
                sa = decode(da, true, prevuser);
                physwrite16(sa, val);
            }
            PS &= 0xFFF0; PS |= FLAGC;
            if(val == 0) PS |= FLAGZ;
            if(val & 0x8000) PS |= FLAGN;
            return;
        }
        if((instr & 0177770) == 0000200) { // RTS
            R[7] = R[d & 7];
            R[d & 7] = pop();
            return;
        }
        switch(instr & 0177400) {
        case 0000400: branch(o); return;
        case 0001000: if(!(PS & FLAGZ)) branch(o); return;
        case 0001400: if(PS & FLAGZ) branch(o); return;
        case 0002000: if(!bxor(PS & FLAGN, PS & FLAGV)) branch(o); return;
        case 0002400: if(bxor(PS & FLAGN, PS & FLAGV)) branch(o); return;
        case 0003000: if(!bxor(PS & FLAGN, PS & FLAGV) && !(PS & FLAGZ)) branch(o); return;
        case 0003400: if(bxor(PS & FLAGN, PS & FLAGV) || (PS & FLAGZ)) branch(o); return;
        case 0100000: if(!(PS & FLAGN)) branch(o); return;
        case 0100400: if(PS & FLAGN) branch(o); return;
        case 0101000: if(!(PS & FLAGC) && !(PS & FLAGZ)) branch(o); return;
        case 0101400: if((PS & FLAGC) || (PS & FLAGZ)) branch(o); return;
        case 0102000: if(!(PS & FLAGV)) branch(o); return;
        case 0102400: if(PS & FLAGV) branch(o); return;
        case 0103000: if(!(PS & FLAGC)) branch(o); return;
        case 0103400: if(PS & FLAGC) branch(o); return;
        }
        if((instr & 0177000) == 0104000 || instr == 3 || instr == 4) { // EMT TRAP IOT BPT
            int vec, prev;
            if((instr & 0177400) == 0104000) vec = 030;
            else if((instr & 0177400) == 0104400) vec = 034;
            else if(instr == 3) vec = 014;
            else vec = 020;
            prev = PS;
            switchmode(false);
            push(prev);
            push(R[7]);
            R[7] = memory[vec>>1];
            PS = memory[(vec>>1)+1];
            if(prevuser) PS |= (1<<13)|(1<<12);
            return;
        }
        if((instr & 0177740) == 0240) { // CL?, SE?
            if(instr & 020) {
                PS |= instr & 017;
            } else {
                PS &= ~(instr & 017);
            }
            return;
        }
        switch(instr) {
        case 0000000: // HALT
            if(curuser) break;
            writedebug("HALT\n");
            printstate();
            stop();
            return;
        case 0000001: // WAIT
    //		stop();
    //		setTimeout('LKS |= 0x80; interrupt(INTCLOCK, 6); run();', 20); // FIXME, really
            if(curuser) break;
            waiting = true;
            return;
        case 0000002: // RTI
        case 0000006: // RTT
            R[7] = pop();
            val = pop();
            if(curuser) {
                val &= 047;
                val |= PS & 0177730;
            }
            physwrite16(0777776, val);
            return;
        case 0000005: // RESET
            if(curuser) return;
            cons->reset();
            rk05->reset();
            return;
        case 0170011: // SETD ; not needed by UNIX, but used; therefore ignored
            return;
        }
        throw Trap(INTINVAL, instr); //"invalid instruction");
    }
    void reset()
    {
        for(int i=0;i<7;i++) R[i] = 0;
        PS = 0;
        KSP = 0;
        USP = 0;
        curuser = false;
        prevuser = false;
        SR0 = 0;
        curPC = 0;
        instr = 0;
        ips = 0;
        LKS = 1<<7;
        for(size_t i=0;i<memory.size();i++) memory[i] = 0;
        for(int i=0;i<(sizeof(bootrom)/sizeof(int));i++) memory[01000+i] = bootrom[i];
        for(int i=0;i<16;i++) pages[i] = Page(0, 0);
        R[7] = 02002;
       // cleardebug();
        cons->reset();
        rk05->reset();
        clkcounter = 0;
        waiting = false;
    }
    void nsteps(int n, bool pr)
    {
        while(n--) {
            try {
                step();
                if(!interrupts.empty() && interrupts.front().pri >= ((PS >> 5) & 7)) {
                    handleinterrupt(interrupts.front().vec);
                    interrupts.pop_front();
                    interruptsq.pop();
                }
                clkcounter++;
                if(clkcounter >= 40000) {
                    clkcounter = 0;
                    LKS |= (1<<7);
                    if(LKS & (1<<6)) interrupt(INTCLOCK, 6);
                }
            } catch(const Trap& e) {
                    trapat(e.trap(), e.what());
            } catch(const std::exception& e) {

             throw e;
            }
            if(pr)
                printstate();
        }
    }
};
class PDP11_RK05 : public PDP11Device{
    int RKDS, RKER, RKCS, RKWC, RKBA, drive, sector, surface, cylinder, rkimg;
    std::vector<uint8_t> rkdisk;
    static const size_t imglen = 2077696;
    enum {
        RKOVR = (1<<14),
       RKNXD = (1<<7),
       RKNXC = (1<<6),
       RKNXS = (1<<5)

    };
public:
    PDP11_RK05(PDP11& cpu) : PDP11Device(cpu) {}
    int read16(int a) {
        switch(a) {
        case 0777400: return RKDS;
        case 0777402: return RKER;
        case 0777404: return RKCS | ((RKBA & 0x30000) >> 12);
        case 0777406: return RKWC;
        case 0777410: return RKBA & 0xFFFF;
        case 0777412: return (sector) | (surface << 4) | (cylinder << 5) | (drive << 13);
        }
        cpu.panic("invalid read");
    }
    void rknotready()
    {
       // document.getElementById('rkbusy').style.display = '';
        RKDS &= ~(1<<6);
        RKCS &= ~(1<<7);
    }
    void rkready()
    {
      //  document.getElementById('rkbusy').style.display = 'none';
        RKDS |= 1<<6;
        RKCS |= 1<<7;
    }
    void rkerror(int code)
    {
        const char* msg;
        rkready();
        RKER |= code;
        RKCS |= (1<<15) | (1<<14);
        switch(code) {
        case RKOVR: msg = "operation overflowed the disk"; break;
        case RKNXD: msg = "invalid disk accessed"; break;
        case RKNXC: msg = "invalid cylinder accessed"; break;
        case RKNXS: msg = "invalid sector accessed"; break;
        }
        cpu.panic(msg);
    }
    void rkrwsec(int t)
    {
        if(drive != 0) rkerror(RKNXD);
        if(cylinder > 0312) rkerror(RKNXC);
        if(sector > 013) rkerror(RKNXS);
        int pos = (cylinder * 24 + surface * 12 + sector) * 512;
        for(int i=0;i<256 && RKWC;i++) {
            if(t) {
                int val = cpu.memory[RKBA >> 1];
                rkdisk[pos] = val & 0xFF;
                rkdisk[pos+1] = (val >> 8) & 0xFF;
            }
            else
                cpu.memory[RKBA >> 1] = rkdisk[pos] | (rkdisk[pos+1] << 8);
            RKBA += 2;
            pos += 2;
            RKWC = (RKWC + 1) & 0xFFFF;
        }
        sector++;
        if(sector > 013) {
            sector = 0;
            surface++;
            if(surface > 1) {
                surface = 0;
                cylinder++;
                if(cylinder > 0312)
                    rkerror(RKOVR);
            }
        }
        //if(RKWC) setTimeout('rkrwsec('+t+')', 3);
        else {
            rkready();
     //       if(RKCS & (1<<6)) cpu.interrupt(INTRK, 5);
        }
    }

};
#endif // PDP11_CPU_H
