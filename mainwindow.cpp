#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pdp8_utilities.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::on_timer);
       timer->start(1000/15); // update 30 times a sec
       connect(ui->simpleConsole,&Console::getData,this,&MainWindow::onData);
    cpu.power();
  //  pdp8Cpu.OpenDevices(nullptr,nullptr,nullptr,nullptr,0);
    //pdp8Cpu.LoadBoot((unsigned short*)&pdp8State.mem,false);

    PDP8::LoadRim("d:\\PDP8\\MAINDEC-8E-D0AB-InstTest-1.pt",cpu);
  //  pdp8Cpu.state = &pdp8State;
//    pdp8State.ma = 0200;
//    pdp8State.pc = 0200;
    //ui->switchRow->setSr(030);
      tty=  &PDP8::Cpu::InstallSimpleTTY(cpu);
    ui->switchRow->setSr(07777);
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;
   //  pdp8Cpu.runIt(pdp8State);
     last_run_state= false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(!cpu.run()) {
        cpu.singleStep(); //.start();
    }
}

  void MainWindow::onData(const QChar data) {
      cpu.terminalIn(data.toLatin1());
  }

void MainWindow::on_timer() {
    PDP8::Regesters& r = cpu.regs();
    cpu.setSR(ui->switchRow->getSr());
    ui->labelAC->setText(QString::number(r.lac,9));
    ui->labelPC->setText(QString::number(r.pc,8));
    ui->labelMQ->setText(QString::number(r.mq,8));
    ui->labelMB->setText(QString::number(r.mb,8));
    ui->labelMA->setText(QString::number(r.ma,8));

    if(tty && tty->haveData() ){
        char c = tty->received();
        const QChar ch = QChar::fromLatin1(c);
        if(ch.isPrint() || ch == '\r' || ch == '\n') ui->simpleConsole->putData(QChar::fromLatin1(c));
        else {
            QString s = QString("Printable(\"%1\")").arg(c);

            ui->simpleConsole->putData(s.toLatin1());
        }

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
   cpu.regs().pc =  ui->switchRow->getSr();
               ui->labelPC->setText(QString::number(cpu.regs().pc,8));
}

void MainWindow::on_pushButton_7_clicked()
{
    // stop
    cpu.stop();
}

void MainWindow::on_pushButton_5_clicked()
{
      cpu.cont();
}

void MainWindow::on_pushButton_6_clicked()
{
     cpu.singleStep();
}
