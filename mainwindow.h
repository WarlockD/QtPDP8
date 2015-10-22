#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "pdp8i.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

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

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    PDP8::PDP8_State pdp8State;
    PDP8::Emx8 pdp8Cpu;


};

#endif // MAINWINDOW_H
