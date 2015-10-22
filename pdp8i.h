#ifndef PDP8I_H
#define PDP8I_H

#include <iostream>
#include <cassert>
#include <thread>
#include <queue>
#include <chrono>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <mutex>          // std::mutex
#include <QDebug>

namespace PDP8 {

    class TwelveBit { // right now just for adder tester
        short _d  : 12;
    public:
        template<typename T> inline TwelveBit(T v) : _d(static_cast<short>(v)) {}
#define AOPR(b) template<typename T> inline TwelveBit operator b(T a) { _d b static_cast<short>(a); return *this; }  inline TwelveBit operator b(TwelveBit a) { _d b a._d; return *this; }
        AOPR(=) AOPR(+=)AOPR(-=) AOPR(|=) AOPR(&=) AOPR(^=)
#undef AOPR


    };
typedef std::vector<unsigned short> PDP8Memory;
typedef int ByteDelegate(int i);
//pbf[pbp]=pc+ifl;				// Looped buffer holding last 16 cycles for debug
//ibf[pbp]=mem[pc+ifl];
//abf[pbp]=ac;
//pbp=( pbp+1 )&15;
    enum class State {
        Fetch,
        Defer,
        Execute,
        Iot,
        Opr
    };


    struct PDP8Regs {
        unsigned short ac, pc, mq, sw, md, ma,xm;
    };
    struct RingBuffer {
        PDP8Regs buffer[16];
        size_t pos;
        void push(PDP8Regs& a) {
            buffer[pos] = a;
             pos=( pos+1 )&15;
        }

    };
    class PDP8_Exception : std::exception {
        std::string msg;
    public:
        PDP8_Exception(const std::string& msg) : msg(msg) {}
        const char* what() const noexcept { return msg.c_str(); }

    };

    enum class  BreakState {
        Running,
        UserBreak,
        ButtonStop,
        ButtonPause,
        ProgramBreak
    };
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
    class mem_ostream
    {
    public:
        mem_ostream() {}
        void close()
        {
            m_vec.clear();
        }
        const std::vector<char>& get_internal_vec()
        {
            return m_vec;
        }
        template<typename T>
        void write(const T& t)
        {
            std::vector<char> vec(sizeof(T));
            std::memcpy(reinterpret_cast<void*>(&vec[0]), reinterpret_cast<const void*>(&t), sizeof(T));
            write(vec);
        }
        void write(const char* p, size_t size)
        {
            for(size_t i=0; i<size; ++i)
                m_vec.push_back(p[i]);
        }

    private:
        std::vector<char> m_vec;
    };

// This class contains all the regesters of the PDP8 CPU as well as a helper disassembler


    // This is the main part of the cpu.
    struct PDP8_State {
        State state;
         std::mutex mtx; // healper mutex as it should of been created by the main thread
       // PDP8Memory mem;
        unsigned short mem[32768];
        // md is the "sense regester, or bus?  Not clear on it but it might be the holding
        // place comming out of core
        unsigned short ac, pc, mq, sw, mb, ma,xm,ir,md;
        unsigned char dfl,ifl;
        bool run;
        bool singleStep;
        bool interrupt_enable;
        bool interrupt_enable_delay;
        size_t intrupet_request; // one of these bits are set, then we have a request
        bool in_interrupt;
        bool checkInterrupt() const { return interrupt_enable && interrupt_enable_delay &&  intrupet_request !=0; } // temp
        bool skip;
        int delay;
        PDP8_State() { power();
                      // PDP8Memory.resize(32768);
                     }

        inline unsigned short& operator[](int v) { return mem[v]; }

        // when the power switch is turned, this is turned on
        // clear eveything
        inline void power() { ac = pc = mq = sw = mb = ma = ir = 0; run = false; state = State::Fetch; }

        // when the start key is pressed
        inline void start() { interrupt_enable = false;  ac = 0; run = true; }
        inline void setSR(int i) { sw = i; } // set the SR switch
        inline void loadAdd() {  pc = sw & 07777; dfl = ((sw>>12) & 07); ifl = ((sw>>15) & 07); }
        inline void dep() { mb = sw & 07777; mem[ma=pc] = mb;  pc = (pc+1) & 07777; }
        inline void exam() { mb = mem[ma=pc]; pc = (pc+1) & 07777; }
        inline void stop() { run = false; }
        inline void cont() { run = true; }
        inline void step() { run = false; singleStep = true; }

    };
                            struct Device {
                                virtual void clear() = 0;
                                virtual void power() = 0;
                                virtual void execute(PDP8_State& s) = 0;
                                virtual ~Device() {}
                            };
                            struct RF08 : public Device {
                                FileBuffer f;
                                virtual void clear() {}
                                virtual void power() {}
                                virtual void execute(PDP8_State& s) {}

                            };
     class Emx8 {
         bool debug_check;
         static const int SECSIZE = 256;
            static const int MEMSZ = 32768;
         //  int iot(int,int,int*);
                 //  void group3(int*,int*,int);
                 //  int group2(int,int,int);
                 //  int group1(int,int);
             public:
                 int cinf,coutf,pinf,poutf,rinbf,ttyinten1,doutf,dout,ttyinten2;
                 int ibus, intinh, intf, ilag;
                 int ibf[16]; // 16
                 int pbf[16];
                 int abf[16];
                 unsigned char ring[4096]; //4096
                 int rngi,rngo;
                 int pbp, dskrg,dskmem,dskad,dskfl,tm,hl,dskema, rfdn;
                 int ifr,ifl,dfr,dfl,svr,uflag,usint,phcell;
                 int rkwc,rkdn,rkca,rkcmd;
                 int rxrg,rxad,rxtr,rxdn,rxctr,rxp;
                 int rxbf[SECSIZE/2];
                 int rkda;
                 int vcix,vciy,vcflg;
                 int clken,clkfl,clkcnt;
                 int txinf,txoutf,txmode,txblk,txcnt;
                 FileBuffer rk05;
                 FileBuffer rf08;
                 FileBuffer df32;
                 FileBuffer ptr;
                 FileBuffer ptp;
                 FileBuffer rx50a;
                 FileBuffer rx50b;
                 FileBuffer rx50;
                 FileBuffer tx;


