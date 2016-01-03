#include "pdp8_silly.h"
#include <QDataStream>
#include "pdp8dissam.h"

namespace PDP8 {

static void shared_group3(uint16_t &acc,uint16_t &mmq,uint16_t xmd ){
    if ( xmd&0200 ) acc=acc&010000;
    if ( xmd==07521 || xmd==07721 ) {
        int qtm=acc;
        acc=mmq|( acc&010000 );
        mmq=qtm&07777;
        return;
    }
    if ( xmd&020 ) {mmq=acc&07777; acc&=010000; }
    if ( xmd&0100 ) acc|=mmq;
}

static bool shared_group2(uint16_t acc,uint16_t xmd ){
     uint8_t state=0;
     if ( acc&04000 ) state|=0100;
     if ( ( acc&07777 )==0 ) state|=040;
     if ( acc&010000 ) state|=020;
     if ( ( xmd&0160 )==0 ) state=0;
     if ( xmd&010 ) {
         if ( ( ~state&xmd )==xmd ) return true;
         return false;
     }
     if ( state&xmd ) return true;
     return false;
}

static uint16_t shared_group1(uint16_t acc,uint16_t md ){
    if ( md&0200 ) acc&=010000;
    if ( md&0100 ) acc&=07777;
    if ( md&040 ) acc^=07777;
    if ( md&020 ) acc^=010000;
    if ( md&1 ) acc++;
    acc&=017777;
    if ( md&016 )
        switch( md&016 )
    {
        case 2:
        {
            int tmp=(acc<<6)|((acc>>6)&077);		// BSW .. v untidy!
            tmp&=07777;
            if (acc&010000) tmp|=010000;
            acc=tmp;
        }
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
/*
static bool silly_iot(uint16_t xmd, CpuState& state){
    Regesters& r = state.regs();
    switch(xmd&0770) {
    case 0:
        switch(xmd&0777)
        {
        case 000: if(state._)
            flg=intf;intf=intinh=0;
            break;
        case 001:	intinh|=1;
            break;
        case 002:	intf=intinh=0;
            break;
        case 003:	if (ibus) flg=1;
            break;
        case 004:
            acc=(acc&010000)?014000:0;
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
            init();
            break;
        }
        break;
    }

    int i,j;
                unsigned short *p;
                unsigned short* xm = &mem[0]; // Stops CLR from unexpectedly moving the arrays
                unsigned short rx5[SECSIZE / 2];
                char iotbf[20];

                sprintf(iotbf,"%04o",xmd);
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
                                  if (!ptr->atEnd()) {
                                      QDataStream s(ptr);
                                      uint8_t c;
                                      s >> c;
                                      rinbf=(Pmask&0200)?c:c|0200;	// For 7 bit mode set parity bit
                                      pinf=1;
                                  }
                                  printf(".");
                                  return(acc);
                    case 021: flg=poutf;break;
                    case 022: poutf=0;break;
                    case 026:
                    case 024: if (ptp)
                              {
                                   QDataStream s(ptp);
                                  s << (uint8_t)(acc&0377&Pmask);
                                  poutf=1;
                                  Plen++;
                              }
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
                        emit consoleOut(acc&0177);
                        coutf=1;
                        break;
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
                        init();
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
                            dskmem=((mem[07751]+1)&07777)|i;  // mem
                            tm=(dskrg&03700)<<6;
                            dskad=(acc&07777)+tm;           ///dsk
                            i=010000-mem[07750];
                            p=&xm[dskmem];
                            df32->seek(dskad*2);
                           // fseek(df32,dskad*2,SEEK_SET);
//							printf("DF32:%o>%o,",dskad,dskmem);
                            if (xmd&2)              //read
                                df32->read((char*)p,i*2); // fread(p,1,i*2,df32);
                            else
                                df32->write((char*)p,i*2); // fwrite(p,1,i*2,df32);
                            dskfl=1;
                            mem[07751]=0;
                            mem[07750]=0;
                            acc=acc&010000;
                            break;
                        case 0611:      dskrg=0;
                            break;
                        case 0615:      dskrg=(acc&07777);              // register
                            break;
                        case 0616:      acc=(acc&010000)|dskrg;
                            break;
                        case 0626:      acc=(acc&010000)+(dskad&07777);
                            break;
                        case 0622:      if (dskfl) flg=1;
                            break;
                        case 0612:      acc=acc&010000;
                        case 0621:      flg=1; //No error
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
                            dskmem=((mem[07751]+1)&07777)|i;  //mem
                            dskad=(acc&07777)|(dskema<<12); // dsk
                            i=010000-mem[07750];
                            p=&xm[dskmem];

                            rf08->seek(dskad*2); // fseek(rf08,dskad*2,SEEK_SET);
                            printf("RF08:%d>%o,", dskad / 128, dskmem);
                            if (xmd&2)              //read
                                rf08->read((char*)p,i*2); //fread(p,1,i*2,rf08);
                            else
                                 rf08->write((char*)p,i*2); // fwrite(p,1,i*2,rf08);
                            rfdn=1;
                            mem[07751]=0;
                            mem[07750]=0;
                            acc=acc&010000;
                            break;
                        case 0611:      dskrg=0;
                            break;
                        case 0615:      dskrg=(acc&0770);              //register
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
                            flg=1; // skip always
                        case 0621:      // No error skip
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
                            rk05->seek(rkda*512); //fseek(rk05,rkda*512,SEEK_SET);
                            p=&xm[rkca];
                            rk05->read((char*)p,rkwc*2);//   fread(p,2,rkwc,rk05);
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
                            rk05->seek(rkda*512); // fseek(rk05,rkda*512,SEEK_SET);
                            p=&xm[rkca];
                             rk05->write((char*)p,rkwc*2);//fwrite(p,2,rkwc,rk05);
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
                                rx50->seek((rxad)*SECSIZE);  // fseek(rx50,(rxad)*SECSIZE,0);
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

                                if (rxrg==6)
                                    rx50->read((char*)rx5,SECSIZE);//  fread(rx5,1,SECSIZE,rx50);
                                else
                                    rx50->write((char*)rx5,SECSIZE);// fwrite(rx5,1,SECSIZE,rx50);

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
*/
void PDP8_Hack::step() {
    if(interruptPending()) {
        _int_ion = false;                                   /* interrupts off */
         _no_ion_pending = false;
         r.sf = (r.ifr >> 9) | (r.dfr >> 12);// | (r.uf << 6);
         r.ifr = r.ib = r.dfr = r.uf = r.ub = 0;
         m[0]=r.pc&07777;
         if (r.uf==3) r.sf|=0100; //if (uflag==3) svr|=0100;
         r.pc = 1;
    }

   // pbf[pbp]=pc+ifl;				// Looped buffer holding last 16 cycles for debug
   // ibf[pbp]=mem[pc+ifl];
   // abf[pbp]=ac;
   // pbp=( pbp+1 )&15;


    r.opcode = r.mb=m[r.pc+r.ifr ];					// FETCH
    r.ma=( ( r.mb&0177 )+( r.mb&0200?( r.pc&07600 ):0 ) )+r.ifr ;
    r.pc=( ( r.pc+1 )&07777 );

    if ((r.mb&07000)<06000){			// DEFER
        if ( r.mb&0400 ) {
            if ( ( r.mb&07770 )==010 ) m[r.ma]++;
            m[r.ma]&=07777;
            if (r.mb&04000)
                r.ma=m[r.ma]+r.ifr;
            else
                r.ma=m[r.ma]+r.dfr;
        }
    }
    switch ( r.mb&07000 )			// EXECUTE
    {
    case 0000:
        r.lac&=( m[r.ma]|010000 );
        break;
    case 01000:
        r.lac+=m[r.ma];
        break;
    case 02000:
            if ( ( m[r.ma]=( 1+m[r.ma] )&07777 )==0 ) setSkip();
        break;
    case 03000:
            m[r.ma]=r.lac&07777;
        r.lac&=010000;
        break;
    case 04000:
        r.ifr=r.ib<<9;
        r.ma=(r.ma&07777)+r.ifr;
            m[r.ma]=r.pc;
        r.pc=( r.ma+1 )&07777;
        _no_cif_pending=true;
        r.uf |=2;
        break;
    case 05000:
        r.pc= (r.ma&07777);
        r.ifr=r.ib<<9;
        _no_cif_pending=true;
        r.uf |=2;
        break;
    case 06000:
        if (r.svr==3) { setInterruptRequest(USER_DEV); break;}
        _skip = false;
       // if ((r.mb&0707)==0205||(r.mb&0707)==0206||(r.mb&0707)==0207)
          //  dcm( ac, md, pc);
      //  else
         //   ac=iot(  ac, md, flg, vrix, vriy );
       // if ( flg ) pc++;
        break;
    case 07000:
        break;
        /*
        if ( r.mb&0400 ) {
            if ( r.mb&1 ) {group3( ac, mq, md ); break; }
            pc=group2( ac, pc, md );
            if ( r.mb&0200 ) ac&=010000;
            if ( r.mb&4 )
                if (uflag==3) usint=1;
                else ac|=swreg;
                if ( md&2 ) {
                    if (uflag==3) {usint=1;break;}
                    for (i=0;i<16;i++) {
                        printf("%05o %04o %05o\n", pbf[pbp], ibf[pbp], abf[pbp]);
                        pbp=(pbp+1)&15;
                    }
                     return (1); }
                break;
        }
        ac=group1( ac, md );
        break;
    }
    if (intinh==1 && md!=06001) intf=1;

     if(_skip) { r.ma = (r.pc+ 1) & 07777; _skip = false; } else  r.ma = r.pc;
     */
    };
}

std::string PDP8_Silly::printfc(const char* c, ...) {
    char buffer[1024];
        va_list ap;
        va_start(ap, c);
        vsnprintf(buffer, sizeof(buffer), c, ap);
        va_end(ap);
        return std::string(buffer);
}
PDP8_Silly::PDP8_Silly(QObject *p) : QObject(p) {
init();
}
QBuffer * PDP8_Silly::openFile(const char* name) {
    QFile file(name);
    if(file.open(QFile::ReadOnly)) {
        QBuffer* buf = new QBuffer(this);
        buf->open(QBuffer::ReadWrite);

        buf->setData(file.readAll());
        buf->seek(0); // make sure we are at the start
        return buf;
    }
    return nullptr;
}
void PDP8_Silly::setRunningState(RunningState state){
    RunningState old_state = (RunningState)_runningState;
    switch(state) {
    case RunningState::SingleStepState:
    case RunningState::SingleStepInstruction:
    {
        if(old_state == RunningState::Run) break; // error here, we are in thread

        int delay = 0;
        int vrix=0;
        int vriy=0;
        std::stringstream s;
        // int PDP8_Silly::Prun(int& pc, int& ac, int& mq,  int& delay, int &swreg,int &vrix, int &vriy){
        s << "LAC=" << ostr(LAC,5) << " MQ=" << ostr(MQ,5) << " : " << PDP8::dsam8(mem[PC],PC,&mem[0],true);
        qDebug() << QString::fromStdString(s.str());
         Prun(PC, LAC, MQ,delay,SW,vrix, vriy);

    }
        _runningState = (qint32)RunningState::Stop;
        break;
    case RunningState::Stop:
        if(old_state != RunningState::Run) break;
         _runningState = (qint32)state;
        // Start it up by just going though
    case RunningState::Run:
        if(old_state == RunningState::Run) break;// start thread
         _runningState = (qint32)state;
        //this->start();
        break;
    default: // ugh
        state = RunningState::Stop;
         _runningState = (qint32)RunningState::Stop;
        break;
    }
}
 void PDP8_Silly::step(int steps) {
         for(int i=0;i<steps;i++) {

         if(RunningState::Run != (RunningState)_runningState) break;
         int vrix=0;
         int vriy=0;
         if(Prun(PC, LAC, MQ,97,SW,vrix, vriy)) {
             _runningState = (int)RunningState::Halt;
             break;
         }
     }

 }

void PDP8_Silly::run() {
    RunningState currentState = (RunningState)_runningState;
    int halt = 0;
    mMutex.lock();
    int pc = PC;
    int lac = LAC;
    int mq = MQ;
    int sw = SW;
    mMutex.unlock();
    int delay = 0;
    int vrix=0;
    int vriy=0;
    while(currentState == RunningState::Run){
        for(int i=0; i < 10000; i++) {
            if(halt=Prun(pc, lac, mq,delay,sw,vrix, vriy)) break;
        }
        if(halt) currentState = RunningState::Halt;
        else  currentState = (RunningState)_runningState;
        mMutex.lock();
        PC = pc;
        MQ = mq;
        LAC = lac;
        mMutex.unlock();

    }
    mMutex.lock();
    PC = pc;
    MQ = mq;
    LAC = lac;
    mMutex.unlock();
}

void PDP8_Silly::init(){
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
    PC=LAC=MQ=SW=0;
    //printf("Init? printf\n");
    //OldOut("Not allowed...RK8E\n");

    // wierd stuff
    regsel = 0;
}
void PDP8_Silly::XmInit(int freg)
{
    ifr=freg&070;
    ifl=ifr<<9;
    dfr=(freg&07)<<3;
    dfl=dfr<<9;
}

void PDP8_Silly::XtInit()
{
    svr=usint=uflag=0;
}

int PDP8_Silly::GetRing()
{
    int tm=ring[rngo];
    if (rngi==rngo) return -1;
    rngo=(rngo+1)&4095;
    return tm;
}
void PDP8_Silly::OpenDevices(const char* Df32, const char* Rk05, const char* Ptr, const char* Ptp,int msk) {
    if (Df32) df32= openFile(Df32);
    if (Rk05) rk05=openFile(Rk05);
    if (Ptr && (msk&1)) ptr=openFile(Ptr);
    if (Ptp &&(msk&2)) {ptp=new QBuffer(this);ptp_filename = QString::fromLocal8Bit(Ptp); Plen=0;}
    if (!df32 && !rf08) df32 = openFile(":/DF32.DSK");
    if (!rk05) df32 = openFile(":/diagpack2.rk05");
//	rf08=df32;
//	rx50=rx50a=fopen("os278wc.bin","r+b");
//	rx50b=fopen("dm101b1.bin","r+b");
    //tx=fopen("tx.dsk","r+b");
}
void PDP8_Silly::CloseDevices(int msk)
           {
    if (df32) { delete df32; df32 = nullptr; }
    if (rk05) { delete rk05; rk05 = nullptr; }
    if (ptr&&(msk&2)) { delete ptr; ptr = nullptr; }
    if (ptp&&(msk&2)) {
        QFile r(ptp_filename);
        if(r.open(QFile::WriteOnly)) {
            r.write(ptp->buffer());
            r.close();
        }
        delete ptp; ptp = nullptr;
    }
    if (rx50a) { delete rx50a; rx50a = nullptr; }
    if (rx50b) { delete rx50b; rx50b = nullptr; }
    if (tx) { delete tx; tx = nullptr; }
}
void PDP8_Silly::LoadBoot(bool  Flg)
            {
                unsigned int i;
                /* 4kMon DF32 boot */
                static const int ldqr[]={06603,06622,05201,05604,07600};
                /* RX50 BOOTSTRAP */
                static const int ldqx[]={01061,01046,060,03060,07327,01061,06751,07301,04053,04053,07004,06755,05054,06754,07450,
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

void PDP8_Silly::group3(int &acc,int &mmq,int xmd ){
    if ( xmd&0200 ) acc=acc&010000;
    if ( xmd==07521 || xmd==07721 ) {
        int qtm=acc;
        acc=mmq|( acc&010000 );
        mmq=qtm&07777;
        return;
    }
    if ( xmd&020 ) {mmq=acc&07777; acc&=010000; }
    if ( xmd&0100 ) acc|=mmq;
}

int PDP8_Silly::group2(int acc,int xpc,int xmd ){
    int state=0;
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
 int PDP8_Silly::group1(int acc,int md ){
      if ( md&0200 ) acc&=010000;
      if ( md&0100 ) acc&=07777;
      if ( md&040 ) acc^=07777;
      if ( md&020 ) acc^=010000;
      if ( md&1 ) acc++;
      acc&=017777;
      if ( md&016 )
          switch( md&016 )
      {
          case 2:
          {
              int tmp=(acc<<6)|((acc>>6)&077);		// BSW .. v untidy!
              tmp&=07777;
              if (acc&010000) tmp|=010000;
              acc=tmp;
          }
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

 void PDP8_Silly::dcm(int &dac, int dmd, int &dpc){
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



}
 void PDP8_Silly::keyPressEvent(QKeyEvent* a) {
     if(a->count() >0) {
         int ch = a->text()[0].toLatin1();
         keyPress(ch);
     }
 }

 void PDP8_Silly::keyPress(int a) {
     cinf=toupper(a)|0200;
     if ( cinf==0200+8 ) cinf=0377;
     if ( cinf==0200+24 ) cinf=131; // Remap ^X->^C
 }

 int PDP8_Silly::Prun(int& pc, int& ac, int& mq,  int delay, int swreg,int &vrix, int &vriy){
     int  ma, md, i;
                     int flg, reg;//, msk;

                     //unsigned short* pmem = &mem[0]; // Stops CLR from unexpectedly moving the arrays
                 //    double filt=0.005;
//double* p = &bright[0];  // Pins whole array
                     ifl=xmreg;
                     if ( ++ilag>=delay ) {
                         phcell=~phcell;
                     }
/*  Something is seriously wrong with all this pointer math
 * // OH I get it now., he is adjusting the brightness of the lights in the regester
 depending on what regester is selected and what the pc is looking at
 // humm...kind of enginous honstly, but useless for what I need
                     if ( ++ilag>=delay ) {
                         phcell=~phcell;
                         reg=dsreg[regsel];
                         msk=040000;
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
                         ilag=0;
                     }
*/
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

                     if ((md&07000)<06000)			// DEFER
                         if ( md&0400 ) {
                             if ( ( ma&07770 )==010 ) mem[ma]++;
                             mem[ma]&=07777;
                             if (md&04000)
                                 ma=mem[ma]+ifl;
                             else
                                 ma=mem[ma]+dfl;
                         }
                         //   printf( "%04o %04o %05o : %05o %04o %04o\n", pc,md,ac,ma,ifl,dfl);

                         dsreg.opcode=04000 | ((md&07000)>>3);	// Build display reg (State)

                         switch ( md&07000 )			// EXECUTE
                         {
                         case 0000:
                             ac&=( mem[ma]|010000 );
                             break;
                         case 01000:
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
                             flg=0;
                             if ((md&0707)==0205||(md&0707)==0206||(md&0707)==0207)
                                 dcm( ac, md, pc);
                             else
                                 ac=iot(  ac, md, flg, vrix, vriy );
                             if ( flg ) pc++;
                             break;
                         case 07000:
                             if ( md&0400 ) {
                                 if ( md&1 ) {group3( ac, mq, md ); break; }
                                 pc=group2( ac, pc, md );
                                 if ( md&0200 ) ac&=010000;
                                 if ( md&4 )
                                     if (uflag==3) usint=1;
                                     else ac|=swreg;
                                     if ( md&2 ) {
                                         if (uflag==3) {usint=1;break;}
                                         for (i=0;i<16;i++) {
                                             printf("%05o %04o %05o\n", pbf[pbp], ibf[pbp], abf[pbp]);
                                             pbp=(pbp+1)&15;
                                         }
                                          return (1); }
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
                         dsreg.status=reg;
                         dsreg.lac=ac;
                         dsreg.mb=mem[pc+ifl];
                         dsreg.mq=mq;
                         dsreg.mbac=ac|mem[pc+ifl];
                            xmreg=ifl;
                         return(0);
 }

 int PDP8_Silly::iot(int acc,int xmd,int &flg, int &vrix, int &vriy){
     int i,j;
                 unsigned short *p;
                 unsigned short* xm = &mem[0]; // Stops CLR from unexpectedly moving the arrays
                 unsigned short rx5[SECSIZE / 2];
                 char iotbf[20];

                 sprintf(iotbf,"%04o",xmd);
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
                                   if (!ptr->atEnd()) {
                                       QDataStream s(ptr);
                                       uint8_t c;
                                       s >> c;
                                       rinbf=(Pmask&0200)?c:c|0200;	// For 7 bit mode set parity bit
                                       pinf=1;
                                   }
                                   printf(".");
                                   return(acc);
                     case 021: flg=poutf;break;
                     case 022: poutf=0;break;
                     case 026:
                     case 024: if (ptp)
                               {
                                    QDataStream s(ptp);
                                   s << (uint8_t)(acc&0377&Pmask);
                                   poutf=1;
                                   Plen++;
                               }
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
                         emit consoleOut(acc&0177);
                         coutf=1;
                         break;
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
                         init();
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
                             df32->seek(dskad*2);
                            // fseek(df32,dskad*2,SEEK_SET);
 //							printf("DF32:%o>%o,",dskad,dskmem);
                             if (xmd&2)              /*read */
                                 df32->read((char*)p,i*2); // fread(p,1,i*2,df32);
                             else
                                 df32->write((char*)p,i*2); // fwrite(p,1,i*2,df32);
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

                             rf08->seek(dskad*2); // fseek(rf08,dskad*2,SEEK_SET);
                             printf("RF08:%d>%o,", dskad / 128, dskmem);
                             if (xmd&2)              /*read */
                                 rf08->read((char*)p,i*2); //fread(p,1,i*2,rf08);
                             else
                                  rf08->write((char*)p,i*2); // fwrite(p,1,i*2,rf08);
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
                             rk05->seek(rkda*512); //fseek(rk05,rkda*512,SEEK_SET);
                             p=&xm[rkca];
                             rk05->read((char*)p,rkwc*2);//   fread(p,2,rkwc,rk05);
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
                             rk05->seek(rkda*512); // fseek(rk05,rkda*512,SEEK_SET);
                             p=&xm[rkca];
                              rk05->write((char*)p,rkwc*2);//fwrite(p,2,rkwc,rk05);
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
                                 rx50->seek((rxad)*SECSIZE);  // fseek(rx50,(rxad)*SECSIZE,0);
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

                                 if (rxrg==6)
                                     rx50->read((char*)rx5,SECSIZE);//  fread(rx5,1,SECSIZE,rx50);
                                 else
                                     rx50->write((char*)rx5,SECSIZE);// fwrite(rx5,1,SECSIZE,rx50);

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
}

