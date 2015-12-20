#ifndef SWITCHROW_H
#define SWITCHROW_H

#include <QWidget>
#include <QLineEdit>
#include "panelswitch.h"

class SwitchRow : public QWidget
{
    Q_OBJECT
    PanelSwitch* sr[12];
    QLineEdit* lineEdit;
    Q_PROPERTY(int sr_value MEMBER m_srvalue NOTIFY srChanged READ getSr WRITE setSr)
private:
    int m_srvalue;
public:
    explicit SwitchRow(QWidget *parent = 0);
    ~SwitchRow();
    void setSr(int value,bool sendSignal=true);
    int getSr() const { return m_srvalue; }
public slots:
    void switchPressed(bool state, int tag);
signals:
    void srChanged(int newValue);
};

#endif // SWITCHROW_H
