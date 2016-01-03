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
    Ui::MainWindow *ui;
    QTimer *timer;
    std::shared_ptr<PDP8::ThreadedCPU> cpu;
    std::shared_ptr<PDP8::KL8C> kl8c;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void on_halt_signal(const QString& dsam);
private slots:
     void updateRegesters(int LAC, int PC, int MQ);
private slots:
    void on_pushButton_clicked();
    void on_timer();
    void on_halt(const QString dsam);
       void onData(int data);
       void onIncommingTeletype(int data);
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
    void srChanged(int value);
protected:
    void closeEvent(QCloseEvent *event) override;
private:
	std::vector<char>  getResource(const char* filename);
    void setSr(int value); // helper function to make sure SR is set and the signals propergate right
    void  onDataTerm(Terminal::SerialDCEInterface&i);
};

#endif // MAINWINDOW_H
