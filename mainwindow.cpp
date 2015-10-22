#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pdp8_utilities.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::on_timer);
       timer->start(1000/15); // update 30 times a sec
       connect(ui->simpleConsole,&Console::getData,this,&MainWindow::onData);
    pdp8Cpu.CPU_Init();
  //  pdp8Cpu.OpenDevices(nullptr,nullptr,nullptr,nullptr,0);
    //pdp8Cpu.LoadBoot((unsigned short*)&pdp8State.mem,false);
    PDP8::LoadRim("f:\\PDP8\\MAINDEC-8E-D0AB-InstTest-1.pt",pdp8State);
    pdp8Cpu.state = &pdp8State;
    pdp8State.ma = 0200;
    pdp8State.pc = 0200;
    //ui->switchRow->setSr(030);
    ui->switchRow->setSr(07777);
     pdp8Cpu.runIt(pdp8State);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(!pdp8State.run) {
        pdp8State.start();

         pdp8State.singleStep= false;
         pdp8State.run = true;
    }
}

  void MainWindow::onData(const QChar data) {
      pdp8Cpu.terminal_in.emplace(data.toLatin1());
  }

void MainWindow::on_timer() {
    pdp8State.sw = ui->switchRow->getSr();
    ui->labelAC->setText(QString::number(pdp8State.ac,8));
    ui->labelPC->setText(QString::number(pdp8State.pc,8));
    ui->labelMQ->setText(QString::number(pdp8State.mq,8));
    ui->labelMB->setText(QString::number(pdp8State.mb,8));
    ui->labelMA->setText(QString::number(pdp8State.ma,8));
    int c =      pdp8Cpu.GetTerminalOut();
    if(c !=-1){
        pdp8Cpu.terminal_out.pop();
        ui->simpleConsole->putData(QChar::fromLatin1(c));
    }

   // ui->labelAC->setText(QString::number(pdp8State.ac,8));

}

void MainWindow::on_pushButton_2_clicked()
{
   pdp8State.pc =  ui->switchRow->getSr();
               ui->labelPC->setText(QString::number(pdp8State.pc,8));
}

void MainWindow::on_pushButton_7_clicked()
{
    // stop
    pdp8State.run = false;
}

void MainWindow::on_pushButton_5_clicked()
{
      pdp8State.run = true;
}

void MainWindow::on_pushButton_6_clicked()
{
     pdp8State.step();
}
