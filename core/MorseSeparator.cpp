#include "MorseSeparator.h"

const MorseSeparator MorseSeparator::Undefined(
        QStringLiteral("Aucun séparateur"),
        QStringLiteral("\t"), // Je prefere utiliser l tabulation par defaut
        QStringLiteral("(\\r?\\n+)|([ \\t]{2,}|\\t)|(\\s*/\\s*)|(\\s*\\|\\s*)")
        );

const MorseSeparator MorseSeparator::Tabulation(
        QStringLiteral("Tabulation"),
        QStringLiteral("\t"),
        QStringLiteral("[ \\t]{2,}|\\t")       // Capture >= 2 espaces/tabs OU une seule tabulation
        );

const MorseSeparator MorseSeparator::NewLine(
        QStringLiteral("Retour à la ligne"),
        QStringLiteral("\n"),
        QStringLiteral("\\r?\\n+")             // Capture \n (Linux/macOS) et \r\n (Windows), même multiples
        );

const MorseSeparator MorseSeparator::Slash(
        QStringLiteral("Slash ( / )"),
        QStringLiteral(" / "),
        QStringLiteral("\\s*/\\s*") // Tolère "/", " / ", " /", etc. à la saisie
        );

const MorseSeparator MorseSeparator::Pipe(
        QStringLiteral("Barre verticale ( | )"),
        QStringLiteral(" | "),
        QStringLiteral("\\s*\\|\\s*") // Tolère "|", " | ", etc.
        );

QList<const MorseSeparator*> MorseSeparator::all() {
    return QList<const MorseSeparator*>{ &Undefined, &Tabulation, &NewLine, &Slash, &Pipe };
}
