#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pdp8i.h"
#include "tty_vt52.h"
#include <QMainWindow>
#include <QTimer>
#include <QQueue>
#include <QDebug>

#include "console.h"
#include "qtty2.h"
#include "placabletextedit.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    bool last_run_state;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void on_halt_signal(const QString& dsam);
private slots:
    void on_pushButton_clicked();
    void on_timer();
    void on_halt(const QString dsam);
       void onData(int data);
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
    void  onDataTerm(Terminal::SerialDCEInterface&i);
    Ui::MainWindow *ui;
    QTimer *timer;
    PDP8::ThreadedCPU cpu;
    std::shared_ptr<PDP8::KL8C> kl8c;
   // std::shared_ptr<Terminal::SerialDCEInterface> sinterface;
};

#endif // MAINWINDOW_H
