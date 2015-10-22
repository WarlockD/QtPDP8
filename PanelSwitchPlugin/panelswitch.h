#ifndef PANELSWITCH_H
#define PANELSWITCH_H

#include <QWidget>
#include <QAbstractButton>
#include <QCheckBox>
#include <QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT  PanelSwitch : public QCheckBox
{
    Q_OBJECT
private:
    bool _on;
    bool _startState;
    bool _toggle;
    QColor _lightShade;
    QColor _darkShade;

public:
    explicit PanelSwitch(QWidget *parent = 0);
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

signals:

public slots:
};

#endif // PANELSWITCH_H
