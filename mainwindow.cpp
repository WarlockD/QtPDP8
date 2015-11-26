#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pdp8_utilities.h"
#include <QDebug>
#include <QQueue>




void MainWindow::on_halt(const QString dsam) {

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::on_timer);
       timer->start(1000/15); // update 30 times a sec
       connect(ui->simpleConsole,&QTTY2::keyboardSend,this,&MainWindow::onData);
       connect(ui->stupidTerm,&PlacableTextEdit::keyboardSend,this,&MainWindow::onData);
       connect(this,&MainWindow::on_halt_signal,ui->dsamEdit,&QPlainTextEdit::setPlainText);

    cpu.power();
    auto func([this](PDP8::Cpu& c){
        QString str = QString::fromStdString(c.printHistory(16));
        emit on_halt_signal(str);
    });

    cpu.setHaltCallback(func);
  //  pdp8Cpu.OpenDevices(nullptr,nullptr,nullptr,nullptr,0);
    //pdp8Cpu.LoadBoot((unsigned short*)&pdp8State.mem,false);

    PDP8::LoadRim("MAINDEC-8E-D0AB-InstTest-1.pt",cpu);
  //  pdp8Cpu.state = &pdp8State;
//    pdp8State.ma = 0200;
//    pdp8State.pc = 0200;
    //ui->switchRow->setSr(030);
     // hack for now

    std::shared_ptr<PDP8::KL8C> ptr = std::make_shared<PDP8::KL8C>(cpu);


    cpu.installDev(ptr,03,true);
    cpu.installDev(ptr,04,true);
    kl8c = ptr;

    ui->switchRow->setSr(07777);
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;
   //  pdp8Cpu.runIt(pdp8State);
     last_run_state= false;
     auto test = std::bind(&MainWindow::onDataTerm,this,std::placeholders::_1);
     kl8c->setCallBack(test);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onDataTerm(Terminal::SerialDCEInterface&i)
{
    uint8_t data;
    while(i.recive(data)) {
        ui->stupidTerm->putChar(data);
    }
}
void MainWindow::on_pushButton_clicked()
{
     cpu.panelSwitch(PDP8::PanelToggleSwitch::Start);
}

  void MainWindow::onData(int data) {

      if(kl8c) Q_ASSERT(kl8c->trasmit(data)); // for now this should always work


  }

void MainWindow::on_timer() {
    PDP8::Regesters& r = cpu.regs();
    cpu.setSR(ui->switchRow->getSr());
    ui->labelAC->setText(QString::number(r.lac,9));
    ui->labelPC->setText(QString::number(r.pc,8));
    ui->labelMQ->setText(QString::number(r.mq,8));
    ui->labelMB->setText(QString::number(r.mb,8));
    ui->labelMA->setText(QString::number(r.ma,8));

    switch(cpu.state()) {
    case PDP8::State::Fetch: ui->labelState->setText(QStringLiteral("Fetch")); break;
    case PDP8::State::Defer: ui->labelState->setText(QStringLiteral("Defer")); break;
    case PDP8::State::Execute: ui->labelState->setText(QStringLiteral("Exec")); break;
    default:
        ui->labelState->setText(QStringLiteral("Err")); break;
    }
/*
    if(sinterface && sinterface->checkBuffer() ){
        QChar c = sinterface->getBufferuffer();
        ui->simpleConsole->putData(c);

        switch(c) {
        case '\a': ui->simpleConsole->putData(QString("BELL(\"%1\")").arg(c).toLatin1());
            break;
        case '\n': case '\r':
        default:
            const QChar ch = QChar::fromLatin1(c);
            if(ch.isPrint()) ui->simpleConsole->putData(QChar::fromLatin1(c));
            else ui->simpleConsole->putData(QString("Printable(\"%1\")").arg(c).toLatin1());
             }



}

*/

    std::string s = cpu.printState();
    s += kl8c->debug();
    ui->debugConsole->clear();
    ui->debugConsole->putData(QString::fromStdString(s));
    if(!last_run_state && !cpu.run()) {
        ui->debugConsole->putData(QString::fromStdString(cpu.printHistory(16)));
    }
    if(last_run_state && !cpu.run()) {
       // std::string sstring = pdp8Cpu.hst.disam_text(cpu.g.mem);
       // QString str = QString::fromStdString(sstring);
       // qDebug() << str;
        last_run_state = false;


    }
    if(cpu.run() && !last_run_state) last_run_state = true;
   // ui->labelAC->setText(QString::number(pdp8State.ac,8));

}

