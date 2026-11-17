#include "MorseConverter.h"

#include <QRegularExpression>
#include <stdexcept>

MorseConverter::MorseConverter()
{
    // See: https://fr.wikipedia.org/wiki/Code_Morse_international
    mapCharToMorse = {
        // Lettres
        {'A', ".-"},    {'B', "-..."},  {'C', "-.-."},  {'D', "-.."},
        {'E', "."},     {'F', "..-."},  {'G', "--."},   {'H', "...."},
        {'I', ".."},    {'J', ".---"},  {'K', "-.-"},   {'L', ".-.."},
        {'M', "--"},    {'N', "-."},    {'O', "---"},   {'P', ".--."},
        {'Q', "--.-"},  {'R', ".-."},   {'S', "..."},   {'T', "-"},
        {'U', "..-"},   {'V', "...-"},  {'W', ".--"},   {'X', "-..-"},
        {'Y', "-.--"},  {'Z', "--.."},

        // Chiffres
        {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
        {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
        {'8', "---.."}, {'9', "----."},

        // Signes de ponctuation et symboles
        {'.', ".-.-.-"}, {',', "--..--"}, {'?', "..--.."}, {'\'', ".----."},
        {'!', "-.-.--"}, {'/', "-..-."},  {'(', "-.--."},  {')', "-.--.-"},
        {'&', ".-..."},  {':', "---..."}, {';', "-.-.-."}, {'=', "-...-"},
        {'+', ".-.-."},  {'-', "-....-"}, {'_', "..--.-"}, {'"', ".-..-."},
        {'$', "...-..-"},{'@', ".--.-."}

        // Extension aux caractères internationaux ou digrammes
        // Ignore car a la mention [réf. souhaitée] sur Wikipedia

        // Codes spéciaux
        // Ignore car specifique au service mobile maritime.
    };

    // Construction automatique de la table inverse
    // See: https://doc.qt.io/qt-6/fr/qmap-iterator.html
    for (auto it = mapCharToMorse.begin(), end = mapCharToMorse.end(); it != end; ++it){
        mapMorseToChar.insert(it.value(), it.key());
    }
}

QString MorseConverter::normalizeFrench(const QString &input) const {
    QString res = input.toUpper();
    res.replace(QRegularExpression("[ÀÂÄ]"), "A");
    res.replace(QRegularExpression("[ÉÈÊË]"), "E");
    res.replace(QRegularExpression("[ÎÏ]"), "I");
    res.replace(QRegularExpression("[ÔÖ]"), "O");
    res.replace(QRegularExpression("[ÙÛÜ]"), "U");
    res.replace("Ç", "C");
    res.replace("Œ", "OE");
    res.replace("Æ", "AE");
    return res;
}

QString MorseConverter::textToMorse(const QString &text, const MorseSeparator* separator) const {
    QStringList resultWords = {};

    // !! Rappel : Traailler avec le text normalise !!
    const QString normalized = normalizeFrench(text);

    // Extraction des mots. \s+ permet de cibler les expaces vides
    const QStringList words = normalized.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);

    for (const QString& word : words) {
        QStringList morseLetters = {};

        for (const QChar& ch : word) {
            if (mapCharToMorse.contains(ch)){
                morseLetters.append(mapCharToMorse.value(ch));
            } else {
                // TODO later : Decider s'il faut abandonner les carracteres non supportes
                morseLetters.append("?");
            }
        }


        // En morse, le separateur des caracteres est espace
        resultWords.append(morseLetters.join(" "));
    }

    // Il y a possibilite d'utiliser " / " pour separer les mots mais ici on va utiliser 7 espaces
    return resultWords.join(separator->value());
}

QString MorseConverter::morseToText(const QString &morse, const MorseSeparator* separator) const {
    QStringList resultWords = {};

    const QRegularExpression morseSeparatorRegex(separator->regex());
    const QStringList morseWords = morse.split(morseSeparatorRegex, QString::SkipEmptyParts);

    for (const QString &mWord : morseWords) {
        // Pas besoin de stocker un tableau de char
        QString decodedWord;

        // On avit enleve les separateurs de mot, maintenant il est temps de passer aux sep de char qui est toujours espace.
        const QStringList letters = mWord.trimmed().split(QRegularExpression("\\s+"), QString::SkipEmptyParts);

        for (const QString& code : letters) {
            if (mapMorseToChar.contains(code)) {
                decodedWord.append(mapMorseToChar.value(code));
            } else {
                // TODO later : ici aussi : Decider s'il faut abandonner les carracteres non supportes
                decodedWord.append('?');
            }
        }
        resultWords.append(decodedWord);
    }

    return resultWords.join(" ");
}

QString MorseConverter::translate(const QString& input, TranslationDirection direction, const MorseSeparator* separator) const {
    return (direction == TranslationDirection::FrenchToMorse)
            ? textToMorse(input, separator)
            : morseToText(input, separator);
}

QString MorseConverter::directionToString(TranslationDirection dir) {
    return (dir == TranslationDirection::FrenchToMorse) ? "FR → MORSE" : "MORSE → FR";
}

TranslationDirection MorseConverter::stringToDirection(const QString &str) {
    if (str == "MORSE → FR") {
        return TranslationDirection::MorseToFrench;
    }

    if (str == "FR → MORSE") {
        return TranslationDirection::FrenchToMorse;
    }

    throw std::invalid_argument("Direction de traduction inconnue : " + str.toStdString());
}
