#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QList>

struct HistoryRecord {
    int id;
    QString direction;
    QString sourceText;
    QString resultText;
    QString timestamp;
};

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool initDatabase();
    bool addRecord(const QString &direction, const QString &source, const QString &result);
    QList<HistoryRecord> getAllRecords() const;
    bool clearHistory();
    bool deleteRecord(int id);

private:
    DatabaseManager() = default;
    ~DatabaseManager();
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
