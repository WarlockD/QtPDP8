#include "pdp8_utilities.h"
#include <iostream>
#include <stdexcept>
#define MEMSIZE 32768
#include <algorithm>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <streambuf>

template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class vectorwrapbuf : public std::basic_streambuf<CharT, TraitsT> {
public:
    vectorwrapbuf(std::vector<CharT> &vec) {
        setg(vec.data(), vec.data(), vec.data() + vec.size());
    }
};

#define	VC8I
//#define	KV8I

// Select PT08 if required; it will override some of the IOTs from the Data Communications System 680/I
//#define PT08

// Select one of the following:
#define	AF01A
//#define	AF04A

// Select one of the following:
#define	AA01A
//#define AA05

// If selecting DP01AA, don't bother with DF32/RF08
//#define	DP01AA
#ifndef	DP01AA
// Select one of the following:
#define	DF32
// #define RF08
#endif	// DP01AA

// Select one of the following:
#define	TA8A
//#define TC58
//#define TR02

#define	TAG_DATA		0x0001				// memory region is tagged as data,
#define	TAG_SUB			0x0002				// subroutine target, or,
#define	TAG_LABEL		0x0004				// label
#define	TAG_RETURN		0x0008				// return from subroutine
#define	TAG_TYPE_MASK	0x00ff				// mask of above types
#define	TAG_WRITABLE	0x0100				// set if anyone writes to this data location (else constant)
#define	TAG_KONSTANT	0x0200				// can be changed from Caaaa -> Kvvvv
#define	TAG_INDIRECTFC	0x0400				// target of an indirect flow control (JMS I / JMP I) (only meaningful if not writable)
struct segment_t
{
    uint16_t	saddr;						// starting address
    uint16_t	nwords;						// number of contiguous words
}	;
static const char *codes[] = {"AND", "TAD", "ISZ", "DCA", "JMS", "JMP"};	// IOT and OPR are decoded elsewhere
namespace PDP8 {
std::ostream& operator<< (std::ostream& s, const octzero& o) {
    s << std::setfill('0') << std::setw(o._digits) << std::oct << o._num;
    return s;
}
std::string to_octal(int num,const char* fmt) {
    char buff[8];
    sprintf(buff,fmt,num);
    return buff;
}
void LoadRim(std::istream& i ,CpuState& s, size_t origin){
    size_t hi=0, lo=0, wd=0;
    do {                                                    /* skip leader */
        if(i.eof()) throw PDP8_Exception("End of Leader before data");
        hi = i.get();
        } while ((hi == 0) || (hi >= 0200));
    do {

        /* data block */
        if(i.eof()) throw PDP8_Exception("Bad Paper tape format");
        lo = i.get();
        wd = (hi << 6) | lo;
        if (wd > 07777) origin = wd & 07777;
        else s[origin++ & 07777] = wd;
        if(i.eof()) throw PDP8_Exception("Bad Paper tape format");
        hi = i.get();
        } while (hi < 0200);
}
void LoadRim(const std::vector<char>& i ,CpuState& s, size_t origin){
    size_t pos=0;
    auto get = [&i,&pos]() { return (uint8_t)(pos < i.size() ? i[pos++] : -1); };
    auto eof = [&i,&pos]() { return pos >= i.size(); };
    size_t hi=0, lo=0, wd=0;
    do {                                                    /* skip leader */
        if(eof()) throw PDP8_Exception("End of Leader before data");
        hi = get();
        } while ((hi == 0) || (hi >= 0200));
    do {

        /* data block */
        if(eof()) throw PDP8_Exception("Bad Paper tape format");
        lo = get();
        wd = (hi << 6) | lo;
        if (wd > 07777) origin = wd & 07777;
        else s[origin++ & 07777] = wd;
        if(eof()) throw PDP8_Exception("Bad Paper tape format");
        hi = get();
        } while (hi < 0200);
}
void LoadRim(const std::vector<char>& i ,unsigned short* M, size_t origin){
    size_t pos=0;
    auto get = [&i,&pos]() { return (uint8_t)(pos < i.size() ? i[pos++] : -1); };
    auto eof = [&i,&pos]() { return pos >= i.size(); };
    int hi=0, lo=0, wd=0;
    // Find leader first
    // Some times these pwper dumps have text at the start to read
    do{
        hi = get();
    } while(hi < 0200 && hi != -1); // skip text or bad data
    if(hi == -1) throw PDP8_Exception("No heder found in file, must start with atleast one 0200 char");
    while(((hi=get())!=-1) &&  ((hi == 0) || (hi >= 0200)));   /* skip leader */
    if(hi == -1) throw PDP8_Exception("End of leader before data");
    do { // data block
        if(eof()) throw PDP8_Exception("Bad Paper tape format");
        lo = get();
        wd = (hi << 6) | lo;
        if (wd > 07777) origin = wd & 07777;
        else M[origin++ & 07777] = wd;
        if(eof()) throw PDP8_Exception("Bad Paper tape format");
        hi = get();
        } while (hi < 0200);
}

void LoadRim(const std::string& filename, CpuState& s, size_t origin ){
    /* RIM loader format consists of alternating pairs of addresses and 12-bit
       words.  It can only operate in field 0 and is not checksummed.
    */
    std::fstream fi;

    fi.open(filename,std::ios_base::binary | std::ios_base::in);
    if(!fi.good()) throw PDP8_Exception("Can't open \"" + filename + "\"");
   LoadRim(fi,s,origin);
}

void LoadBin(const std::vector<char>& data, CpuState& s, size_t origin ){
	(void)data, (void)s, (void)origin;
    /* RIM loader format consists of alternating pairs of addresses and 12-bit
       words.  It can only operate in field 0 and is not checksummed.
    */
    //vectorwrapbuf<char> databuf(data);
  //  std::istream is(&databuf);
  // LoadBin(is,s,origin);
}
void LoadBin(const std::string& filename, CpuState& s, size_t origin ){
    /* RIM loader format consists of alternating pairs of addresses and 12-bit
       words.  It can only operate in field 0 and is not checksummed.
    */
    std::fstream fi;

    fi.open(filename,std::ios_base::binary | std::ios_base::in);
    if(!fi.good()) throw PDP8_Exception("Can't open \"" + filename + "\"");
   LoadBin(fi,s,origin);
}
/* BIN loader format consists of a string of 12-bit words (made up from
   7-bit characters) between leader and trailer (200).  The last word on
   tape is the checksum.  A word with the "link" bit set is a new origin;
   a character > 0200 indicates a change of field.
*/

int binGet(std::istream& fi, unsigned char& newf)
{
    bool rubout = false;
    for(;;) {
        if(fi.eof()) return EOF;
        int c = fi.get();
        if(rubout) rubout = false;
        else if(c == 0377) rubout = true;
        else if(c > 0200) newf = (c & 070) << 9; /* change field */
        else return c;
    }
}



void LoadBin(std::istream& fi, CpuState& s, size_t origin ){

    int hi, lo, wd, csum, t;
    unsigned char field, newf;
    int sections_read = 0;

    for (;;) {
        csum = 0;
        origin = 0;
        field = newf = 0;                   /* init */
        do {                                                /* skip leader */
            if ((hi = binGet (fi, newf)) == EOF) {
                if (sections_read == 0) throw PDP8_Exception("Bad Paper tape format: No sections read");
                    return;
                }
            } while ((hi == 0) || (hi >= 0200));
        for (;;) {                                          /* data blocks */
            if ((lo = binGet (fi, newf)) == EOF) throw PDP8_Exception("Bad Paper tape format: Low Charater");  /* low char */
            wd = (hi << 6) | lo;                            /* form word */
            t = hi;                                         /* save for csum */
            if ((hi = binGet (fi, newf)) == EOF) throw PDP8_Exception("Bad Paper tape format: High Charater");    /* next char */
            if (hi == 0200) {                               /* end of tape? */
                if ((csum - wd) & 07777) {                  /* valid csum? */
                    if (sections_read == 0) throw PDP8_Exception("Bad Paper tape format: No sections read");
                    return ;
                    }
                throw PDP8_Exception("Bad Paper tape format: Bad Checksum");    /* next char */
                }
            csum = csum + t + lo;                           /* add to csum */
            if (wd > 07777)                                 /* chan 7 set? */
                origin = wd & 07777;                        /* new origin */
            else {                                          /* no, data */
                if ((field | origin) >= MEMSIZE) throw PDP8_Exception("Bad Paper tape format: Memory Overflow");
                s[field | origin] = wd;
                origin = (origin + 1) & 07777;
                }
            field = newf;                                   /* update field */
            }
        }
  //  return SCPE_IERR;
}


void LoadPaperTape(const std::string& filename, CpuState& s, size_t origin ){
    std::fstream fi;
    fi.open(filename,std::ios_base::binary | std::ios_base::in);
    if(!fi.good()) throw PDP8_Exception("Can't open \"" + filename + "\"");
    std::string lfilename ;
    std::transform(filename.begin(), filename.end(), lfilename.begin(), ::tolower);
    if(lfilename.find_last_of(".rim") != std::string::npos) LoadBin(fi,s,origin); else LoadRim(fi,s,origin);
 }


}
