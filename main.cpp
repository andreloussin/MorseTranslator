#include "ui/mainwindow.h"
#include "database/DatabaseManager.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Morse Translator");

    if (!DatabaseManager::instance().initDatabase()) {
        QMessageBox::critical(nullptr, "Erreur Critique", "Impossible d'initialiser la base de données SQLite.");
        return -1;
    }

    MainWindow w;
    w.show();

    return app.exec();
}
