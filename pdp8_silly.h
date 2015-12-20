#ifndef PDP8_SILLY_H
#define PDP8_SILLY_H
#include "includes.h"
#include "pdp8events.h"
#include "pdp8state.h"
#include <QObject>
#include <array>

#include <QFile>
#include <QThread>
#include <QBuffer>
#include <QKeyEvent>
#include <QAtomicInteger>
#include <QMutex>

namespace PDP8 {

class PDP8_Hack : public CpuState {
     void dcm(int &dac, int dmd, int &dpc);
    int iot(int acc,int xmd,int &flg, int &vrix, int &vriy);
public:
    PDP8_Hack() {}
    void step();
};

class PDP8_Silly : public QObject {
    Q_OBJECT

    static const size_t MEMSZ =32768;
    static const size_t SECSIZE =512;
    int cinf,coutf,pinf,poutf,rinbf,ttyinten1,doutf,dout,ttyinten2;
    int ibus, intinh, intf, ilag;
    std::array<int,16> ibf;
    std::array<int,16> pbf;
    std::array<int,16> abf;
    std::array<unsigned char,4096> ring;
    int rngi,rngo;
    int pbp, dskrg,dskmem,dskad,dskfl,tm,hl,dskema, rfdn;
    int ifr,ifl,dfr,dfl,svr,uflag,usint,phcell;
    int rkwc,rkdn,rkca,rkcmd;
    int rxrg,rxad,rxtr,rxdn,rxctr,rxp;
    std::array<int,SECSIZE/2> rxbf; // [SECSIZE/2];
    int rkda;
    int vcix,vciy,vcflg;
    int clken,clkfl,clkcnt;
    int txinf,txoutf,txmode,txblk,txcnt;
    QBuffer  *rf08,*df32,*rk05,*ptr,*ptp,*rx50a,*rx50b,*rx50,*tx;
    QString ptp_filename; // out file name

    void group3(int &acc,int &mmq,int xmd );
    int group2(int acc,int xpc,int xmd );
     int group1(int acc,int md );
     void dcm(int &dac, int dmd, int &dpc);
     QBuffer * openFile(const char* name) ;

     struct DisplayRegesters{

         int opcode;
         int status;
         int lac;
         int mb;
         int mq;
         int mbac;
     };
protected:
     void run();
signals:
     void consoleOut(int c);
     void updateRegesters(int LAC, int PC, int MQ);
     void updateDisplay(const DisplayRegesters& reg);
public slots:
     void keyPressEvent(QKeyEvent* a);
     void keyPress(int a);
      void setRunningState(RunningState state);
public:
      std::array<unsigned short,MEMSZ> mem;
     int Pmask,dinf,Plen,dtyp,ExtTrm;

      //  int dsreg[10];
     DisplayRegesters dsreg;
     int regsel;
     int xmreg;
     double bright[255]; // code says 27, going to make it much  more:P
public:
     struct Regs {
         int LAC;
         int PC;
         int SW;
         int MQ;
     };

     QMutex mMutex;

     PDP8::EventSystem sim;
     // QT dosn't support scopped enums for AtomicInteger
     int _runningState;
     RunningState getRunningState() const { return (RunningState)_runningState; }
     int LAC;
     int MQ;
     int PC;
     int SW;
     void setSW(int sw){
        SW = sw&07777;
     }
     void loadADD(){
         PC = SW;
     }
     Regs getRegs() {
         Regs r;
       //  mMutex.lock();
         r = { LAC, PC, SW, MQ };
       //  mMutex.unlock();
         return r;
     }
     void setRegs( Regs r) {
         mMutex.lock();
         LAC = r.LAC;
         MQ = r.MQ;
         PC = r.PC;
         SW = r.SW;
         mMutex.unlock();
     }
     void setPC(int pc) {
         PC = pc & 07777;
         emit updateRegesters(LAC,PC,MQ);
     }
     void setMQ(int mq) {
         MQ = mq & 07777;
         emit updateRegesters(LAC,PC,MQ);
     }
     void setLAC(int lac) {
         LAC = lac & 017777;
         emit updateRegesters(LAC,PC,MQ);
     }
    explicit PDP8_Silly(QObject* p=nullptr);
    std::string printfc(const char* c, ...);
    void init();
    void XmInit(int freg);
    void XtInit();
    int GetRing();
    void OpenDevices(const char* Df32, const char* Rk05, const char* Ptr, const char* Ptp,int msk);
    void CloseDevices(int msk);
    void LoadBoot(bool  Flg);
    int Prun(int& xpc, int& xac, int& xmq,  int delay, int swreg,int &vrix, int &vriy);
    void step(int steps);
    int iot(int acc,int xmd,int &flg, int &vrix, int &vriy);
};


}



#endif // PDP8_SILLY_H
