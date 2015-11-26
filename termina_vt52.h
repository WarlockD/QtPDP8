#ifndef TERMINA_VT52_H
#define TERMINA_VT52_H


#include "terminal.h"

namespace Terminal {

// VT52 and its varrenents
class VT52 : public BasicTerminal {
    enum class State {
        Normal,
        InEscape,
        NeedX,
        NeedY,
    };
    State _state;
    unsigned char _posBuffer; // used to hold the two charater position
    bool _holdScreen;
    bool _altKeypad;
    bool _graphicsMode;
public:
    VT52();
    bool holdScreen() const { return _holdScreen; }
    bool altKeypad() const { return _altKeypad; }
    bool graphicsMode() const { return _altKeypad; }
    // ESC K is sent on a ESC Z
      virtual const char* putChar(char c) override;
};



}
#endif // TERMINA_VT52_H
