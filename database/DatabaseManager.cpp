#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    /*
     * Ici, si vous vous demandez pourquoi, c'est pour avoir une
     * instance globale dans tout l'app et de ne pas avoir e cnstructeur.
     * voici ce qui va se passer : le constructeur par defaut de mon
     * DatabaseManager sera appele pour iniialiser l'instance,
     * static empeiche de recreer l'instance si c'etait deja fait.
     * En ce qui concerne les attributs de la classe comme db par exemple,
     * leurs constructeurs par defaut sera apppele. d'ailleur,
     * je ne pourai pas declarer de constructeur = default pour DatabaseManager
     * si l'un de ses attributs n'avait pas de constructeur par defaut.
     * Voila qui est dit :D :D.
    */
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        QString connectionName = db.connectionName();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }
}

bool DatabaseManager::initDatabase() {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // Il faut noter qu'ici, le nom de na connection n'est pas fourni
    // donc ce sera la connection par defaut. Ceci me permet d'utiliser les
    // queries sans specifier la db. On se permet de faire ca car on n'aura
    // q'une seule db.
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dataDir + "/morse_history.db");

    if (!db.open()) {
        qCritical() << "Erreur ouverture SQLite:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createTable = R"(
        CREATE TABLE IF NOT EXISTS history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            direction TEXT NOT NULL,
            source_text TEXT NOT NULL,
            result_text TEXT NOT NULL,
            created_at TEXT NOT NULL
        )
    )";

    if (!query.exec(createTable)) {
        qCritical() << "Erreur création table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::addRecord(const QString &direction, const QString &source, const QString &result) {
    if (source.trimmed().isEmpty() || result.trimmed().isEmpty()) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO history (direction, source_text, result_text, created_at) "
                  "VALUES (:dir, :src, :res, :date)");
    query.bindValue(":dir", direction);
    query.bindValue(":src", source);
    query.bindValue(":res", result);
    query.bindValue(":date", QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));

    return query.exec();
}

QList<HistoryRecord> DatabaseManager::getAllRecords() const {
    QList<HistoryRecord> records;
    QSqlQuery query("SELECT id, direction, source_text, result_text, created_at FROM history ORDER BY id DESC");

    while (query.next()) {
        HistoryRecord rec;
        rec.id = query.value(0).toInt();
        rec.direction = query.value(1).toString();
        rec.sourceText = query.value(2).toString();
        rec.resultText = query.value(3).toString();
        rec.timestamp = query.value(4).toString();
        records.append(rec);
    }
    return records;
}

bool DatabaseManager::clearHistory() {
    QSqlQuery query;
    return query.exec("DELETE FROM history");
}

bool DatabaseManager::deleteRecord(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM history WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}
