#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
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

private:
    void setupUi();
    void updateDirectionLabels();
    void updateSeparatorComboPosition();

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

    //
    QHBoxLayout* leftFooter = nullptr;
    QHBoxLayout* rightFooter = nullptr;
    QComboBox* separatorCombo = nullptr;

private:
    // --- Constantes de l'interface ---
    static const QString PLACEHOLDER_FR;
    static const QString PLACEHOLDER_MORSE;
    static const QString PLACEHOLDER_RESULT;
};
#endif // MAINWINDOW_H
