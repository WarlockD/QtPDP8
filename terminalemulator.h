#ifndef TERMINALEMULATOR_H
#define TERMINALEMULATOR_H

// This is a base class that handles a basic char based terminal
// Trying to use as much std as I can so I can port this easyer to other
// platforms


#include <string>
#include <vector>
#include <cstddef>
#include <set>
#include <type_traits>

#define TE_HISTORY_ENABLED

namespace TerminalEmulator {
enum class Mode : uint8_t {
Origin ,Wrap ,Insert ,Screen ,Cursor  ,NewLine
};
class ScreenParm {
    std::set<Mode>  modes;
public:
    inline bool get(Mode m) const { auto i = modes.find(m); return i != modes.cend();}
    inline void set(Mode m) { modes.insert(m); }
    inline void reset(Mode m) { auto i = modes.find(m); if(i != modes.end()) modes.erase(i); }
};



enum class Rendition : uint8_t {
    None =0, Bold = 1, Blink = 2, Underline = 4, Reverse = 4
};
// simple overloads for some enum class bit operations
inline Rendition operator |(Rendition l, Rendition r) {
    using T = std::underlying_type<Rendition>::type;
    return (Rendition)(static_cast<T>(l) | static_cast<T>(r));
}
inline Rendition& operator |=(Rendition& l, Rendition r) {
    l = l | r; return l; // use the l Function
}
inline Rendition operator &(Rendition l, Rendition r) {
    using T = std::underlying_type<Rendition>::type;
    return (Rendition)(static_cast<T>(l) & static_cast<T>(r));
}
inline Rendition operator ~(Rendition l) {
    using T = std::underlying_type<Rendition>::type;
    return (Rendition)(~static_cast<T>(l));
}
inline Rendition& operator &=(Rendition& l, Rendition r) {
    l = l & r; return l; // use the l Function
}
inline bool operator ==(Rendition l, Rendition r) {
    using T = std::underlying_type<Rendition>::type;
    return (static_cast<T>(l) &  static_cast<T>(r)) != 0; // check if flag is in thes state
}
inline bool operator !=(Rendition l, Rendition r) { return !(l == r); }

// Class that contains the charater as well as the attributes
struct CharAttributes {
    uint16_t c;
    uint8_t f;
    uint8_t b;
    Rendition r;
    inline CharAttributes() : c(' '), f(0), b(1), r(Rendition::None) {} // Just in case
    inline CharAttributes(uint16_t C, uint8_t F = 0, uint8_t B = 1, Rendition R = Rendition::None) : c(C), f(F), b(B), r(R) {}
  //  CharAttributes(const CharAttributes& a) : c(a.c),f(a.f), b(a.b),r(a.r) {}
  //  CharAttributes(CharAttributes&& a) : c(std::move(a.c)),f(std::move(a.f)),b(std::move(a.b)),r(std::move(a.r)) {}
  // CharAttributes& operator=(CharAttributes const &m) noexcept { *this = m; return *this; }
   // CharAttributes& operator=(CharAttributes &&m) noexcept { c = std::move(m.c);  f = std::move(m.f); b = std::move(m.b); r = std::move(m.r); return *this;  }
    inline void reverse() { std::swap(f,b);  /*p->r &= ~RE_TRANSPARENT; */}
    inline bool operator==(const CharAttributes& l) { return c == l.c && f == l.f && b == l.b && r == l.r; }
    inline bool operator!=(const CharAttributes& l) { return !(*this == l); }
};


/*
   An extendable tmpfile(1) based buffer.
*/
class HistoryBuffer
{
public:
  HistoryBuffer();
 ~HistoryBuffer();

public:
  void setScroll(bool on);
  bool hasScroll();

public:
  void add(const unsigned char* bytes, int len);
  void get(unsigned char* bytes, int len, int loc);
  int  len();

private:
  int  ion;
  int  length;
};

class HistoryScroll
{
public:
  HistoryScroll();
 ~HistoryScroll();

public:
  void setScroll(bool on);
  bool hasScroll();

public: // access to history
  int  getLines();
  int  getLineLen(int lineno);
  void getCells(int lineno, int colno, int count, CharAttributes* res);

public: // backward compatibility (obsolete)
  CharAttributes   getCell(int lineno, int colno) { CharAttributes res; getCells(lineno,colno,1,&res); return res; }

public: // adding lines.
  void addCells(CharAttributes* a, int count);
  void addLine();

private:
  int startOfLine(int lineno);
  HistoryBuffer index; // lines Row(int)
  HistoryBuffer cells; // text  Row(ca)
};


// Base class that contains the basic screen and functions ot manipulate it
class Screen
{
public:
    Screen(int lines, int columns);
    ~Screen();
public: // these are all `Screen' operations
    //
    // VT100/2 Operations ------------------
    //
    // Cursor Movement
    //
    void cursorUp    (int n);
    void cursorDown  (int n);
    void cursorLeft  (int n);
    void cursorRight (int n);
    void setCursorY  (int y);
    void setCursorX  (int x);
    void setCursorYX (int y, int x);
    void setMargins  (int t, int b);
    //
    // Cursor Movement with Scrolling
    //
    void NewLine     ();
    void NextLine    ();
    void index       ();
    void reverseIndex();
    //
    void Return      ();
    void BackSpace   ();
    void Tabulate    ();
    //
    // Editing
    //
    void eraseChars  (int n);
    void deleteChars (int n);
    void insertChars (int n);
    void deleteLines (int n);
    void insertLines (int n);
    //
    // -------------------------------------
    //
    void clearTabStops();
    void changeTabStop(bool set);
    //
    void resetMode   (Mode n);
    void setMode     (Mode n);
    void saveMode    (Mode n);
    void restoreMode (Mode n);
    //
    void saveCursor  ();
    void restoreCursor();
    //
    // -------------------------------------
    //
    void clearEntireScreen();
    void clearToEndOfScreen();
    void clearToBeginOfScreen();
    //
    void clearEntireLine();
    void clearToEndOfLine();
    void clearToBeginOfLine();
    //
    void helpAlign   ();
    //
    // -------------------------------------
    //
    void setRendition  (Rendition rendition);
    void resetRendition(Rendition rendition);
    void setForeColor  (int fgcolor);
    void setBackColor  (int bgcolor);
    //
    void setDefaultRendition();
    void setForeColorToDefault();
    void setBackColorToDefault();
    //
    // -------------------------------------
    //
    bool getMode(Mode n);
    //
    // only for report cursor position
    //
    int  getCursorX();
    int  getCursorY();
    //
    // -------------------------------------
    //
    void clear();
    void home();
    void reset();
    //
    void ShowCharacter(unsigned short c);
    //
    void resizeImage(int new_lines, int new_columns);
    //
    std::vector<CharAttributes>  getCookedImage();