                 bool key_in;
                 unsigned char key;

                 std::queue<int> terminal_in;
                 std::queue<int> terminal_out;

             public:
                 int Pmask,dinf,Plen,dtyp,ExtTrm;
                 ByteDelegate* terminalOut;
                 ByteDelegate* terminalIn;
                 ByteDelegate* debugOut;
                PDP8_State* state;
             public:
                 Emx8() {

                     terminalOut = terminalIn = debugOut = nullptr;
                 }
             public:

                 void CPU_Init()
                     {
                     debug_check = false;
                     //	RedirectIOToConsole();
                            pbp=0;
                         ibus=intf=intinh=cinf=coutf=pinf=poutf=ilag=dinf=doutf=dout=0;
                         usint=clken=clkfl=clkcnt=0;
                         rkca=0;rkdn=0;rkda=0;rkcmd=0; // Set for read Diskad:0 -> mem[0]
                         dskrg=dskfl=dskema=rfdn=phcell=0;
                         rxrg=rxad=rxtr=rxdn=0;
                         txinf=txoutf=txmode=0;
                         rxdn=1;						 // Set initial done
                         ttyinten1=ttyinten2=-1;
                         vcix=vciy=vcflg=0;
                         svr&=0100;					 // Do not clear ts bit
                         rngi=rngo=0;					// Ring buf pointers for remote tty on code 40/41
                         printf("Init? printf\n");
                         //OldOut("Not allowed...RK8E\n");
                     }

             void XmInit(int freg)
                     {
                         ifr=freg&070;
                         ifl=ifr<<9;
                         dfr=(freg&07)<<3;
                         dfl=dfr<<9;
                     }

             void XtInit()
                     {
                         svr=usint=uflag=0;
                     }

              int GetRing()
                     {
                         int tm=ring[rngo];
                         if (rngi==rngo) return -1;
                         rngo=(rngo+1)&4095;
                         return tm;
                     }

              void OpenDevices(const char* Df32, const char* Rk05, const char* Ptr, const char* Ptp,int msk)
                     {
                    //     if (Df32)
                    //         df32=fopen(Df32,"r+b");
                      //   if (Rk05)
                       //      rk05=fopen(Rk05,"r+b");
                         /*
                         if (Ptr&&(msk&1))
                             ptr=fopen(Ptr,"rb");
                         if (Ptp&&(msk&2))
                             {ptp=fopen(Ptp,"wb");Plen=0;}
                         if (!df32 && !rf08) {
                             df32 = fopen("DF32.DSK", "r+b");
                         }
*/

                             rk05.openFile("F:\\Downloads\\diagpack2.rk05");// "r+b");

                     //	rf08=df32;
                     //	rx50=rx50a=fopen("os278wc.bin","r+b");
                     //	rx50b=fopen("dm101b1.bin","r+b");
                         //tx=fopen("tx.dsk","r+b");
                     }

              void CloseDevices(int msk)
                     {
                      //   if (df32) fclose(df32);
                         //if (rk05) fclose(rk05);
                      //   if (ptp&&(msk&2)) fclose(ptp);
                      //   if (ptr&&(msk&1)) fclose(ptr);
                    //     fclose(rx50a);
                     //    fclose(rx50b);
                  //      fclose(tx);
                     }

             void LoadBoot(unsigned short* mem,bool  Flg)
                     {
                         unsigned int i;
                         /* 4kMon DF32 boot */
                         int ldqr[]={06603,06622,05201,05604,07600};
                         /* RX50 BOOTSTRAP */
                         int ldqx[]={01061,01046,060,03060,07327,01061,06751,07301,04053,04053,07004,06755,05054,06754,07450,
                                 05020,01061,06751,01061,046,01032,03060,0360,04053,03002,02050,05047,0,06753,05033,06752,05453,0420,020};

                         for (tm=0;tm<MEMSZ;tm++) mem[tm]=07402;
                         for ( i=0; i<sizeof( ldqr )/sizeof(int); i++ ) {
                             mem[i+0200]=ldqr[i];
                         }
                         mem[07750]=07576;
                         mem[07751]=07576;

                         //	RK05 boot for OS/8
                         mem[030]=06743;
                         mem[031]=05031;

                     for (i=0;i<sizeof(ldqx)/sizeof(int);i++)
                             if (Flg) mem[i+020]=ldqx[i];
                     }

