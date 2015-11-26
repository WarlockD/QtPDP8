#include "termina_vt52.h"
#define RETURN 015
#define LINEFEED 012
#define BACKSPACE 010
#define TAB 011
#define SPACE 040
#define DELETE 0177
#define ESC 033
#define XOFF 023
#define XON 021
namespace Terminal {
    VT52::VT52() : BasicTerminal(80,24), _state(State::Normal) {}

    // ESC K is sent on a ESC Z
    const char* VT52::putChar(char c) {
     c & 0x7F;
     switch(_state) {
     case State::Normal:
         if(c != ESC) return BasicTerminal::putChar(c);
          _state = State::InEscape;// escape code
          break;
     case State::InEscape:
         switch(c) {
            case 'A': if(_cursor.y() > 0) _cursor.moveUp(); break; // Cursor up
            case 'B': if(_cursor.y() <= (_height-1)) _cursor.moveDown(); break; // Cursor up
            case 'C': if(_cursor.x() <= (_width-1)) _cursor.moveRight(); break; // Cursor up
            case 'D': if(_cursor.x() > 0 ) _cursor.moveLeft(); break; // Cursor up
            case 'F': _graphicsMode = true; break; // enter graphics mode, not supported yet
            case 'G': _graphicsMode = false; break;// exit graphics mode
            case 'H': _cursor = Cursor(); break; // set home
            case 'L':  reverseLineFeed(); break; // reverse line feed, not sure if this is right?
            case 'J': clearScreen(_cursor.y(),_cursor.x()); break;
            case 'K': clearLine(_cursor.y(),_cursor.x());  break;
            case 'Y':
                _state = State::NeedY;
             return nullptr;
            case 'Z': // Ident
                _state = State::Normal; // have to change the state here since we return here
                return "\033K"; // return for VT52 without copier or printer
         case 0133: _holdScreen = true; break;// hold screen mode on, not supported right yet
         case 0134: _holdScreen = false; break;// hold screen mode off, not supported right yet
         case 075: _altKeypad = true; break; // alternate keypad mode on '='
         case 076: _altKeypad = false; break;// alternate keypad mode off '>'
         }
         _state = State::Normal;
         break;
     case State::NeedY:
         _posBuffer = c;
         _state = State::NeedX;
         break;
     case State::NeedX:
         setCursor(Cursor(c - 040,_posBuffer-040));
         _state = State::Normal;
         break;
     }
     return nullptr;
    }
}





