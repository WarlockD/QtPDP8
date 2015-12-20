#ifndef PDP8_RK05_H
#define PDP8_RK05_H

#include "includes.h"
#include "pdp8i.h"

#include "pdp8events.h"
// VT52 simple emulation for terminal
namespace PDP8 {
    // Disk pack
    class RK01 {
        static const uint32 MAX_WORDS = 831488;
        // transfer 16.7 us a word
        // 240ms access time with extream track position, worst acess timing is 443ms
        // ok, 200 tracks (plus 3 spares), 4096 words per track, both sides, 16 sectors, 8 per side, 256 per sector
        // arm stepper modem works with a mim positiong time of 2ms
        // transfer is at 720k a second*2 clock data
        uint16 buffer[4][203][16][256];

    };

    class RK8 : public Device {


    };


}

#endif // PDP8_RK05_H
