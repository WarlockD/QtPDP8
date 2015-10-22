#include "terminalemulator.h"
#include <cassert>
namespace TerminalEmulator {

#define HERE printf("%s(%d): here\n",__FILE__,__LINE__)

//FIXME: this is emulation specific. Use FALSE for xterm, TRUE for ANSI.
//FIXME: see if we can get this from terminfo.
#define BS_CLEARS false
#define BASE_COLORS 8
#define loc(X,Y) ((Y)*columns+(X))

/*! creates a `Screen' of `lines' lines and `columns' columns.
*/

Screen::Screen(int lines, int columns)
{
  this->lines   = lines;
  this->columns = columns;
    image.resize(lines*columns);
  tabstops   = NULL; initTabStops();

  histCursor = 0;

  clearSelection();
  reset();
}

/*! Destructor
*/

Screen::~Screen()
{
  if (tabstops) free(tabstops);
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Normalized                    Screen Operations                           */
/*                                                                           */
/* ------------------------------------------------------------------------- */

// Cursor Setting --------------------------------------------------------------

/*! \section Cursor

    The `cursor' is a location within the screen that is implicitely used in
    many operations. The operations within this section allow to manipulate
    the cursor explicitly and to obtain it's value.

    The position of the cursor is guarantied to be between (including) 0 and
    `columns-1' and `lines-1'.
*/

/*!
    Move the cursor up.

    The cursor will not be moved beyond the top margin.
*/

void Screen::cursorUp(int n)
//=CUU
{
  if (n == 0) n = 1; // Default
  int stop = cuY < tmargin ? 0 : tmargin;

  cuX = std::min(columns-1,cuX); // nowrap!
  cuY = std::max(stop,cuY-n);
}

/*!
    Move the cursor down.

    The cursor will not be moved beyond the bottom margin.
*/

void Screen::cursorDown(int n)
//=CUD
{
  if (n == 0) n = 1; // Default
  int stop = cuY > bmargin ? lines-1 : bmargin;
  cuX =   std::min(columns-1,cuX); // nowrap!
  cuY =   std::min(stop,cuY+n);
}

/*!
    Move the cursor left.

    The cursor will not move beyond the first column.
*/

void Screen::cursorLeft(int n)
//=CUB
{
  if (n == 0) n = 1; // Default
  cuX =   std::min(columns-1,cuX); // nowrap!
  cuX =   std::max(0,cuX-n);
}

/*!
    Move the cursor left.

    The cursor will not move beyond the rightmost column.
*/

void Screen::cursorRight(int n)
//=CUF
{
  if (n == 0) n = 1; // Default
  cuX =   std::min(columns-1,cuX+n);
}

/*!
    Set top and bottom margin.
*/

void Screen::setMargins(int top, int bot)
//=STBM
{
  if (top == 0) top = 1;      // Default
  if (bot == 0) bot = lines;  // Default
  top = top - 1;              // Adjust to internal lineno
  bot = bot - 1;              // Adjust to internal lineno
  if ( !( 0 <= top && top < bot && bot < lines ) )
  { fprintf(stderr,"%s(%d) : setRegion(%d,%d) : bad range.\n",
                   __FILE__,__LINE__,top,bot);
    return;                   // Default error action: ignore
  }
  tmargin = top;
  bmargin = bot;
  cuX = 0;
  cuY = getMode(Mode::Origin) ? top : 0;
}

/*!
    Move the cursor down one line.

    If cursor is on bottom margin, the region between the
    actual top and bottom margin is scrolled up instead.
*/

void Screen::index()
//=IND
{
  if (cuY == bmargin)
  {
    if (tmargin == 0 && bmargin == lines-1) addHistLine(); // hist.history
    scrollUp(tmargin,1);
  }
  else if (cuY < lines-1)
    cuY += 1;
}

/*!
    Move the cursor up one line.

    If cursor is on the top margin, the region between the
    actual top and bottom margin is scrolled down instead.
*/

void Screen::reverseIndex()
//=RI
{
  if (cuY == tmargin)
     scrollDown(tmargin,1);
  else if (cuY > 0)
    cuY -= 1;
}

/*!
    Move the cursor to the begin of the next line.

    If cursor is on bottom margin, the region between the
    actual top and bottom margin is scrolled up.
*/

void Screen::NextLine()
//=NEL
{
  Return(); index();
}

// Line Editing ----------------------------------------------------------------

/*! \section inserting / deleting characters
*/

/*! erase `n' characters starting from (including) the cursor position.

    The line is filled in from the right with spaces.
*/

void Screen::eraseChars(int n)
{
  if (n == 0) n = 1; // Default
  int p = std::max(0,std::min(cuX+n-1,columns-1));
  clearImage(loc(cuX,cuY),loc(p,cuY),' ');
}

/*! delete `n' characters starting from (including) the cursor position.

    The line is filled in from the right with spaces.
*/

void Screen::deleteChars(int n)
{
  if (n == 0) n = 1; // Default
  int p = std::max(0,std::min(cuX+n,columns-1));
  moveImage(loc(cuX,cuY),loc(p,cuY),loc(columns-1,cuY));
  clearImage(loc(columns-n,cuY),loc(columns-1,cuY),' ');
}

/*! insert `n' spaces at the cursor position.

    The cursor is not moved by the operation.
*/

void Screen::insertChars(int n)
{
  if (n == 0) n = 1; // Default
  int p = std::max(0,std::min(columns-1-n,columns-1));
  int q = std::max(0,std::min(cuX+n,columns-1));
  moveImage(loc(q,cuY),loc(cuX,cuY),loc(p,cuY));
  clearImage(loc(cuX,cuY),loc(q-1,cuY),' ');
}

/*! delete `n' lines starting from (including) the cursor position.

    The cursor is not moved by the operation.
*/

void Screen::deleteLines(int n)
{
  if (n == 0) n = 1; // Default
  scrollUp(cuY,n);
}

/*! insert `n' lines at the cursor position.

    The cursor is not moved by the operation.
*/

void Screen::insertLines(int n)
{
  if (n == 0) n = 1; // Default
  scrollDown(cuY,n);
}

// Mode Operations -----------------------------------------------------------

/*! Set a specific mode. */

void Screen::setMode(Mode m)
{
    currParm.set(m);
    if(m==Mode::Origin) { cuX = 0; cuY = tmargin; } //FIXME: home)
}

/*! Reset a specific mode. */

void Screen::resetMode(Mode m)
{
    currParm.reset(m);
    if(m==Mode::Origin) {  cuX = 0; cuY = 0; } //FIXME: home)
}

/*! Save a specific mode. */

void Screen::saveMode(Mode m)
{
    if(currParm.get(m)) saveParm.set(m); else saveParm.reset(m);

}
/*! Restore a specific mode. */

void Screen::restoreMode(Mode m)
{
    if(saveParm.get(m)) currParm.set(m); else currParm.reset(m);
}


/*! Save the cursor position and the rendition attribute settings. */

void Screen::saveCursor()
{
  sa_cuX     = cuX;
  sa_cuY     = cuY;
  sa_cu_re   = cu_re;
  sa_cu_fg   = cu_fg;
  sa_cu_bg   = cu_bg;
}

/*! Restore the cursor position and the rendition attribute settings. */

void Screen::restoreCursor()
{
  cuX     = std::min(sa_cuX,columns-1);
  cuY     = std::min(sa_cuY,lines-1);
  cu_re   = sa_cu_re;
  cu_fg   = sa_cu_fg;
  cu_bg   = sa_cu_bg;
  effectiveRendition();
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                             Screen Operations                             */
/*                                                                           */
/* ------------------------------------------------------------------------- */

/*! Assing a new size to the screen.

    The topmost left position is maintained, while lower lines
    or right hand side columns might be removed or filled with
    spaces to fit the new size.

    The region setting is reset to the whole screen and the
    tab positions reinitialized.
*/

void Screen::resizeImage(int new_lines, int new_columns)
{

  if (cuY > new_lines-1)
  { // attempt to preserve focus and lines
    bmargin = lines-1; //FIXME: margin lost
    for (int i = 0; i < cuY-(new_lines-1); i++)
    {
      addHistLine(); scrollUp(0,1);
    }
  }

  // make new image
  std::vector<CharAttributes> newimg(new_lines*new_columns);


  clearSelection();


  int cpy_lines   = std::min(new_lines,  lines);
  int cpy_columns = std::min(new_columns,columns);
  // copy to new image
  for (int y = 0; y < cpy_lines; y++)
  for (int x = 0; x < cpy_columns; x++)
  {
    newimg[y*new_columns+x] = image[loc(x,y)];
    newimg[y*new_columns+x] = image[loc(x,y)];
    newimg[y*new_columns+x] = image[loc(x,y)];
    newimg[y*new_columns+x] = image[loc(x,y)];
  }
  image = std::move(newimg); // I never know when the compiler is going to move something or not

  lines = new_lines;
  columns = new_columns;
  cuX =  std::min(cuX,columns-1);
  cuY =  std::min(cuY,lines-1);

  // FIXME: try to keep values, evtl.
  tmargin=0;
  bmargin=lines-1;
  initTabStops();
  clearSelection();
}

/*
   Clarifying rendition here and in TEWidget.

   currently, TEWidget's color table is
     0       1       2 .. 9    10 .. 17
     dft_fg, dft_bg, dim 0..7, intensive 0..7

   cu_fg, cu_bg contain values 0..8;
   - 0    = default color
   - 1..8 = ansi specified color

   re_fg, re_bg contain values 0..17
   due to the TEWidget's color table

   rendition attributes are

      attr           widget screen
      -------------- ------ ------
      RE_UNDERLINE     XX     XX    affects foreground only
      RE_BLINK         XX     XX    affects foreground only
      RE_BOLD          XX     XX    affects foreground only
      RE_REVERSE       --     XX
      RE_TRANSPARENT   XX     --    affects background only
      RE_INTENSIVE     XX     --    affects foreground only

   Note that RE_BOLD is used in both widget
   and screen rendition. Since xterm/vt102
   is to poor to distinguish between bold
   (which is a font attribute) and intensive
   (which is a color attribute), we translate
   this and RE_BOLD in falls eventually appart
   into RE_BOLD and RE_INTENSIVE.
*/


void Screen::effectiveRendition()
// calculate rendition
{
  ef_re = cu_re & (Rendition::Underline | Rendition::Blink);
  if (cu_re == Rendition::Reverse)
  {
    ef_fg = cu_bg;
    ef_bg = cu_fg;
  }
  else
  {
    ef_fg = cu_fg;
    ef_bg = cu_bg;
  }
  if (cu_re == Rendition::Bold)
  {
    if (ef_fg < BASE_COLORS)
      ef_fg += BASE_COLORS;
    else
      ef_fg -= BASE_COLORS;
  }
}

/*!
    returns the image.

    Get the size of the image by \sa getLines and \sa getColumns.

    NOTE that the image returned by this function must later be
    freed.

*/

std::vector<CharAttributes>  Screen::getCookedImage()
{ int x,y;
    std::vector<CharAttributes> merged(lines*columns);
  CharAttributes dft;

  for (y = 0; (y < lines) && (y < (hist.getLines()-histCursor)); y++)
  {
    int len = std::min(columns,hist.getLineLen(y+histCursor));
    int yp  = y*columns;
    int yq  = (y+histCursor)*columns;

  //  hist.getCells(y+histCursor,0,(&len,merged[0])+yp);
    for (x = len; x < columns; x++) merged[yp+x] = dft;
    for (x = 0; x < columns; x++)
    {   int p=x + yp; int q=x + yq;
        if ( ( q >= sel_TL ) && ( q <= sel_BR ) )
          merged[p].reverse(); // for selection
    }
  }
  if (lines >= hist.getLines()-histCursor)
  {
    for (y = (hist.getLines()-histCursor); y < lines ; y++)
    {
       int yp  = y*columns;
       int yq  = (y+histCursor)*columns;
       int yr =  (y-hist.getLines()+histCursor)*columns;
       for (x = 0; x < columns; x++)
       { int p = x + yp; int q = x + yq; int r = x + yr;
         merged[p] = image[r];
         if ( q >= sel_TL && q <= sel_BR )
              merged[p].reverse(); // for selection
       }

    }
  }
  // evtl. inverse display
  if (getMode(Mode::Screen))
  { int i,n = lines*columns;
    for (i = 0; i < n; i++)
         merged[i].reverse(); // for reverse display
  }
  if (getMode(Mode::Cursor) && (cuY+(hist.getLines()-histCursor) < lines)) // cursor visible
    merged[loc(cuX,cuY+(hist.getLines()-histCursor))].reverse();
  return merged;
}


/*!
*/

void Screen::reset()
{
    setMode(Mode::Wrap  ); saveMode(Mode::Wrap   );  // wrap at end of margin
  resetMode(Mode::Origin); saveMode(Mode::Origin);  // position refere to [1,1]
  resetMode(Mode::Insert); saveMode(Mode::Insert);  // overstroke
    setMode(Mode::Cursor);                         // cursor visible
  resetMode(Mode::Screen);                         // screen not inverse
  resetMode(Mode::NewLine);

  tmargin=0;
  bmargin=lines-1;

  setDefaultRendition();
  saveCursor();

  clear();
}

/*! Clear the entire screen and home the cursor.
*/

void Screen::clear()
{
  clearEntireScreen();
  home();
}

/*! Moves the cursor left one column.
*/

void Screen::BackSpace()
{
  cuX = std::max(0,cuX-1);
  if (BS_CLEARS) image[loc(cuX,cuY)].c = ' ';
}

/*!
*/

void Screen::Tabulate()
{
  // note that TAB is a format effector (does not write ' ');
  cursorRight(1); while(cuX < columns-1 && !tabstops[cuX]) cursorRight(1);
}

void Screen::clearTabStops()
{
  for (int i = 0; i < columns; i++) tabstops[i-1] = false;
}

void Screen::changeTabStop(bool set)
{
  if (cuX >= columns) return;
  tabstops[cuX] = set;
}

void Screen::initTabStops()
{
  if (tabstops) free(tabstops);
  tabstops = (bool*)malloc(columns*sizeof(bool));
  // Arrg! The 1st tabstop has to be one longer than the other.
  // i.e. the kids start counting from 0 instead of 1.
  // Other programs might behave correctly. Be aware.
  for (int i = 0; i < columns; i++) tabstops[i] = (i%8 == 0 && i != 0);
}

/*!
   This behaves either as IND (Screen::Index) or as NEL (Screen::NextLine)
   depending on the NewLine Mode (LNM). This mode also
   affects the key sequence returned for newline ([CR]LF).
*/

void Screen::NewLine()
{
  if (getMode(Mode::NewLine)) Return();
  index();
}

/*! put `c' literally onto the screen at the current cursor position.

    VT100 uses the convention to produce an automatic newline (am)
    with the *first* character that would fall onto the next line (xenl).
*/

void Screen::checkSelection(int from, int to)
{
  if (sel_begin == -1) return;
  int scr_TL = loc(0, hist.getLines());
  //Clear entire selection if it overlaps region [from, to]
  if ( (sel_BR > (from+scr_TL) )&&(sel_TL < (to+scr_TL)) )
  {
    clearSelection();
  }
}

void Screen::ShowCharacter(unsigned short c)
{
  // Note that VT100 does wrapping BEFORE putting the character.
  // This has impact on the assumption of valid cursor positions.
  // We indicate the fact that a newline has to be triggered by
  // putting the cursor one right to the last column of the screen.

  if (cuX >= columns)
  {
    if (getMode(Mode::Wrap)) NextLine(); else cuX = columns-1;
  }

  if (getMode(Mode::Insert)) insertChars(1);

  int i = loc(cuX,cuY);

  checkSelection(i, i); // check if selection is still valid.

  image[i].c = c;
  image[i].f = ef_fg;
  image[i].b = ef_bg;
  image[i].r = ef_re;

  cuX += 1;
}

// Region commands -------------------------------------------------------------


/*! scroll up `n' lines within current region.
    The `n' new lines are cleared.
    \sa setRegion \sa scrollDown
*/

void Screen::scrollUp(int from, int n)
{
  if (n <= 0 || from + n > bmargin) return;
  //FIXME: make sure `tmargin', `bmargin', `from', `n' is in bounds.
  moveImage(loc(0,from),loc(0,from+n),loc(columns-1,bmargin));
  clearImage(loc(0,bmargin-n+1),loc(columns-1,bmargin),' ');
}

/*! scroll down `n' lines within current region.
    The `n' new lines are cleared.
    \sa setRegion \sa scrollUp
*/

void Screen::scrollDown(int from, int n)
{
//FIXME: make sure `tmargin', `bmargin', `from', `n' is in bounds.
  if (n <= 0) return;
  if (from > bmargin) return;
  if (from + n > bmargin) n = bmargin - from;
  moveImage(loc(0,from+n),loc(0,from),loc(columns-1,bmargin-n));
  clearImage(loc(0,from),loc(columns-1,from+n-1),' ');
}

/*! position the cursor to a specific line and column. */
void Screen::setCursorYX(int y, int x)
{
  setCursorY(y); setCursorX(x);
}

/*! Set the cursor to x-th line. */

void Screen::setCursorX(int x)
{
  if (x == 0) x = 1; // Default
  x -= 1; // Adjust
  cuX = std::max(0,std::min(columns-1, x));
}

/*! Set the cursor to y-th line. */

void Screen::setCursorY(int y)
{
  if (y == 0) y = 1; // Default
  y -= 1; // Adjust
  cuY = std::max(0,std::min(lines  -1, y + (getMode(Mode::Origin) ? tmargin : 0) ));
}

/*! set cursor to the `left upper' corner of the screen (1,1).
*/

void Screen::home()
{
  cuX = 0;
  cuY = 0;
}

/*! set cursor to the begin of the current line.
*/

void Screen::Return()
{
  cuX = 0;
}

/*! returns the current cursor columns.
*/

int Screen::getCursorX()
{
  return cuX;
}

/*! returns the current cursor line.
*/

int Screen::getCursorY()
{
  return cuY;
}

// Erasing ---------------------------------------------------------------------

/*! \section Erasing

    This group of operations erase parts of the screen contents by filling
    it with spaces colored due to the current rendition settings.

    Althought the cursor position is involved in most of these operations,
    it is never modified by them.
*/

/*! fill screen between (including) `loca' and `loce' with spaces.

    This is an internal helper functions. The parameter types are internal
    addresses of within the screen image and make use of the way how the
    screen matrix is mapped to the image vector.
*/

void Screen::clearImage(int loca, int loce, char c)
{ int i;
  int scr_TL=loc(0,hist.getLines());
  //FIXME: check positions

  //Clear entire selection if it overlaps region to be moved...
  if ( (sel_BR > (loca+scr_TL) )&&(sel_TL < (loce+scr_TL)) )
  {
    clearSelection();
  }
  for (i = loca; i <= loce; i++)
  {
    image[i].c = c;
    image[i].f = ef_fg; //DEFAULT_FORE_COLOR; //FIXME: xterm and linux/ansi
    image[i].b = ef_bg; //DEFAULT_BACK_COLOR; //       many have different
    image[i].r = ef_re; //DEFAULT_RENDITION;  //       ideas here.
  }
}

/*! move image between (including) `loca' and `loce' to 'dst'.

    This is an internal helper functions. The parameter types are internal
    addresses of within the screen image and make use of the way how the
    screen matrix is mapped to the image vector.
*/

void Screen::moveImage(int dst, int loca, int loce)
{
//FIXME: check positions
  if (loce < loca) {
    // kdDebug() << "WARNING!!! call to Screen:moveImage with loce < loca!" << endl;
    return;
  }
  std::copy(&image[loca],&image[loce-loca+1],&image[dst]);

//  memmove(&image[dst],&image[loca],(loce-loca+1)*sizeof(ca));
}

/*! clear from (including) current cursor position to end of screen.
*/

void Screen::clearToEndOfScreen()
{
  clearImage(loc(cuX,cuY),loc(columns-1,lines-1),' ');
}

/*! clear from begin of screen to (including) current cursor position.
*/

void Screen::clearToBeginOfScreen()
{
  clearImage(loc(0,0),loc(cuX,cuY),' ');
}

/*! clear the entire screen.
*/

void Screen::clearEntireScreen()
{
  clearImage(loc(0,0),loc(columns-1,lines-1),' ');
}

/*! fill screen with 'E'
    This is to aid screen alignment
*/

void Screen::helpAlign()
{
  clearImage(loc(0,0),loc(columns-1,lines-1),'E');
}

/*! clear from (including) current cursor position to end of current cursor line.
*/

void Screen::clearToEndOfLine()
{
  clearImage(loc(cuX,cuY),loc(columns-1,cuY),' ');
}

/*! clear from begin of current cursor line to (including) current cursor position.
*/

void Screen::clearToBeginOfLine()
{
  clearImage(loc(0,cuY),loc(cuX,cuY),' ');
}

/*! clears entire current cursor line
*/

void Screen::clearEntireLine()
{
  clearImage(loc(0,cuY),loc(columns-1,cuY),' ');
}

// Rendition ------------------------------------------------------------------

/*!
    set rendition mode
*/

void Screen::setRendition(Rendition re)
{
  cu_re |= re;
  effectiveRendition();
}

/*!
    reset rendition mode
*/

void Screen::resetRendition(Rendition re)
{
  cu_re &= ~re;
  effectiveRendition();
}

/*!
*/

void Screen::setDefaultRendition()
{
  setForeColorToDefault();
  setBackColorToDefault();
  cu_re   = Rendition::None;
  effectiveRendition();
}

/*!
*/

void Screen::setForeColor(int fgcolor)
{
  cu_fg = (fgcolor&7)+((fgcolor&8) ? 4+8 : 2);
  effectiveRendition();
}

/*!
*/

void Screen::setBackColor(int bgcolor)
{
  cu_bg = (bgcolor&7)+((bgcolor&8) ? 4+8 : 2);
  effectiveRendition();
}

/*!
*/

void Screen::setBackColorToDefault()
{
  cu_bg = 0;
  effectiveRendition();
}

/*!
*/

void Screen::setForeColorToDefault()
{
  cu_fg = 1;
  effectiveRendition();
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                            Marking & Selection                            */
/*                                                                           */
/* ------------------------------------------------------------------------- */

void Screen::clearSelection()
{
  sel_BR = -1;
  sel_TL = -1;
  sel_begin = -1;
}

void Screen::setSelBeginXY(const int x, const int y)
{
  sel_begin = loc(x,y+histCursor) ;
  sel_BR = sel_begin;
  sel_TL = sel_begin;
}

void Screen::setSelExtentXY(const int x, const int y)
{
  if (sel_begin == -1) return;
  int l =  loc(x,y + histCursor);

  if (l < sel_begin)
  {
    sel_TL = l;
    sel_BR = sel_begin;
  }
  else
  {
    /* FIXME, HACK to correct for x too far to the right... */
    if (( x == columns )|| (x == 0)) l--;

    sel_TL = sel_begin;
    sel_BR = l;
  }
}

std::string Screen::getSelText(const bool preserve_line_breaks)
{
  if (sel_begin == -1)
     return std::string(); // Selection got clear while selecting.

  int *m;			// buffer to fill.
  int s, d;			// source index, dest. index.
  int hist_BR = loc(0, hist.getLines());
  int hY = sel_TL / columns;
  int hX = sel_TL % columns;
  int eol;			// end of line

  s = sel_TL;			// tracks copy in source.

                // allocate buffer for maximum
                // possible size...
  d = (sel_BR - sel_TL) / columns + 1;
  m = new int[d * (columns + 1) + 2];
  d = 0;

  while (s <= sel_BR)
    {
      if (s < hist_BR)
    {			// get lines from hist.history
                // buffer.
      eol = hist.getLineLen(hY);

      if ((hY == (sel_BR / columns)) &&
          (eol >= (sel_BR % columns)))
        {
          eol = sel_BR % columns + 1;
        }

      while (hX < eol)
        {
          m[d++] = hist.getCell(hY, hX++).c;
          s++;
        }

      if (s <= sel_BR)
        {
                // The line break handling
                // It's different from the screen
                // image case!
          if (eol % columns == 0)
        {
                // That's either a completely filled
                // line or an empty line
          if (eol == 0)
            {
              m[d++] = '\n';
            }
          else
            {
                // We have a full line.
                // FIXME: How can we handle newlines
                // at this position?!
            }
        }
          else if ((eol + 1) % columns == 0)
        {
                // FIXME: We don't know if this was a
                // space at the last position or a
                // short line!!
          m[d++] = ' ';
        }
          else
        {
                // We have a short line here. Put a
                // newline or a space into the
                // buffer.
          m[d++] = preserve_line_breaks ? '\n' : ' ';
        }
        }

      hY++;
      hX = 0;
      s = hY * columns;
    }
    else
      {				// or from screen image.
    eol = (s / columns + 1) * columns - 1;

    if (eol < sel_BR)
      {
        while ((eol > s) &&
           isspace(image[eol - hist_BR].c))
          {
        eol--;
          }
      }
    else
      {
        eol = sel_BR;
      }

    while (s <= eol)
      {
        m[d++] = image[s++ - hist_BR].c;
      }

    if (eol < sel_BR)
      {
                // eol processing see below ...
        if ((eol + 1) % columns == 0)
          {
        if (image[eol - hist_BR].c == ' ')
          {
            m[d++] = ' ';
          }
          }
        else
          {
        m[d++] = ((preserve_line_breaks ||
               ((eol % columns) == 0)) ?
              '\n' : ' ');
          }
      }

    s = (eol / columns + 1) * columns;
      }
    }

    std::string res;
  for (int i = 0; i < d; i++)
    {
      res.push_back((m[i]));
    }


  delete m;


  return res;
}
std::string Screen::getHistory() {
    sel_begin = 0;
    sel_BR = sel_begin;
    sel_TL = sel_begin;
    setSelExtentXY(columns-1,lines-1);
    std::string tmp= getSelText(true);
   // while (tmp.at(tmp.length()-2).unicode()==10 && tmp.at(tmp.length()-1).unicode()==10)
   //     tmp.truncate(tmp.length()-1);

    return tmp;
}
/* above ... end of line processing for selection -- psilva
cases:

1)    (eol+1)%columns == 0 --> the whole line is filled.
   If the last char is a space, insert (preserve) space. otherwise
   leave the text alone, so that words that are broken by linewrap
   are preserved.

FIXME:
    * this suppresses \n for command output that is
      sized to the exact column width of the screen.

2)    eol%columns == 0     --> blank line.
   insert a \n unconditionally.
   Do it either you would because you are in preserve_line_break mode,
   or because it's an ASCII paragraph delimiter, so even when
   not preserving line_breaks, you want to preserve paragraph breaks.

3)    else		 --> partially filled line
   insert a \n in preserve line break mode, else a space
   The space prevents concatenation of the last word of one
   line with the first of the next.

*/

void Screen::addHistLine()
{
  assert(hasScroll() || histCursor == 0);

  // add to hist buffer
  // we have to take care about scrolling, too...

  if (hasScroll())
  { CharAttributes dft;

    int end = columns-1;
    while (end >= 0 && image[end] == dft)
      end -= 1;

   // hist.addCells(image,end+1);
    hist.addLine();

    // adjust history cursor
    histCursor += (hist.getLines()-1 == histCursor);
  }

  if (!hasScroll()) histCursor = 0; //FIXME: a poor workaround
}

void Screen::setHistCursor(int cursor)
{
  histCursor = cursor; //FIXME:rangecheck
}

int Screen::getHistCursor()
{
  return histCursor;
}

int Screen::getHistLines()
{
  return hist.getLines();
}

void Screen::setScroll(bool on)
{
  histCursor = 0;
  clearSelection();
  hist.setScroll(on);
}

bool Screen::hasScroll()
{
  return hist.hasScroll();
}




}


