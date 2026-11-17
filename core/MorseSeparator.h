#ifndef MORSESEPARATOR_H
#define MORSESEPARATOR_H

#include <QString>
#include <QRegularExpression>
#include <QList>
#include <QMetaType>

class MorseSeparator
{
public:
    MorseSeparator() = default;

    QString label() const { return _label; }
    QString value() const { return _value; }
    QRegularExpression regex() const { return _regex; }

    bool operator==(const MorseSeparator &other) const {
        return _value == other._value;
    }
    bool operator!=(const MorseSeparator &other) const {
        return !(*this == other);
    }

    static const MorseSeparator Undefined;
    static const MorseSeparator Tabulation;
    static const MorseSeparator NewLine;
    static const MorseSeparator Slash;
    static const MorseSeparator Pipe;

    static QList<const MorseSeparator*> all();

private:
    // Constructeur privé : empêche de créer des séparateurs invalides ailleurs
    MorseSeparator(QString label, QString value, QString regexPattern)
        : _label(std::move(label)),
          _value(std::move(value)),
          _regex(regexPattern) {}

    QString _label;
    QString _value;
    QRegularExpression _regex;
};

Q_DECLARE_METATYPE(const MorseSeparator*)

#endif // MORSESEPARATOR_H