                     // This is the main simulator section


             // run this using std::thread
             void threadRun(PDP8_State* state) {
                 int vrix=0,vriy =0; // just for filler
                 int ret =0;
                 qDebug() << "Starting!";
                 for(;;) { // thread never stops

                     if(!state->run) std::this_thread::sleep_for(std::chrono::milliseconds(250)); // sleap for half a sec
                     else {
                         std::chrono::nanoseconds current_cycle = std::chrono::nanoseconds::zero();

                         if(state->singleStep) {
                             // single step
                             ret = Prun(*state,vrix,vriy,current_cycle);
                             state->run = false;
                         } else {
                             auto start = std::chrono::high_resolution_clock::now();
                             for(int i=0;i<10000;i++) {
                                 ret = Prun(*state,vrix,vriy,current_cycle);
                                 if(ret != 0 || !state->run) break;
                             }
                             auto end = std::chrono::high_resolution_clock::now();
                            // std::chrono::duration<double> diff = end-start;
                             auto mdiff = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
                             if(true) {
                                 qDebug() << "nanoseconds diffrence: " << mdiff.count();
                                  qDebug() << "current_cycle diffrence: " << current_cycle.count();
                             }
                             // debuging?
                             auto ydiff = std::chrono::duration_cast<std::chrono::nanoseconds>(current_cycle-mdiff);
                             if(true) {
                                 qDebug() << "nanoseconds Cycle diffrence: " << ydiff.count();
                             }
                             if(ydiff.count() >0)  std::this_thread::sleep_for(ydiff);

                             // longer run
                             if(ret !=0) state->run = false;
                         }


                    }
                 }
             }
             void runIt(PDP8_State& state) {
                 if(state.run) throw PDP8_Exception("Thread already running!");
                 std::thread th(&Emx8::threadRun,this,&state);
                 th.detach();
             }

