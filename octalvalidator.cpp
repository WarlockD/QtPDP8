#include "octalvalidator.h"
const QChar d8('8') ;
const QChar d9('9') ;

OctalValidator::OctalValidator(QObject *parent) : QValidator(parent)
{

}

 QValidator::State OctalValidator::validate(QString & input, int & pos) const {
     if (input.isEmpty()) {
       //  input = "0000";
         return State::Acceptable;
     }


     for(int i=0;i<input.size();i++) {
         const QChar c = input.at(i);
         if(!c.isDigit() || c == d8 || c== d9) return State::Invalid;
     }
     return State::Acceptable;
 }
