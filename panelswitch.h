#ifndef PANELSWITCH_H
#define PANELSWITCH_H

#include <QWidget>
#include <QAbstractButton>

// Fyi, to avoid circulr emit's, setDown does NOT call switchChanged.
// switchdown is ment to init the switch state anyway
class PanelSwitch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool down MEMBER m_down NOTIFY switchChanged READ isDown WRITE setDown)
    Q_PROPERTY(int tag MEMBER m_tag  READ getTag WRITE setTag)  // Simple tag for rows of switches, so we don't have to look up QProperty each time
    Q_PROPERTY(bool momantary MEMBER m_momantary)
    Q_PROPERTY(QColor lightShade MEMBER m_lightShade)
    Q_PROPERTY(QColor darkShade MEMBER m_darkShade)

private:
    int m_tag;
    bool m_startState;
    bool m_down;
    bool m_momantary;
    QColor m_lightShade;
    QColor m_darkShade;

public:
    explicit PanelSwitch(QWidget *parent = 0);
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    inline void setColor(const QColor& c) {
        m_lightShade = c;
        m_darkShade = c.darker();
        update();
    }
    inline bool isDown() const { return m_down; }
    inline void setTag(int i) { m_tag = i; }
    inline int getTag() const { return m_tag; }
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *e) Q_DECL_OVERRIDE;

public slots:
    void setDown(bool down);
Q_SIGNALS:
    void switchChanged(bool state);
    void switchPressed(bool state, int tag);



};

#endif // PANELSWITCH_H
