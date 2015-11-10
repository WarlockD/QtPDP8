#ifndef STUPIDVT52WIDGET_H
#define STUPIDVT52WIDGET_H

#include <QWidget>
class StupidVT52Screen {
    const static int colCount = 80;
    const static int lineCount = 24;
    enum class State { Normal, InEsc, NeedX, NeedY };
    State state;
    QString _lines[lineCount]; // the screen for the charaters
    int curX,curY;
public:
    StupidVT52Screen() : curX(0), curY(0),state(State::Normal) {
        for(int i=0;i<lineCount;i++) _lines[i].reserve(colCount);
    }
    QString& operator[](int i) { return _lines[i]; }
    void tab() {
        int n = (curX & 7) +8;
        if(n >colCount-1) return;
        curX = n;
    }
    int getX() const { return curX; }
    int getY() const { return curY; }
    void setCurX(int x) {curX = qMax(0,qMin(colCount-1, x));}
    void setCurY(int y) {curY = qMax(0,qMin(lineCount-1, y));}
    void setCurXY(int x, int y) { setCurX(x); setCurY(y); }
    void curUp() { curY = qMax(0,curY-1); }
    void curDown() { curY = qMin(lineCount-1,curY+1); }
    void curLeft() { curX = qMax(0,curX-1); }
    void curRight() { curX = qMin(colCount-1,curX+1); }
    void backspace() {
        if(curX == 0) return;
        QString& s = _lines[curY];
        if(s.count()-1 == curX && s[curX] == ' ') s.resize(s.count()-1); // trim
        curX--;
    }

    void startOfLine() { curX = 0; }
    void nextLine() { startOfLine(); scrollUp(); }
    void scrollUp() {
        if(curY  != (lineCount-1)) curY++;
        else {

            for(int i=0;i<curY;i++)
                _lines[i] = _lines[i+1];
            _lines[curY].clear();
        }

    }
    void scrollDown() {
        if(curY  != 0) curY--;
        else {
            for (int i=colCount-1;i>0;i--) _lines[i] = _lines[i-1];
            _lines[0].clear();
        }
    }
    void deleteChars(int n=1){
        // if cursor is beyond the end of the line there is nothing to do
        QString& s = _lines[curY];
        if ( curX >= s.count() )return;
        if ( curX+n > s.count() )  n = s.count() - curX;
        s.remove(curX,n);
    }
    void insertChars(int n=1) {
        QString& s = _lines[curY];
        if (s.size() < (curX+n) ) s.resize(curX+n);
        for(int i=curX;i<(curX+n);i++)_lines[i] = ' ';
        if (s.count() > colCount ) s.resize(colCount);
    }
    void clearToEndOfScreen() {
        QString& s = _lines[curY];
        s.resize(curX);
        for(int i=curY;i<lineCount;i++) _lines[i].clear();
    }
    void clearEntireScreen() {
         for(int i=0;i<lineCount;i++) _lines[i].clear();
    }

    void putChar(const  QChar c) {
        if(curX == (colCount-1)) { // we don't scroll down on a vt52
             _lines[curY][curX] = c;
        } else {
            QString& s = _lines[curY];
            if(s.size() < (curX+1)) {
                s.resize(curX+1);
                for(int i=curX;i<(curX+1);i++)_lines[i] = ' ';
            }
            curX++;
            s[curX] = c;
        }

    }
    bool putRawData(int c) {
        switch(state) {
            case State::NeedX:
            setCurX(c - 040);
            state = State::NeedY;
            break;
        case State::NeedY:
            setCurY(c - 040);
            state = State::Normal;
            break;
        case State::InEsc:
            switch(c) {
                case 'A': curUp(); break;
                   case 'B': curDown(); break;
                    case 'C': curRight(); break;
                    case 'D': curLeft(); break;
                   // case 'F': curUp(); break;
                   // case 'G': curUp(); break;
                case 'H': curY = curX = 0; break;
                case 'I': scrollDown(); break;
                  case 'J': clearToEndOfScreen(); break;
                case 'Y':
                    state = State::NeedX;
                    return false;
                case 'Z': // send idientifyer
                    state = State::Normal;
                    return true;
                    break;
                }
            state = State::Normal;
            break;
        case State::Normal:
        case '\r': curX = 0; break;
        case '\n': scrollUp(); break;
        default:
            putChar(QChar::fromLatin1(c));
            break;

    }
        return false;
    }
};

class StupidVT52Widget
{
    const static int _usedColumns = 80;
    const static int _usedLines = 24;
    const static int _leftMargin = 24;
    const static int _topMargin = 24;

    struct CharInfo {
        QChar c;
        unsigned int r; // misc info on it but nothing really for the VT52
    };
    int _fontWidth;
    int _fontHeight;
    QFont* _font;
    QTimer* _blinker;
    CharInfo _screen[24][80]; // the screen for the charaters


    // maybe make a pix map for the background?
public: // screen interface


public:
    StupidVT52Widget();

signals:

protected slots:
  //  void scrollBarPositionChanged(int value);
 //   void blinkEvent();
  //  void blinkCursorEvent();

    //Renables bell noises and visuals.  Used to disable further bells for a short period of time
    //after emitting the first in a sequence of bell events.
  //  void enableBell();
protected:
     virtual void keyPressEvent(QKeyEvent* event);
     virtual void paintEvent( QPaintEvent* pe );
private:
    void drawBackground(QPainter& painter, const QRect& rect, const QColor& color,bool useOpacitySetting);
    void drawContents(QPainter& painter, const QRect& rect);
    void drawContentsStupid(QPainter& painter);
};

#endif // STUPIDVT52WIDGET_H
