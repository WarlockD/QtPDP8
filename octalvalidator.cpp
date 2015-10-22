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

     const QChar c = input.at(pos-1);
     if(c.isDigit() && c != d8 && c!= d9) return State::Acceptable;
     else return State::Invalid;
 }
