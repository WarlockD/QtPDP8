#include "pdp8_utilities.h"
#include <iostream>
#include <stdexcept>
#define MEMSIZE 32768
#include <algorithm>
#include <string>
namespace PDP8 {

void LoadRim(std::istream& i ,PDP8_State& s, size_t origin){
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
        else s.mem[origin++ & 07777] = wd;
        if(i.eof()) throw PDP8_Exception("Bad Paper tape format");
        hi = i.get();
        } while (hi < 0200);
}

void LoadRim(const std::string& filename, PDP8_State& s, size_t origin ){
    /* RIM loader format consists of alternating pairs of addresses and 12-bit
       words.  It can only operate in field 0 and is not checksummed.
    */
    std::fstream fi;

    fi.open(filename,std::ios_base::binary | std::ios_base::in);
    if(!fi.good()) throw PDP8_Exception("Can't open \"" + filename + "\"");
   LoadRim(fi,s,origin);
}
void LoadBin(const std::string& filename, PDP8_State& s, size_t origin ){
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



void LoadBin(std::istream& fi, PDP8_State& s, size_t origin ){

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
                s.mem[field | origin] = wd;
                origin = (origin + 1) & 07777;
                }
            field = newf;                                   /* update field */
            }
        }
  //  return SCPE_IERR;
}


void LoadPaperTape(const std::string& filename, PDP8_State& s, size_t origin ){
    std::fstream fi;
    fi.open(filename,std::ios_base::binary | std::ios_base::in);
    if(!fi.good()) throw PDP8_Exception("Can't open \"" + filename + "\"");
    std::string lfilename ;
    std::transform(filename.begin(), filename.end(), lfilename.begin(), ::tolower);
    if(lfilename.find_last_of(".rim") != std::string::npos) LoadBin(fi,s,origin); else LoadRim(fi,s,origin);
 }
}