             int Prun(PDP8_State& state,
                 int &vrix, int &vriy,std::chrono::nanoseconds& total_time)
                     {
                 static const std::chrono::nanoseconds memory_cycle_time(1500);
                  static const std::chrono::nanoseconds iot_cycle_time(4250);
                        int& delay = state.delay;
                        unsigned short& pc = state.pc;
                        unsigned short& ac = state.ac;
                        unsigned short& ma = state.ma;
                        unsigned short& md = state.mb;
                        unsigned short* mem = state.mem;
                        unsigned short& mq = state.mq;
                        unsigned short& swreg = state.sw;
                         int i;
                         int flg, reg=0, msk=0;
                       //  double *p = &bright[0];
                       //  unsigned short* pmem = &mem[0]; // Stops CLR from unexpectedly moving the arrays
                         //double filt=0.005;

                         ifl=state.xm;

                         if ( ++ilag>=delay) {
                             phcell=~phcell;
                            // reg=dsreg[regsel];
                             msk=040000;
                             /*
                             for (md=0;md<15;md++,p++) {
                                 if ((pc+ifl)&msk) *p+=(255.0-*p)*filt;
                                 else
                                     *p-=(*p)*filt;
                                 msk=msk>>1;
                             }
                             msk=04000;
                             for (md=0;md<12;md++,p++) {
                                 if (reg&msk) *p+=(255.0-*p)*filt;
                                 else
                                     *p-=(*p)*filt;
                                 msk=msk>>1;
                             }
                             */
                             if(!terminal_in.empty()){
                                 int nch = terminal_in.front();
                                 terminal_in.pop();
                                 cinf = toupper(nch) | 0200;
                                 if (cinf == 0200 + 5) { // ^E .. Halt
                                     for (i = 0; i<15; i++) {
                                         printf("%05o %04o %05o\n", pbf[pbp], ibf[pbp], abf[pbp]);
                                         pbp = (pbp + 1) & 15;
                                     }
                                   //  xpc = pc; xac = ac; xmq = mq;
                                     return (1);
                                 }
                                 if (cinf == 0200 + 8) cinf = 0377;
                                 if (cinf == 0200 + 24) cinf = 131; // Remap ^X->^C
                             }


                             ilag=0;
                         }

                         // 50 Hz clock ... set clock flag every 20000 cycles (av cycle=1us set by #cycles per call)

                         if ((++clkcnt==20000)&&clken) {
                             clkfl=1;
                             clkcnt=0;
                         }

                         // VC8E flag inc each cycle .. timeout set in iot
                         if (vcflg) vcflg++;

                         ibus=( dskfl||pinf||poutf||usint||clkfl );  // Read flags ? Int
                         ibus|=(cinf||coutf)&ttyinten1;
                         ibus|=(dinf||doutf)&ttyinten2;
                         // ibus|=dtyp&&phcell&&(dskrg&0200);		   // Sim RF08 photocell
                         ibus|=dtyp&&rfdn&&(dskrg&0100);		   // Sim RF08 int
                         ibus|=rkdn&&(rkcmd&0400);

                         if ( intf&&ibus ) {				// INTERRUPT
                             mem[0]=pc&07777;
                             pc=1;
                             intf=intinh=0;
                             svr=(ifl>>9)+(dfl>>12);
                             if (uflag==3) svr|=0100;
                             dfr=ifr=dfl=ifl=uflag=0;
                         }

                         pbf[pbp]=pc+ifl;				// Looped buffer holding last 16 cycles for debug
                         ibf[pbp]=mem[pc+ifl];
                         abf[pbp]=ac;
                         pbp=( pbp+1 )&15;


                         md=mem[pc+ifl];					// FETCH
                         ma=( ( md&0177 )+( md&0200?( pc&07600 ):0 ) )+ifl;
                         pc=( ( pc+1 )&07777 );
                         ac&=017777;
                        total_time += memory_cycle_time+memory_cycle_time;
                         if ((md&07000)<06000)			// DEFER
                              total_time += memory_cycle_time;
                             if ( md&0400 ) {
                                 if ( ( ma&07770 )==010 ) mem[ma]++;
                                 mem[ma]&=07777;
                                 if (md&04000)
                                     ma=mem[ma]+ifl;
                                 else
                                     ma=mem[ma]+dfl;
                             }
                             //   printf( "%04o %04o %05o : %05o %04o %04o\n", pc,md,ac,ma,ifl,dfl);

                            // dsreg[1]=04000 | ((md&07000)>>3);	// Build display reg (State)

                             switch ( md&07000 )			// EXECUTE
                             {
                             case 0000:

                                 ac&=( mem[ma]|010000 );
                                 break;
                             case 01000:
                                 assert(((static_cast<short>(ac) + (mem[ma]))&017777) == (((ac + mem[ma]))&017777));
                                 ac+=mem[ma];
                                 break;
                             case 02000:
                                 if (ma<MEMSZ)
                                     if ( ( mem[ma]=( 1+mem[ma] )&07777 )==0 ) pc++;
                                 break;
                             case 03000:
                                 if (ma<MEMSZ)
                                     mem[ma]=ac&07777;
                                 ac&=010000;
                                 break;
                             case 04000:
                                 ifl=ifr<<9;
                                 ma=(ma&07777)+ifl;
                                 if (ma<MEMSZ)
                                     mem[ma]=pc;
                                 pc=( ma+1 )&07777;
                                 intinh&=1;
                                 uflag|=2;
                                 break;
                             case 05000:
                                 pc= (ma&07777);
                                 ifl=ifr<<9;
                                 intinh&=1;
                                 uflag|=2;
                                 break;
                             case 06000:
                                 if (uflag==3) {usint=1;break;}
                                    total_time += memory_cycle_time;
                                 flg=0;
                                 if ((md&0707)==0205||(md&0707)==0206||(md&0707)==0207)
                                     dcm(mem, ac, md, pc);
                                 else
                                     ac=iot( mem, ac, md, flg, vrix, vriy );
                                 if ( flg ) pc++;
                                 break;
                             case 07000:
                                 if ( md&0400 ) {
                                     if ( md&1 ) {group3( ac, mq, md ); break; }
                                     pc=group2( ac, pc, md );
                                     if ( md&0200 ) ac&=010000;
                                     if ( md&4 ) { if (uflag==3) usint=1; else ac|=swreg; }
                                     if ( md&2 ) {
                                         if (uflag==3) {usint=1;break;}
                                         for (i=0;i<16;i++) {
                                             printf("%05o %04o %05o\n", pbf[pbp], ibf[pbp], abf[pbp]);
                                             pbp=(pbp+1)&15;
                                         }
                                       //  xpc=pc; xac=ac; xmq=mq;
                                         return (1);
                                     }
                                     break;

                                 }
                                 ac=group1( ac, md );
                                 break;
                             }
                             if (intinh==1 && md!=06001) intf=1;
                             reg=(ifl>>9)|(dfl>>12);
                             if (ac&010000) reg|=04000;
                             if (intf) reg|=0200;
                             if (ibus) reg|=01000;
                            // dsreg[2]=reg;
                           //  dsreg[3]=ac;
                           //  dsreg[4]=mem[pc+ifl];
                            // dsreg[5]=mq;
                           //  dsreg[6]=ac|mem[pc+ifl];
                           //  xpc=pc; xac=ac; xmq=mq; xmreg=ifl;
                             return(0);
                     }
                     int GetTerminalOut();

