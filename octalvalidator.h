#ifndef OCTALVALIDATOR_H
#define OCTALVALIDATOR_H

#include <QObject>
#include <QValidator>

class OctalValidator : public QValidator
{
    Q_OBJECT
public:
    explicit OctalValidator(QObject *parent = 0);
    State validate(QString & input, int & pos) const override;
};

#endif // OCTALVALIDATOR_H
