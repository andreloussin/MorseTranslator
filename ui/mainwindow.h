#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QTableWidget>

#include "../core/MorseConverter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onSourceTextChanged();
    void onSwitchDirection();
    void onCopyResult();
    void onClearInput();
    void onSaveToHistory();
    void onClearHistory();
    void onHistoryItemDoubleClicked(int row, int /*column*/);

private:
    void setupUi();
    void updateDirectionLabels();
    void updateSeparatorComboPosition();
    void loadHistoryData();
    void applyStyles();

    MorseConverter converter;
    TranslationDirection direction = TranslationDirection::FrenchToMorse;

    // Composants UI
    QLabel* leftLangLabel = nullptr;
    QLabel* rightLangLabel = nullptr;
    QPushButton* switchBtn = nullptr;

    QTextEdit* sourceEdit = nullptr;
    QTextEdit* resultEdit = nullptr;

    QPushButton* clearBtn = nullptr;
    QPushButton* copyBtn = nullptr;
    QPushButton* saveBtn = nullptr;
    QPushButton* clearHistoryBtn = nullptr;

    //
    QHBoxLayout* leftFooter = nullptr;
    QHBoxLayout* rightFooter = nullptr;
    QComboBox* separatorCombo = nullptr;

    QTableWidget* historyTable = nullptr;

private:
    // --- Constantes de l'interface ---
    static const QString PLACEHOLDER_FR;
    static const QString PLACEHOLDER_MORSE;
    static const QString PLACEHOLDER_RESULT;
};
#endif // MAINWINDOW_H
