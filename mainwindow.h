#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pdp8i.h"
#include "tty_vt52.h"
#include <QMainWindow>
#include <QTimer>
#include <QQueue>
#include "console.h"

namespace Ui {
class MainWindow;
}
class TempInterface : public PDP8::SerialInterface {
    QQueue<QChar> _outData;
    Console* _console;
public:
    TempInterface() {}
    void setConsole(Console* c) { _console =c; }
    virtual bool haveData() const {
        return !_outData.empty();
    }
    virtual int received() {
        return haveData() ? _outData.dequeue().toLatin1() : -1;
    }

    virtual void trasmit(unsigned char data) {
       if(_console) _console->putData(QChar::fromLatin1(data));
    }
    void keyboardkey(const QChar& data) {
        _outData.enqueue(data);
    }

};
class MainWindow : public QMainWindow
{
    Q_OBJECT
    bool last_run_state;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_timer();
       void onData(const QChar data);
       void on_pushButton_2_clicked();

       void on_pushButton_7_clicked();

       void on_pushButton_5_clicked();

       void on_pushButton_6_clicked();

       void on_pushButton_8_clicked();

       void on_pushButton_9_clicked();

       void on_pushButton_11_clicked();

       void on_pushButton_4_clicked();

       void on_pushButton_10_clicked();

       void on_pushButton_12_clicked();

       void on_pushButton_13_clicked();

       void on_pushButton_14_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    PDP8::ThreadedCPU cpu;
    std::shared_ptr<PDP8::KL8C> kl8c;
    std::shared_ptr<TempInterface> sinterface;
};

#endif // MAINWINDOW_H
