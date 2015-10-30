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

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    PDP8::ThreadedCPU cpu;
    std::shared_ptr<PDP8::SimpleTTY> tty;
};

#endif // MAINWINDOW_H
