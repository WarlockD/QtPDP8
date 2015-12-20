#include "switchrow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "octalvalidator.h"

SwitchRow::SwitchRow(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    lineEdit = new QLineEdit;
    lineEdit->setValidator(new OctalValidator);
    connect(lineEdit,&QLineEdit::editingFinished,
            this,[this]()
    {
        bool ok;
        setSr(lineEdit->text().toInt(&ok,8));
    });
    int count = 0;
    for(int i=11;i>=0;i--,count++) { // add in reverse order
        QVBoxLayout *with_label = new QVBoxLayout;

        QLabel* switch_label = new QLabel;
        switch_label->setText(QString::number(count)); // label text has to be in reverse
        switch_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        with_label->addWidget(switch_label);

        PanelSwitch* s = new PanelSwitch;
        switch(i) {
            case 0: case 1: case 2: case 6: case 7: case 8: s->setColor(Qt::white); break;
        }
        s->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        s->setTag(i);
        with_label->addWidget(s);


        layout->addLayout(with_label);
        connect(s,&PanelSwitch::switchPressed,this,&SwitchRow::switchPressed);
        sr[i] = s;
    }

    QVBoxLayout* vlayout = new QVBoxLayout;
QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addStretch();
    hlayout->addWidget(lineEdit);
    hlayout->addStretch();
    vlayout->addLayout(hlayout);
      vlayout->addLayout(layout);

    this->setLayout(vlayout);
      setSr(0);
}
SwitchRow::~SwitchRow() {
    //::~QWidget();
}

void SwitchRow::setSr(int value,bool sendSignal) {
    if (value != m_srvalue) {
            m_srvalue = value;

            for(int i=0;i <12; i++,value>>=1){
                sr[i]->setDown(value&1);
            }
            lineEdit->setText(QString::number(m_srvalue,8));
            update();
            if(sendSignal) emit srChanged(m_srvalue); // check, is there a way we can run both these at once
        }
}

 void SwitchRow::switchPressed(bool state, int tag) {
     bool value = (m_srvalue >> tag) & 1 ? true : false;
     if(value != state) { // we will check all the switch states at once
         int n_value = 0;
         for(int i=0;i<12;i++)if(sr[i]->isDown()) n_value |= (1<<i);
         m_srvalue = n_value;

         lineEdit->setText(QString::number(m_srvalue,8));
         emit srChanged(m_srvalue);
     }
 }