                     int iot(unsigned short* mem,int acc,int xmd,int &flg, int &vrix, int &vriy )
                     {
                         int i,j;
                         unsigned short *p;
                         unsigned short* xm = &mem[0]; // Stops CLR from unexpectedly moving the arrays
                         unsigned short rx5[SECSIZE / 2];
                         char iotbf[20];

                         sprintf(iotbf,"%04o",xmd);
                      //    qDebug() << iotbf;

                         if (ExtTrm) {
                          if ((xmd&0770)==030) xmd+=0350;
                          if ((xmd&0770)==040) xmd+=0350;
                         }

                         switch( xmd&0770 )
                         {
                         case 010:
                         case 020:
                             switch( xmd&0777 )
                             {
                             case 011: flg=pinf;break;
                             case 012: pinf=0;return( (acc&010000)|rinbf );
                             case 016: acc=(acc&010000)|rinbf;
                             case 014: if (ptr)
                                        //   if (!feof(ptr)) {
                                        //       rinbf=(Pmask&0200)?getc(ptr):getc(ptr)|0200;	// For 7 bit mode set parity bit
                                        //       pinf=1;
                                         //  }
                                          // printf(".");
                                           return(acc);
                             case 021: flg=poutf;break;
                             case 022: poutf=0;break;
                             case 026:/*
                             case 024: if (ptp)
                                       {
                                           if (terminalOut != nullptr)
                                               terminalOut(acc & 0377 & Pmask);
                                           else
                                             putc(acc&0377&Pmask,ptp);
                                           poutf=1;
                                           Plen++;
                                       }
                                       */
                                       break;
                             }
                         case 030:
                         case 040:
                             switch( xmd&0777 )
                             {
                             case 030:  cinf=0;break;
                             case 035:  ttyinten1=(acc&1)?-1:0;break;
                             case 031:  if ( cinf ) flg=1;break;
                             case 032:  cinf=0; acc&=010000;break;
                             case 034:  return( (acc&010000)|cinf );
                             case 036:  i=cinf; cinf=0; return( (acc&010000)|i );
                             case 040:  coutf=1; break;
                             case 041:  flg=coutf; break;
                             case 042:  coutf=0; break;
                             case 046:
                             case 044:
                                  state->mtx.lock();
                                 terminal_out.emplace(acc & 0177);
                                  state->mtx.unlock();
                                 coutf = 1; break;

                             }
                             break;
                         case 050:							// Minimalist VC8E implementation
                             switch( xmd&0777)
                             {
                             case 050:	vcflg=0;break;
                             case 051:	vcflg=0;break;
                             case 052:	if (vcflg>100) flg=1;break;
                             case 053:	vcix=(acc&01777)^01000;vcflg=1;break;
                             case 054:	vciy=(acc&01777)^01000;vcflg=1;break;
                             case 055:	vrix=vcix;vriy=vciy;break;
                             case 056:	break;
                             case 057:	acc=acc&010000;break;
                             }
                             break;
                         //case 0300:							// Serial disk handler
                         //case 0310:
                         //	switch (xmd&017)
                         //	{
                         //	case 001:	*flg=1;break;
                         //	case 006:
                         //		if (txmode==2) {
                         //			fread(&i,1,1,tx);
                         //			acc|=i&077;
                         //			txcnt--;
                         //		}
                         //		break;
                         //	case 011:	if (txoutf) *flg=1;break;
                         //	case 016:
                         //		txoutf=1;
                         //		if ((acc&0377)==0377) {
                         //			txmode=txcnt=0;
                         //			fflush(tx);
                         //			break;
                         //		}
                         //		switch (txmode) {
                         //	case 0: txblk=acc&077;
                         //			txmode=1;
                         //			break;
                         //	case 1:	txblk|=(acc&077)<<6;
                         //			fseek(tx,txblk*512,SEEK_SET);
                         //			txmode=2;
                         //			break;
                         //	case 2:	i=acc&077;
                         //			fwrite(&i,1,1,tx);
                         //			txcnt++;
                         //			break;
                         //		}
                         //		break;
                         //	}
                         //	break;
                         case 0400:							// Remote TTY via socket
                         case 0410:
                             switch( xmd&017 )
                             {
                             case 000:  dinf=0;break;
                             case 005:  ttyinten2=(acc&1)?-1:0;break;
                             case 001:  if ( dinf ) flg=1;break;
                             case 002:  dinf=0; acc&=010000;break;
                             case 004:  return( (acc&010000)|dinf );
                             case 006:  i=dinf; dinf=0; return( (acc&010000)|i );
                             case 010:  doutf=1; break;
                             case 011:  flg=doutf; break;
                             case 012:  doutf=0; break;
                             case 016:
                             case 014:  ring[rngi++]=acc&0377;rngi&=4095;doutf=1;break;
                             }
                             break;
                         case 0200:
                         case 0210:
                         case 0220:
                         case 0230:
                         case 0240:
                         case 0250:
                         case 0260:
                         case 0270:
                             switch(xmd&0777)
                             {
                             case 0204:
                                 usint=0;
                                 break;
                             case 0254:
                                 if (usint) flg=1;
                                 break;
                             case 0264:
                                 uflag=0;
                                 break;
                             case 0274:
                                 uflag=1;
                                 break;
                             case 0214:
                                 acc|=dfr;
                                 break;
                             case 0224:
                                 acc|=ifr;
                                 break;
                             case 0234:
                                 acc|=svr;
                                 break;
                             case 0244:
                                 dfr=(svr&07)<<3;
                                 dfl=dfr<<9;
                                 ifr=(svr&070);
                                 if (svr&0100) uflag=1;
                                 break;
                             }
                             switch(xmd&0707)
                             {
                             case 0201:
                                 dfr=xmd&070;
                                 dfl=dfr<<9;
                                 break;
                             case 0202:
                                 ifr=xmd&070;
                                 intinh|=2;
                                 break;
                             case 0203:
                                 dfr=xmd&070;
                                 ifr=xmd&070;
                                 dfl=dfr<<9;
                                 intinh|=2;
                                 break;
                             }
                             break;
                         case 0:
                             switch(xmd&0777)
                             {
                             case 000:	flg=intf;intf=intinh=0;
                                 break;
                             case 001:	intinh|=1;
                                 break;
                             case 002:	intf=intinh=0;
                                 break;
                             case 003:	if (ibus) flg=1;
                                 break;
                             case 004:	acc=(acc&010000)?014000:0;
                                 if (intinh&1) acc|=0200;
                                 if (ibus) acc|=01000;
                                 acc|=svr;
                                 break;
                             case 005:	intinh=3;
                                 acc&=07777;
                                 if (acc&04000) acc|=010000;
                                 svr=acc&0177;
                                 dfr=(svr&07)<<3;
                                 dfl=dfr<<9;
                                 ifr=(svr&070);
                                 if (svr&0100) uflag=1;
                                 break;
                             case 007:	acc=0;
                                 CPU_Init();
                                 break;
                             }
                             break;
                         case 0600:
                         case 0610:
                         case 0620:
                         case 0640:
                             if (!dtyp)
                                 switch(xmd&0777)
                             {
                                 case 0601:
                                     dskad=dskfl=0;
                                     break;
                                 case 0605:
                                 case 0603:
                                     i=(dskrg&070)<<9;
                                     dskmem=((mem[07751]+1)&07777)|i;  /* mem */
                                     tm=(dskrg&03700)<<6;
                                     dskad=(acc&07777)+tm;           /* dsk */
                                     i=010000-mem[07750];
                                     p=&xm[dskmem];
                                     df32.seekg(dskad*2);
         //							printf("DF32:%o>%o,",dskad,dskmem);
                                     if (xmd&2)              /*read */
                                         df32.read((char*)p,i*2);
                                     else
                                         df32.write((char*)p,i*2);
                                     dskfl=1;
                                     mem[07751]=0;
                                     mem[07750]=0;
                                     acc=acc&010000;
                                     break;
                                 case 0611:      dskrg=0;
                                     break;
                                 case 0615:      dskrg=(acc&07777);              /* register */
                                     break;
                                 case 0616:      acc=(acc&010000)|dskrg;
                                     break;
                                 case 0626:      acc=(acc&010000)+(dskad&07777);
                                     break;
                                 case 0622:      if (dskfl) flg=1;
                                     break;
                                 case 0612:      acc=acc&010000;
                                 case 0621:      flg=1; /* No error */
                                     break;
                             }
                             if (dtyp)
                                 switch(xmd&0777)
                             {
                                 case 0601:      dskad=rfdn=0;
                                     break;
                                 case 0605:
                                 case 0603:
                                     i=(dskrg&070)<<9;
                                     dskmem=((mem[07751]+1)&07777)|i;  /* mem */
                                     dskad=(acc&07777)|(dskema<<12); /* dsk */
                                     i=010000-mem[07750];
                                     p=&xm[dskmem];
                                     rf08.seekg(dskad*2);
                                     printf("RF08:%d>%o,", dskad / 128, dskmem);
                                     if (xmd&2)              /*read */
                                         rf08.read((char*)p,i*2);
                                     else
                                         rf08.write((char*)p,i*2);
                                     rfdn=1;
                                     mem[07751]=0;
                                     mem[07750]=0;
                                     acc=acc&010000;
                                     break;
                                 case 0611:      dskrg=0;
                                     break;
                                 case 0615:      dskrg=(acc&0770);              /* register */
                                     acc&=010000;
                                     break;
                                 case 0616:      acc=(acc&010000)|dskrg;
                                     acc|=04000&phcell;				// add photocell sync bit
                                     break;
                                 case 0626:      acc=(acc&010000)+(dskad&07777);
                                     break;
                                 case 0623:
                                 case 0622:      if (rfdn) flg=1;
                                     break;
                                 case 0612:      acc&=010000;
                                     flg=1; /* skip always */
                                 case 0621:      /* No error skip*/
                                     break;
                                 case 0641:		dskema=0;
                                     break;
                                 case 0643:		dskema=acc&0377;
                                     acc&=010000;
                                     break;
                                 case 0645:		acc=(acc&010000)|dskema;
                                     break;
                             }
                             break;
                         case 0740:
                             //	printf("Acc:%04o IOT:%04o\n",acc,xmd);
                             switch (xmd&7)
                             {
                             case 0:
                                 break;
                             case 1:
                                 if (rkdn) {flg=1;rkdn=0;}
                                 break;
                             case 2:
                                 acc&=010000;
                                 rkdn=0;
                                 break;
                             case 3:
                                 rkda=acc&07777;
                                 //
                                 // OS/8 Scheme. 2 virtual drives per physical drive
                                 // Regions start at 0 and 6260 (octal).
                                 //
                                 acc&=010000;
                                 if (rkcmd&6) {printf("Unit error\n");return acc;}
                                 switch (rkcmd&07000)
                                 {
                                 case 0:
                                 case 01000:
                                     rkca|=(rkcmd&070)<<9;
                                     rkwc=(rkcmd&0100)?128:256;
                                     rkda|=(rkcmd&1)?4096:0;
                                     // printf("Read Mem:%04o Dsk:%04o\n",rkca,rkda);
                                     //fseek(rk05,rkda*512,SEEK_SET);
                                     rk05.seekg(rkda*512);
                                     p=&xm[rkca];
                                     rk05.read((char*)p,2*rkwc);
                                    // fread(p,2,rkwc,rk05);
                                     rkca=(rkca+rkwc)&07777;
                                     rkdn++;
                                     // printf(".");
                                     break;
                                 case 04000:
                                 case 05000:
                                     rkca|=(rkcmd&070)<<9;
                                     rkwc=(rkcmd&0100)?128:256;
                                     rkda|=(rkcmd&1)?4096:0;
                                     // printf("Write Mem:%04o Dsk:%04o\n",rkca,rkda);
                                       rk05.seekg(rkda*512);
                                    // fseek(rk05,rkda*512,SEEK_SET);
                                     p=&xm[rkca];
                                      rk05.read((char*)p,2*rkwc);
                                     rkca=(rkca+rkwc)&07777;
                                     rkdn++;
                                     break;
                                 case 02000:
                                     break;
                                 case 03000:
                                     if (rkcmd&0200) rkdn++;
                                     break;
                                 }
                                 break;
                             case 4:
                                 rkca=acc&07777;
                                 acc&=010000;
                                 break;
                             case 5:
                                 //				acc=(acc&010000)|(rkdn?04000:0);
                                 acc=(acc&010000)|04000;
                                 break;
                             case 6:
                                 rkcmd=acc&07777;
                                 acc&=010000;
                                 break;
                             case 7:
                                 printf("Not allowed...RK8E\n");
                                 break;
                             }
                             break;
                         case 0130:
                             switch(xmd&0777)
                             {
                             case 0131:	clken=1;clkcnt=0;break;
                             case 0132:	clken=0;break;
                             case 0133:	if (clkfl) {
                                 clkfl=0;
                                 flg=1;
                                         }
                                         break;
                             }
                             break;
                         //case 0420:
                         //	switch(xmd&0777)
                         //	{
                         //	case 0425:
                         //	case 0424:	clken=1;clkcnt=clkfl=0;break;
                         //	case 0422:	clken=0;break;
                         //	case 0421:	if (clkfl) {
                         //		clkfl=0;
                         //		*flg=1;
                         //				}
                         //				break;
                         //	}
                         //	break;
                         case 0750:
                             switch(xmd&0777)
                                    {
                             case 0750:
                                 rx50=(acc&1)?rx50b:rx50a;
                                 break;			// 2 drives
                             case 0751: rxrg=acc&016;	// Cmd bits only
                                 acc=acc&010000;
                                 switch (rxrg)
                                 {
                                 case 0:
                                 case 2:
                                     rxctr=SECSIZE/2;			// always 12 bit mode
                                     rxtr=1;
                                     rxp=0;
                                     break;
                                 case 4:
                                 case 6:
                                     rxctr=2;			// expect track and sect only
                                     rxtr=1;
                                     break;
                                 case 014:
                                     break;
                                 case 010:
                                 case 012:
                                 case 016:
                                     rxdn=1;
                                     break;
                                 }
                                 break;
                             case 0752:
                                 switch (rxrg)
                                 {
                                 case 0:
                                     if (rxctr) {rxbf[rxp++]=acc&07777;rxtr=1;rxctr--;}
                                     if (rxctr==0) {rxdn++;rxtr=0;}
                                     break;
                                 case 2:
                                     if (rxctr) {acc=rxbf[rxp++]|(acc&010000);rxtr=1;rxctr--;}
                                     if (rxctr==0) {rxdn++;rxtr=0;}
                                     break;
                                 case 4:
                                 case 6:
                                     if (rxctr==1) {rxad=rxad+(acc&0377)*10;rxctr--;}  // Track 0-...
                                     if (rxctr==2) {rxad=(acc&0177)-1;rxtr=1;rxctr--;} // Sector 1-...
                                     if (rxctr==0) {
                                         rx50.seekg((rxad)*SECSIZE);
         //								printf("RX50:%d\n",rxad);
                                         //for (rxp=i=0;i<128;i++) {				// Pack
                                         //	if (i&1) {
                                         //		rxpk[rxp+1]|=rxbf[i] >> 8;
                                         //		rxpk[rxp+2]=rxbf[i] & 0377;
                                         //		rxp+=3;
                                         //	} else {
                                         //		rxpk[rxp]=rxbf[i] >> 4;
                                         //		rxpk[rxp+1]=(rxbf[i] << 4) & 0377;
                                         //	}
                                         //}
                                         for (rxp=0;rxp<SECSIZE/2;rxp++) rx5[rxp]=rxbf[rxp];
                                         if (rxrg==6) rx50.read((char*)rx5,1*SECSIZE);
                                         else rx50.write((char*)rx5,1*SECSIZE);
                                         for (rxp=0;rxp<SECSIZE/2;rxp++) rxbf[rxp]=rx5[rxp]&07777;
                                         //for (rxp=i=0;i<128;i++) {				//Unpack
                                         //	if (i&1) {
                                         //		rxbf[i]=((rxpk[rxp+1] << 8) | rxpk[rxp+2]) & 07777;;
                                         //		rxp+=3;
                                         //	} else {
                                         //		rxbf[i]=((rxpk[rxp] << 4) | (rxpk[rxp+1] >> 4)) & 07777;
                                         //	}
                                         //}
                                         rxdn++;
                                     }
                                     break;
                                 case 012:
                                     acc=(acc&010000)+0250;	// Drive ready + density bits = RX02
                                     break;
                                 case 016:
                                     acc&=010000;		// No errors
                                     break;
                                 }
                                 break;
                             case 0753:
                                 flg=rxtr;
                                 rxtr=0;
                                 break;
                             case 0754:
                                 j=0;
                                 break;
                             case 0755:
                                 flg=rxdn;
                                 rxdn=0;
                                 break;
                             case 0756:
                                 j=0;
                                 break;
                             case 0757:
                                 j=0;
                                 break;
                                    }
                                    break;
                         case 0100:									// Power fail system .. do nothing.
                             break;
                         default:
         //					printf("Uknown IOT:%o\n",xmd);
                             break;
                         }
                         return( acc );
                     }