void MainWindow::on_pushButton_2_clicked()
{
    cpu.panelSwitch(PDP8::PanelToggleSwitch::LoadAdd);
}

void MainWindow::on_pushButton_7_clicked()
{
    cpu.panelSwitch(PDP8::PanelToggleSwitch::Stop);
}

void MainWindow::on_pushButton_5_clicked()
{
      cpu.panelSwitch(PDP8::PanelToggleSwitch::Cont);
}

void MainWindow::on_pushButton_6_clicked()
{
       cpu.panelSwitch(PDP8::PanelToggleSwitch::SingleStepInstruction);
}

void MainWindow::on_pushButton_8_clicked()
{
    cpu.power();

    PDP8::LoadRim("MAINDEC-8E-D0AB-InstTest-1.pt",cpu);
    ui->switchRow->setSr(07777);
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;
     last_run_state= false;
}

void MainWindow::on_pushButton_9_clicked()
{
    cpu.power();
//cpu.panelSwitch(PDP8::PanelToggleSwitch::Power);
    PDP8::LoadRim("MAINDEC-8E-D0BB-InstTest-2.pt",cpu);
    ui->switchRow->setSr(07777);
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;
     last_run_state= false;
}

void MainWindow::on_pushButton_11_clicked()
{
    cpu.panelSwitch(PDP8::PanelToggleSwitch::Clear);
}

void MainWindow::on_pushButton_4_clicked()
{
    cpu.panelSwitch(PDP8::PanelToggleSwitch::Exam);
}

void MainWindow::on_pushButton_10_clicked()
{
    cpu.power();
//cpu.panelSwitch(PDP8::PanelToggleSwitch::Power);
    try{
        PDP8::LoadBin("focal69.bin",cpu);
    }catch(PDP8::PDP8_Exception& e) {
 (void)e;
    }

    ui->switchRow->setSr(07777);
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;
     last_run_state= false;
}

void MainWindow::on_pushButton_12_clicked()
{
    cpu.power();
   // try{
    //      PDP8::LoadBin("MAINDEC-08-DHKLD-TTYTest.pt",cpu);
       // PDP8::LoadBin("focal69.bin",cpu);
   // }catch(PDP8::PDP8_Exception& e) {

   // }
    PDP8::LoadRim("MAINDEC-08-DHKLD-TTYTest.pt",cpu);
    ui->switchRow->setSr(0);
    cpu[0020] = 0;  // ASR 33
    qDebug() << "Old Code :" << QString::number(cpu[0021],8);
   // cpu[0021] = 03040;  // devices
    cpu[0022] = 0110;  // devices
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;

    last_run_state= false;


}

void MainWindow::on_pushButton_13_clicked()
{
    cpu.power();
    try{
     //   PDP8::LoadBin("maindec-08-dgv5a-b-pb.bin",cpu);
        PDP8::LoadBin("dhvtc-d-pb",cpu);
    }catch(PDP8::PDP8_Exception& e) {
        (void)e;
    }

    ui->switchRow->setSr(0);
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;
    last_run_state= false;
}

void MainWindow::on_pushButton_14_clicked()
{
    cpu.power();
    try{
       PDP8::LoadBin("focal69.bin",cpu);
      //  PDP8::LoadBin("dhvtc-d-pb",cpu);
    }catch(PDP8::PDP8_Exception& e) {
 (void)e;
    }

    ui->switchRow->setSr(0);
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;
    last_run_state= false;
}
