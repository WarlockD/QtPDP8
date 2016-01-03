#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pdp8_utilities.h"
#include "pdp8_rf08.h"
#include <QDebug>
#include <QQueue>
#include <QCloseEvent>

#include "pdp11_cpu.h"

void MainWindow::on_halt(const QString dsam) {

}
void MainWindow::srChanged(int value) {
    if(cpu) {
        cpu->waitLock([this,value](PDP8::ThreadedCPU& c){

            c.setSR(value);
        });
    }
}
   void MainWindow::setSr(int value) {
       ui->switchRow->setSr(value);
   }

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	
    timer = new QTimer(this);
      connect(ui->switchRow,&SwitchRow::srChanged,this,&MainWindow::srChanged); // to make sure sr matches the switches


    connect(timer,&QTimer::timeout,this,&MainWindow::on_timer,Qt::QueuedConnection);



       timer->start(1000/15); // update 30 times a sec

       connect(ui->simpleConsole,&QTTY2::keyboardSend,this,&MainWindow::onData);
       connect(ui->stupidTerm,&PlacableTextEdit::keyboardSend,this,&MainWindow::onData);
       connect(this,&MainWindow::on_halt_signal,ui->dsamEdit,&QPlainTextEdit::setPlainText);
    //    connect(&_silly,&PDP8::PDP8_Silly::consoleOut,this,&MainWindow::onIncommingTeletype);
    //     connect(&_silly,&PDP8::PDP8_Silly::updateRegesters,this,&MainWindow::updateRegesters);

         // lets make the cpu

    //PDP8::ThreadedCPU* test = new PDP8::ThreadedCPU;

    cpu = std::make_shared<PDP8::ThreadedCPU>();
    kl8c = std::make_shared<PDP8::KL8C>(*cpu);
    std::shared_ptr<PDP8::RF08> rfptr = std::make_shared<PDP8::RF08>(*cpu);

    connect(ui->btnBreakPoint,&QPushButton::clicked,[this](){
       if(cpu) cpu->setBreakPoint(ui->switchRow->getSr());
    });
    auto test = std::bind(&MainWindow::onDataTerm,this,std::placeholders::_1);
    kl8c->setCallBack(test);

    //auto func([this](PDP8::Cpu& c){
   //     QString str = QString::fromStdString(c.printHistory(16));
   //     emit on_halt_signal(str);
   // });
     //connect(ui->simpleConsole,&QTTY2::keyboardSend,this,&MainWindow::onData);


}

MainWindow::~MainWindow()
{
    if(cpu) {
        cpu->panelSwitch(PDP8::PanelToggleSwitch::LoadAdd);
    }

    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event){
    if(cpu) {
        cpu->panelSwitch(PDP8::PanelToggleSwitch::Stop);
         kl8c = nullptr;
        cpu = nullptr;

    }
}


void MainWindow::onDataTerm(Terminal::SerialDCEInterface&i)
{
    uint8_t data;
    while(i.recive(data)) {
        ui->stupidTerm->putChar(data);
    }
}

void MainWindow::onIncommingTeletype(int data) {
  emit ui->stupidTerm->putCharSignal(data);
}

void MainWindow::on_pushButton_clicked()
{
   //  _silly.setRunningState(PDP8::RunningState::Run);
    if(cpu) cpu->panelSwitch(PDP8::PanelToggleSwitch::Start);
}

  void MainWindow::onData(int data) {
    //    _silly.keyPress(data);
      if(kl8c) Q_ASSERT(kl8c->trasmit(data)); // for now this should always work


  }
 //static std::mutex _ugh;
 void MainWindow::updateRegesters(int LAC, int PC, int MQ){
     QString s_lac = QString::number(LAC,8);
     QString s_pc = QString::number(PC,8);
     QString s_mq = QString::number(MQ,8);

     ui->labelAC->setText(s_lac);
     ui->labelPC->setText(s_pc);
     ui->labelMQ->setText(s_mq);
 }

void MainWindow::on_timer() {

    if(cpu) {
        cpu->trylock([this](PDP8::ThreadedCPU& c){ // just try
              PDP8::Regesters& r = c.regs();
            ui->labelAC->setText(QString::number(r.lac,8));
            ui->labelPC->setText(QString::number(r.pc,8));
            ui->labelMQ->setText(QString::number(r.mq,8));
            ui->labelMB->setText(QString::number(r.mb,8));
            ui->labelMA->setText(QString::number(r.ma,8));
            switch(c.state()) {
            case PDP8::State::Fetch: ui->labelState->setText(QStringLiteral("Fetch")); break;
            case PDP8::State::Defer: ui->labelState->setText(QStringLiteral("Defer")); break;
            case PDP8::State::Execute: ui->labelState->setText(QStringLiteral("Exec")); break;
            default:
                ui->labelState->setText(QStringLiteral("Err")); break;
            }
        });
    }

}

void MainWindow::on_pushButton_2_clicked()
{
   // _silly.setRunningState(RunningState::Run);
//_silly.loadADD();
    if(cpu) cpu->panelSwitch(PDP8::PanelToggleSwitch::LoadAdd);
}

