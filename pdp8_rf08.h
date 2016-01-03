#ifndef PDP8_RF08_H
#define PDP8_RF08_H


#include "includes.h"
#include "pdp8i.h"

#include "pdp8events.h"
// VT52 simple emulation for terminal
namespace PDP8 {
// Ah ok, the RF08 is the central processor and the
// rs08 are the disk packs

    // Disk pack
    class RS08 { // disks hardware
        static const uint32 MAX_WORDS = 262144;
        static const uint16 WORD_COUNT_ADDRESS = 07750;
        static const uint16 CURRENT_ADDRESS = 07751;
        // 16.0us per word, min acces 258us 16.9ms average acces, 33.6ms max access, program interrupt 33 ms clock
        // transfer 16.7 us a word
        // 2048 words on each 128 track
        uint16 buffer[128][2048];
    public:
        uint16& operator[](uint16 addr) { (void)addr;
            // bits 7 and 8 are drive address

        }

    };

    class RF08 : public Device {
        static const byte _rf08Dev = 06;
static const uint16 RFS_PCA	=	04000;				/* photocell status */
static const uint16 RFS_DRE	=	02000;				/* data req enable */
static const uint16 RFS_WLS	=	01000;				/* write lock status */
static const uint16 RFS_EIE	=	00400;				/* error int enable */
static const uint16 RFS_PIE	=	00200;				/* photocell int enb */
static const uint16 RFS_CIE	=	00100;				/* done int enable */
static const uint16 RFS_MEX	=	00070;				/* memory extension */
static const uint16 RFS_DRL	=	00004;				/* data late error */
static const uint16 RFS_NXD	=	00002;				/* non-existent disk */
static const uint16 RFS_PER	=	00001;				/* parity error */
static const uint16 RFS_ERR	=	(RFS_WLS + RFS_DRL + RFS_NXD + RFS_PER);
static const uint16 RF_WMASK =03777;

static const uint16 RF_WC		=07750;				/* word count */
static const uint16 RF_MA		=07751;				/* mem address */

static const uint16 RF_READ		=2;
static const uint16 RF_WRITE	=4;

static const uint16 INT_RF	=	1  ;
        RS08 _disk[4];
        byte phcell;
        bool _interrupt;
        bool _intEnabled;
        uint16 dskrg;
        uint16 dskmem;
        uint16 dskad;
        uint16 rfdn;
        uint16_t dskema;
        std::vector<uint16> _data;
        EventFunction _rf08Event;
        EventID _rf08EventID;
    public:
        RF08(CpuState& c) : Device(c) ,phcell(0) {
            _rf08Event = [this]() {
                if(phcell==0) phcell = 1; else phcell = 0;
                return (time_slice)std::chrono::milliseconds(30);
            };
           // time_slice convert = std::chrono::duration_cast<time_slice>(std::chrono::milliseconds(30));

           // _rf08Event = c.sim.activate(_rf08Event,convert);
            const int ldqr[]={06603,06622,05201,05604,07600};
           // for (tm=0;tm<MEMSZ;tm++) mem[tm]=07402;
            for ( int i=0; i<sizeof( ldqr )/sizeof(int); i++ ) {
                c[i+0200]=ldqr[i];
            }
        }
        void loadData(const std::vector<uint16>& data) { _data = data; }

        void clear() override {
            dskrg = dskmem = dskad = rfdn = dskema = 0;
          //  mem[07750]=07576;
           // mem[07751]=07576;

            //	RK05 boot for OS/8
          //  mem[030]=06743;
          //  mem[031]=05031;



          //  ttoEvent.cancelEvent();
          //  ttiEvent.cancelEvent();
            _interrupt = _intEnabled = false;
          //  _intEnabled = true;
        }
        void updateInterrupts() override {
           // dtyp&&rfdn&&(dskrg&0100)
           // c.interruptRequest(_rf08Dev,_intEnabled && _interrupt);
           c.interruptRequest(_rf08Dev,rfdn&&(dskrg&0100));
        }
        // bare bones implmentation, such a hack
        void iot() override{
            Regesters& r = c.regs();
            uint16 i = 0;
            uint16* p = nullptr;
            uint16* f = nullptr;
            uint32 seek_address;
            switch( r.mb&0777 ) {
            case 0601:      dskad=rfdn=0; break;
            case 0605:
            case 0603:
                i=(dskrg&070)<<9;
                dskmem=((c[07751]+1)&07777)|i;  /* mem */
                dskad=(r.lac&07777)|(dskema<<12); /* dsk */
                i=010000-c[07750];
                seek_address = dskad; // *2 fseek(rf08,dskad*2,SEEK_SET);
                p=&c[dskmem];
                f=&_data[seek_address];
                LOGF() << "RF08:" << (dskad / 128) << ">" << std::oct << dskmem;
                //printf("RF08:%d>%o,", dskad / 128, dskmem);
                if (r.mb&2)      {        /*read */
                    for(int a=0;a < i; a++) { *p = *f; p++; f++; }
                   // fread(p,1,i*2,rf08);

                }else{
                    for(int a=0;a < i; a++) {*f= *p; p++; f++; }
                   // fwrite(p,1,i*2,rf08);
                }
                rfdn=1;
                c[07751]=0;
                c[07750]=0;
                r.lac&=010000;
                break;
            case 0611:      dskrg=0;
                break;
            case 0615:      dskrg=(r.lac&0770);              /* register */
                r.lac&=010000;
                break;
            case 0616:      r.lac=(r.lac&010000)|dskrg;
                r.lac|=04000&phcell;				// add photocell sync bit
                break;
            case 0626:      r.lac=(r.lac&010000)+(dskad&07777);
                break;
            case 0623:
            case 0622:      if (rfdn) c.setSkip();
                break;
            case 0612:      r.lac&=010000;
                c.setSkip(); //=1; /* skip always */
            case 0621:      /* No error skip*/
                break;
            case 0641:		dskema=0;
                break;
            case 0643:		dskema=r.lac&0377;
                r.lac&=010000;
                break;
            case 0645:		r.lac=(r.lac&010000)|dskema;
                break;
          }
        }
        std::string debug() const  override {
            std::stringstream s;
            s << " _intEnabled=" << (_intEnabled ? "true" : "false");
            s << std::endl;
            return s.str();
        }
    };


}

#endif // PDP8_RF08_H