             private:
                 void group3(unsigned short& acc,unsigned short& mmq,unsigned short xmd )
                     {
                         int qtm;

                         if ( xmd&0200 ) acc=acc&010000;
                         if ( xmd==07521 || xmd==07721 ) {
                             qtm=acc;
                             acc=mmq|( acc&010000 );
                             mmq=qtm&07777;
                             return;
                         }
                         if ( xmd&020 ) {mmq=acc&07777; acc&=010000; }
                         if ( xmd&0100 ) acc|=mmq;
                     }

             int group2(unsigned short acc,unsigned short xpc,unsigned short xmd )
                      {
                          int state;

                          state=0;
                          if ( acc&04000 ) state|=0100;
                          if ( ( acc&07777 )==0 ) state|=040;
                          if ( acc&010000 ) state|=020;
                          if ( ( xmd&0160 )==0 ) state=0;
                          if ( xmd&010 ) {
                              if ( ( ~state&xmd )==xmd ) return( xpc+1 );
                              return( xpc );
                          }
                          if ( state&xmd ) return( xpc+1 );
                          return( xpc );
                      }

              unsigned short group1(unsigned short acc,unsigned short md )
                      {
                          int tmp;

                          if ( md&0200 ) acc&=010000;
                          if ( md&0100 ) acc&=07777;
                          if ( md&040 ) acc^=07777;
                          if ( md&020 ) acc^=010000;
                          if ( md&1 ) acc++;
                          acc&=017777;
                          if ( md&016 )
                              switch( md&016 )
                          {
                              case 2:  tmp=(acc<<6)|((acc>>6)&077);		// BSW .. v untidy!
                                  tmp&=07777;
                                  if (acc&010000) tmp|=010000;
                                  acc=tmp;
                                  break;
                              case 6:  acc=acc<<1; if ( acc&020000 ) acc++;
                              case 4:  acc=acc<<1; if ( acc&020000 ) acc++;
                                  break;
                              case 10:  if ( acc&1 ) acc|=020000; acc=acc>>1;
                              case 8:  if ( acc&1 ) acc|=020000; acc=acc>>1;
                                  break;
                          }
                          return( acc&017777 );
                      }
              void dcm(unsigned short* mem,unsigned short &dac, unsigned short dmd, unsigned short &dpc)
                      {
                                  printf("SYS:%o %o %o\n",dmd,dac,dpc);
                          switch (dmd) {
                              case 06206:
                                  break;
                              case 06216:
                                  break;
                              case 06226:
                                  break;
                              case 06236:
                                  printf("PR3:%o\n",mem[dpc]);
                                  dpc+=1;
                                  break;
                              case 06246:
                                  dac&=010000;
                                  break;
                              case 06256:
                                  dac=(dac&010000)?014000:0;
                                 if (intinh&1) dac|=0200;
                                 if (ibus) dac|=01000;
                                 dac|=(ifl>>9)|(dfl>>12);
                                  break;
                              case 06266:
                                  break;
                      }
                      }
             };


}


#endif // PDP8I_H