void MainWindow::on_pushButton_7_clicked()
{
   //  _silly.setRunningState(PDP8::RunningState::Stop);
    if(cpu) cpu->panelSwitch(PDP8::PanelToggleSwitch::Stop);
}

void MainWindow::on_pushButton_5_clicked()
{
      if(cpu) cpu->panelSwitch(PDP8::PanelToggleSwitch::Cont);
}

void MainWindow::on_pushButton_6_clicked()
{
     //_silly.setRunningState(PDP8::RunningState::SingleStepInstruction);
    if(cpu) cpu->panelSwitch(PDP8::PanelToggleSwitch::SingleStepInstruction);
}

// this is kind of cool
void externalLibFunction(std::istream & input_stream) {
    std::copy(std::istream_iterator<std::string>(input_stream),
         std::istream_iterator<std::string>(),
         std::ostream_iterator<std::string>(std::cout, " "));
}

std::ifstream QFileToifstream(QFile & file) {
    Q_ASSERT(file.isReadable());
    return std::ifstream(::_fdopen(file.handle(), "r"));
}
/*
void ResrouceRim(const char* filename, PDP8::CpuState& cpu) {
   std::vector<char> data = GetResource(filename);
    PDP8::LoadRim(data,cpu);
}
void ResrouceBin(const char* filename, PDP8::CpuState& cpu) {
   std::vector<char> data = GetResource(filename);
    PDP8::LoadBin(data,cpu);
}
*/
std::vector<char>  MainWindow::getResource(const char* filename) {

	QFile s(filename);
	if (s.open(QFile::ReadOnly)) {
		QByteArray bytes = s.readAll();
		std::vector<char> data(bytes.begin(), bytes.end());
		return data;
	}
	throw std::runtime_error("FUCK YOU");
}
std::vector<char> getFile(const char* filename) {
    std::vector<char> vec;
    std::ifstream file;
    file.exceptions(
        std::ifstream::badbit
      | std::ifstream::failbit
      | std::ifstream::eofbit);
    file.open(filename, std::ifstream::in | std::ifstream::binary);
    file.seekg(0, std::ios::end);
    std::streampos length(file.tellg());
    if (length) {
        file.seekg(0, std::ios::beg);
        vec.resize(static_cast<std::size_t>(length));
        file.read(&vec.front(), static_cast<std::size_t>(length));
    }
    return vec;
}


void MainWindow::on_pushButton_8_clicked()
{
    if(cpu) {
        cpu->power();
      //  PDP8::LoadRim(getFile("MAINDEC-8E-D0AB-InstTest-1.pt"),cpu->mem().data());
        PDP8::LoadRim(getResource(":/diags/MAINDEC-8I-D01C-PB.pt"), cpu->data());
        setSr(07777);
        cpu->regs().ma = 0200;
        cpu->regs().pc = 0200;
    }
}

void MainWindow::on_pushButton_9_clicked()
{
    if(cpu) {
        cpu->power();
      //  PDP8::LoadRim(getFile("MAINDEC-8E-D0AB-InstTest-1.pt"),cpu->mem().data());
        PDP8::LoadRim(getResource(":/diags/MAINDEC-8I-D02B-PB.pt"), cpu->data());
        setSr(07777);
        cpu->regs().ma = 0200;
        cpu->regs().pc = 0200;
    }
}

void MainWindow::on_pushButton_11_clicked()
{
    if(cpu) cpu->panelSwitch(PDP8::PanelToggleSwitch::Clear);
}

void MainWindow::on_pushButton_4_clicked()
{
    if(cpu) cpu->panelSwitch(PDP8::PanelToggleSwitch::Exam);
}

void MainWindow::on_pushButton_10_clicked()
{
    if(cpu) cpu->power();
//cpu.panelSwitch(PDP8::PanelToggleSwitch::Power);
    try{
        PDP8::LoadBin("focal69.bin",*cpu);
    }catch(PDP8::PDP8_Exception& e) {
 (void)e;
    }

    ui->switchRow->setSr(07777);
    if(cpu) cpu->regs().ma = 0200;
    if(cpu) cpu->regs().pc = 0200;
}

void MainWindow::on_pushButton_12_clicked()
{
    if(cpu) cpu->power();
   // try{
    //      PDP8::LoadBin("MAINDEC-08-DHKLD-TTYTest.pt",cpu);
       // PDP8::LoadBin("focal69.bin",cpu);
   // }catch(PDP8::PDP8_Exception& e) {

   // }
    /*
    ResrouceRim("MAINDEC-08-DHKLD-TTYTest.pt",*cpu);
    ui->switchRow->setSr(0);
    cpu[0020] = 0;  // ASR 33
    qDebug() << "Old Code :" << QString::number(cpu[0021],8);
   // cpu[0021] = 03040;  // devices
    cpu[0022] = 0110;  // devices
    cpu.regs().ma = 0200;
    cpu.regs().pc = 0200;

    last_run_state= false;
*/

}

void MainWindow::on_pushButton_13_clicked()
{
    /*
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
    */
}

void MainWindow::on_pushButton_14_clicked()
{
    /*
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
    */
}
