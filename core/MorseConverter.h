#ifndef MORSECONVERTER_H
#define MORSECONVERTER_H

#include "MorseSeparator.h"

#include <QString>
#include <QMap>

enum class TranslationDirection {
    FrenchToMorse,
    MorseToFrench
};

class MorseConverter {
public:
    MorseConverter();

    QString translate(const QString& input, TranslationDirection direction, const MorseSeparator* separator = &MorseSeparator::Undefined) const;
    QString textToMorse(const QString& text, const MorseSeparator* separator = &MorseSeparator::Undefined) const;
    QString morseToText(const QString& morse, const MorseSeparator* separator = &MorseSeparator::Undefined) const;

    static QString directionToString(TranslationDirection dir);
    static TranslationDirection stringToDirection(const QString &str);

private:
    QMap<QChar, QString> mapCharToMorse;
    QMap<QString, QChar> mapMorseToChar;

    QString normalizeFrench(const QString& input) const;
};

#endif // MORSECONVERTER_H
