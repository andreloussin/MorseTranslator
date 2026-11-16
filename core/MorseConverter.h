#ifndef MORSECONVERTER_H
#define MORSECONVERTER_H

#include <QString>
#include <QMap>

class MorseConverter
{
public:
    MorseConverter();
    QString textToMorse(const QString &text) const;
    QString morseToText(const QString &morse) const;

private:
    QMap<QChar, QString> mapCharToMorse;
    QMap<QString, QChar> mapMorseToChar;

private:
    QString normalizeFrench(const QString &input) const;
};

#endif // MORSECONVERTER_H