    /*! return the number of lines. */
    inline int  getLines()  const { return lines; }
    /*! return the number of columns. */
    inline int  getColumns() const { return columns; }



    //
    // Selection
    //
    void setSelBeginXY(const int x, const int y);
    void setSelExtentXY(const int x, const int y);
    void clearSelection();


    void checkSelection(int from, int to);


    // Originaly QString functions
    std::string getSelText(const bool preserve_line_breaks);
    std::string getHistory();

private: // helper

    void clearImage(int loca, int loce, char c);
    void moveImage(int dst, int loca, int loce);

    void scrollUp(int from, int i);
    void scrollDown(int from, int i);

    void addHistLine();

    void initTabStops();

    void effectiveRendition();
    void reverseRendition(CharAttributes& p);
#ifdef TE_HISTORY_ENABLED
public:
    /*! set the position of the history cursor. */
    void setHistCursor(int cursor);
    /*! return the position of the history cursor. */
    int  getHistCursor();

    int  getHistLines ();
    void setScroll(bool on);
    bool hasScroll();
private:
    int histCursor;   // display position relative to start of the history buffer
    HistoryScroll hist;
#endif
private:

    /*
       The state of the screen is more complex as one would
       expect first. The screem does really do part of the
       emulation providing state informations in form of modes,
       margins, tabulators, cursor etc.

       Even more unexpected are variables to save and restore
       parts of the state.
    */

    // screen image ----------------

    int lines;
    int columns;
    std::vector<CharAttributes> image; // [lines][columns]

    // history buffer ---------------



    // cursor location

    int cuX;
    int cuY;

    // cursor color and rendition info

    uint8_t cu_fg;      // foreground
    uint8_t cu_bg;      // background
    Rendition cu_re;      // rendition

    // margins ----------------

    int tmargin;      // top margin
    int bmargin;      // bottom margin

    // states ----------------

    ScreenParm currParm;

    // ----------------------------

    bool* tabstops;

    // selection -------------------

    int sel_begin; // The first location selected.
    int sel_TL;    // TopLeft Location.
    int sel_BR;    // Bottom Right Location.

    // effective colors and rendition ------------

    uint8_t ef_fg;      // These are derived from
    uint8_t ef_bg;      // the cu_* variables above
    Rendition ef_re;      // to speed up operation

    //
    // save cursor, rendition & states ------------
    //

    // cursor location

    int sa_cuX;
    int sa_cuY;

    // rendition info

    Rendition sa_cu_re;
    uint8_t sa_cu_fg;
    uint8_t sa_cu_bg;

    // modes

    ScreenParm saveParm;
};

}


#endif // TERMINALEMULATOR_H
