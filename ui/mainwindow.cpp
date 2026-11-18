#include "mainwindow.h"
#include "core/MorseSeparator.h"
#include "database/DatabaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QHeaderView>
#include <QClipboard>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QTimer>

const QString MainWindow::PLACEHOLDER_FR = QStringLiteral("Écrivez votre texte en français (ex: Bonjour tout le monde)...");
const QString MainWindow::PLACEHOLDER_MORSE = QStringLiteral("Saisissez du Morse (séparez les lettres par un espace et les mots par ' / ')...");
const QString MainWindow::PLACEHOLDER_RESULT = QStringLiteral("La traduction s'affichera ici...");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    applyStyles();
    updateDirectionLabels();
    loadHistoryData();
}

void MainWindow::setupUi() {
    setWindowTitle("Morse Translator — Traducteur Français ↔ Morse");
    resize(1050, 720);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Premier Layout Vertical, contient le nom de l'app puis un vertical layout pour le conntenu qui aura un max width
    QVBoxLayout* baseLayout = new QVBoxLayout(centralWidget);
    baseLayout->setContentsMargins(28, 24, 28, 24);

    // On lui ajoute le titre
    QLabel *appTitle = new QLabel("Morse Translator", this);
    baseLayout->addWidget(appTitle);

    // Pour pouvoir centrer le contenu et mettre un max width, on ba utiliser un HBLay comme les flex-rows dans tailwind
    QHBoxLayout *wrapperLayout = new QHBoxLayout();
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    baseLayout->addLayout(wrapperLayout);

    // On ajoute mintenant le widget qui va contenir le contenu
    QWidget *contentContainer = new QWidget(centralWidget);
    contentContainer->setMaximumWidth(1140); // C'est ici qu'on fixe la taille a 1140px
    contentContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // On centre le conteneur dans le wrapper; le 1 (stretch) pour que le widget occupe le max de place possible
    wrapperLayout->addWidget(contentContainer, 1);


    // Maintenant le layout vertical aui va contenir le reste
    QVBoxLayout *contentLayout = new QVBoxLayout(contentContainer);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(20);
    contentLayout->setAlignment(Qt::AlignTop); // Sans ca, tout est aligne au milieu :(

    // La barre de langue plus bouton switch (Style Google Translate), HBLay
    QFrame *langBar = new QFrame(this); // Le frame, c'est pour avoir la possibilite de de faire des box avec border
    langBar->setObjectName("langBar");
    QHBoxLayout *langLayout = new QHBoxLayout(langBar);
    langLayout->setContentsMargins(16, 10, 16, 10);

    leftLangLabel = new QLabel(this);
    leftLangLabel->setAlignment(Qt::AlignCenter);

    switchBtn = new QPushButton("⇄", this);
    switchBtn->setObjectName("switchBtn");
    switchBtn->setToolTip("Inverser le sens de traduction");
    switchBtn->setFixedSize(44, 44);
    switchBtn->setCursor(Qt::PointingHandCursor);

    rightLangLabel = new QLabel(this);
    rightLangLabel->setAlignment(Qt::AlignCenter);

    langLayout->addWidget(leftLangLabel, 1);
    langLayout->addWidget(switchBtn, 0); // 0 pour stretch pour que le bouton ocupe la taille optimale ou minimale
    langLayout->addWidget(rightLangLabel, 1);

    contentLayout->addWidget(langBar);


    // Maintenant on attaque la zone de traduction, deux inputs cote a cote, le premier pour la saisie et le second pour la traduction.
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);

    // Carte Gauche (Entrée)
    QFrame *leftCard = new QFrame();
    leftCard->setObjectName("cardFrame");
    leftCard->setMaximumHeight(210);
    leftCard->setMinimumWidth(240);
    // il a aussi besoin d elayout car il y a des boutons en bas
    QVBoxLayout *leftCardLayout = new QVBoxLayout(leftCard);
    leftCardLayout->setContentsMargins(16, 16, 16, 12);

    sourceEdit = new QTextEdit();
    sourceEdit->setPlaceholderText(direction == TranslationDirection::FrenchToMorse ? PLACEHOLDER_FR : PLACEHOLDER_MORSE);

    leftFooter = new QHBoxLayout();
    clearBtn = new QPushButton("Effacer");
    clearBtn->setCursor(Qt::PointingHandCursor);
    leftFooter->addStretch();
    leftFooter->addWidget(clearBtn);

    leftCardLayout->addWidget(sourceEdit);
    leftCardLayout->addLayout(leftFooter);

    // Carte Droite (Résultat)
    QFrame *rightCard = new QFrame();
    rightCard->setObjectName("cardFrame");
    rightCard->setMaximumHeight(210);
    rightCard->setMinimumWidth(240);
    QVBoxLayout *rightCardLayout = new QVBoxLayout(rightCard);
    rightCardLayout->setContentsMargins(16, 16, 16, 12);

    resultEdit = new QTextEdit();
    resultEdit->setReadOnly(true);
    resultEdit->setPlaceholderText(PLACEHOLDER_RESULT);

    rightFooter = new QHBoxLayout();
    copyBtn = new QPushButton("Copier");
    copyBtn->setCursor(Qt::PointingHandCursor);

    saveBtn = new QPushButton("Sauvegarder dans l'historique", this);
    saveBtn->setObjectName("saveBtn");
    saveBtn->setCursor(Qt::PointingHandCursor);

    rightFooter->addStretch();
    rightFooter->addWidget(copyBtn);
    rightFooter->addWidget(saveBtn);

    rightCardLayout->addWidget(resultEdit);
    rightCardLayout->addLayout(rightFooter);

    cardsLayout->addWidget(leftCard, 1);
    cardsLayout->addWidget(rightCard, 1);
    contentLayout->addLayout(cardsLayout, 1);
    contentLayout->addStretch();


    // On initialise le select du separateur. on le mets a droite car cest la position de depart du morse
    separatorCombo = new QComboBox();
    separatorCombo->setCursor(Qt::PointingHandCursor);
    for (const MorseSeparator *sep : MorseSeparator::all()) {
        separatorCombo->addItem(sep->label(), QVariant::fromValue(sep));
    }
    rightFooter->insertWidget(0, separatorCombo);

    // On initialise la table de l'historique
    QFrame *historySection = new QFrame();
    historySection->setObjectName("cardFrame");
    QVBoxLayout *historyLayout = new QVBoxLayout(historySection);
    historyLayout->setContentsMargins(16, 14, 16, 14);
    historyLayout->setSpacing(10);

    QHBoxLayout *historyHeader = new QHBoxLayout();
    QLabel *historyTitle = new QLabel("Historique des traductions (double-cliquez pour restaurer)", this);

    clearHistoryBtn = new QPushButton("Vider l'historique");
    clearHistoryBtn->setCursor(Qt::PointingHandCursor);

    historyHeader->addWidget(historyTitle);
    historyHeader->addStretch();
    historyHeader->addWidget(clearHistoryBtn);

    historyTable = new QTableWidget(0, 4, this);
    historyTable->setHorizontalHeaderLabels({"Date", "Sens", "Entrée", "Résultat"});
    historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    historyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    historyTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    historyTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    historyTable->verticalHeader()->setVisible(false);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable->setAlternatingRowColors(false);

    historyLayout->addLayout(historyHeader);
    historyLayout->addWidget(historyTable);
    contentLayout->addWidget(historySection, 2);

    // Maintenant on connect tout
    connect(sourceEdit, &QTextEdit::textChanged, this, &MainWindow::onSourceTextChanged);
    connect(switchBtn, &QPushButton::clicked, this, &MainWindow::onSwitchDirection);
    connect(copyBtn, &QPushButton::clicked, this, &MainWindow::onCopyResult);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearInput);
    connect(separatorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSourceTextChanged);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveToHistory);
    connect(clearHistoryBtn, &QPushButton::clicked, this, &MainWindow::onClearHistory);
    connect(historyTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onHistoryItemDoubleClicked);
}

void MainWindow::updateDirectionLabels() {
    if (direction ==  TranslationDirection::FrenchToMorse) {
        leftLangLabel->setText("FRANÇAIS");
        rightLangLabel->setText("CODE MORSE");
        sourceEdit->setPlaceholderText(PLACEHOLDER_FR);
    } else {
        leftLangLabel->setText("CODE MORSE");
        rightLangLabel->setText("FRANÇAIS");
        sourceEdit->setPlaceholderText(PLACEHOLDER_MORSE);
    }

    updateSeparatorComboPosition();
}

void MainWindow::updateSeparatorComboPosition() {
    if (direction ==  TranslationDirection::FrenchToMorse) {
        rightFooter->insertWidget(0, separatorCombo);
    } else {
        leftFooter->insertWidget(0, separatorCombo);
    }
}

void MainWindow::onSourceTextChanged() {
    const MorseSeparator *separator = separatorCombo->currentData().value<const MorseSeparator*>();
    QString input = sourceEdit->toPlainText();

    QString translated = converter.translate(input, direction, separator);

    resultEdit->setPlainText(translated);
}

void MainWindow::onSwitchDirection() {
    direction = direction == TranslationDirection::FrenchToMorse ? TranslationDirection::MorseToFrench : TranslationDirection::FrenchToMorse;
    updateDirectionLabels();

    // Échange intelligent du contenu actuel
    QString currentResult = resultEdit->toPlainText();
    sourceEdit->setPlainText(currentResult); // Pas besoin de mettre le result manuellement, il aura un signal textChanged depuis sourceEdit
}

void MainWindow::onCopyResult() {
    QString res = resultEdit->toPlainText();
    if (!res.isEmpty()) {
        QApplication::clipboard()->setText(res);
        copyBtn->setText("Copié !");
        // Réinitialisation du texte du bouton après 1.5s; interessant non?
        QTimer::singleShot(1500, this, [this]() {
            copyBtn->setText("Copier");
        });
    }
}

void MainWindow::onClearInput() {
    sourceEdit->clear();
    resultEdit->clear();
}

// History things

void MainWindow::loadHistoryData() {
    historyTable->setRowCount(0);
    const auto records = DatabaseManager::instance().getAllRecords();

    for (const auto &rec : records) {
        int row =historyTable->rowCount();
        historyTable->insertRow(row);

        historyTable->setItem(row, 0, new QTableWidgetItem(rec.timestamp));
        historyTable->setItem(row, 1, new QTableWidgetItem(rec.direction));
        historyTable->setItem(row, 2, new QTableWidgetItem(rec.sourceText));
        historyTable->setItem(row, 3, new QTableWidgetItem(rec.resultText));
    }
}

void MainWindow::onSaveToHistory() {
    QString src = sourceEdit->toPlainText().trimmed();
    QString res = resultEdit->toPlainText().trimmed();
    if (src.isEmpty() || res.isEmpty()) return;

    QString dir =  MorseConverter::directionToString(direction);

    if (DatabaseManager::instance().addRecord(dir, src, res)) {
        loadHistoryData();
    }
}

void MainWindow::onClearHistory() {
    if (DatabaseManager::instance().clearHistory()) {
        loadHistoryData();
    }
}

void MainWindow::onHistoryItemDoubleClicked(int row, int /*column*/) {
    direction = MorseConverter::stringToDirection(historyTable->item(row, 1)->text());
    QString source = historyTable->item(row, 2)->text();

    updateDirectionLabels();
    sourceEdit->setPlainText(source);
}

void MainWindow::applyStyles() {
    QString qss = R"(
    /* --- Fond général de la fenêtre --- */
    QMainWindow, QWidget#centralWidget {
        background-color: #0b1329;
        font-family: 'Segoe UI', -apple-system, sans-serif;
        color: #f8fafc;
    }

    /* --- Titre principal --- */
    QLabel#appTitle {
        font-size: 24px;
        font-weight: 700;
        color: #38bdf8;
        letter-spacing: 0.5px;
        margin-bottom: 4px;
    }

    /* --- Barre de sélection des langues --- */
    QFrame#langBar {
        background-color: #162032;
        border: 1px solid #283548;
        border-radius: 12px;
    }
    QFrame#langBar QLabel {
        font-weight: 600;
        font-size: 13px;
        color: #94a3b8;
        border: none;
        background: transparent;
    }

    /* --- Bouton d'inversion central (Cercle parfait et stylisé) --- */
    QPushButton#switchBtn {background-color: #1e293b;
        border: 1px solid #38bdf8;
        border-radius: 22px;
        font-size: 22px;
        color: #38bdf8;
        padding: 0px;
    }
    QPushButton#switchBtn:hover {
        background-color: #38bdf8;
        color: #0b1329;
        border-color: #38bdf8;
    }

    /* --- Les Cartes --- */
    QFrame#cardFrame {
        background-color: #162032;
        border: 1px solid #283548;
        border-radius: 14px;
    }

    /* --- Zones de texte --- */
    QTextEdit {
        background-color: transparent;
        border: none;
        font-size: 14px;
        color: #f1f5f9;
        selection-background-color: #0284c7;
        selection-color: #ffffff;
    }

    /* --- Boutons secondaires --- */
    QPushButton {
        background-color: #22304a;
        border: 1px solid #334460;
        border-radius: 8px;
        padding: 6px 16px;
        font-size: 12px;
        font-weight: 600;
        color: #e2e8f0;
    }
    QPushButton:hover {
        background-color: #334460;
        border-color: #38bdf8;
        color: #ffffff;
    }
    QPushButton:pressed {
        background-color: #162032;
    }

    /* --- Bouton Principal : Sauvegarder --- */
    QPushButton#saveBtn {
        background-color: #0284c7;
        color: #ffffff;
        border: none;
        font-weight: 700;
    }
    QPushButton#saveBtn:hover {
        background-color: #0369a1;
    }
    QPushButton#saveBtn:pressed {
        background-color: #075985;
    }

    /* --- ComboBox et son menu déroulant --- */
    QComboBox {
        background-color: #0b1329;
        border: 1px solid #334460;
        border-radius: 8px;
        padding: 5px 12px;
        font-size: 12px;
        color: #e2e8f0;
    }
    QComboBox:hover {
        border-color: #38bdf8;
    }
    QComboBox::drop-down {
        subcontrol-origin: padding;
        subcontrol-position: top right;
        width: 24px;
        border-left: none;
    }
    QComboBox QAbstractItemView {
        background-color: #162032;
        border: 1px solid #334460;
        color: #f1f5f9;
        selection-background-color: #0284c7;
        selection-color: #ffffff;
        outline: none;
        padding: 4px;
    }

    /* --- Section Historique --- */
    QLabel#sectionTitle {
        font-size: 13px;
        font-weight: 600;
        color: #94a3b8;
        border: none;
        background: transparent;
    }

    /* --- Tableau sombre corrigé (Sélection et lignes propres) --- */
    QTableWidget {
        background-color: #0b1329;
        border: 1px solid #283548;
        border-radius: 10px;
        gridline-color: #1e293b;
        selection-background-color: #1e3a8a; /* Bleu sombre élégant pour la sélection */
        selection-color: #ffffff;
        outline: none;
    }
    QHeaderView::section {
        background-color: #162032;
        color: #94a3b8;
        padding: 10px;
        font-weight: 700;
        font-size: 11px;
        border: none;
        border-bottom: 1px solid #283548;
    }
    QTableWidget::item {
        padding: 8px;
        color: #cbd5e1;
        border-bottom: 1px solid #162032;
    }
    QTableWidget::item:selected {
        background-color: #0284c7; /* Surlignage net et moderne de la ligne active */
        color: #ffffff;
    }

    /* --- Barres de défilement (Scrollbars) modernes et sombres --- */
    QScrollBar:vertical {
        background: #0b1329;
        width: 10px;
        margin: 0px;
        border-radius: 5px;
    }
    QScrollBar::handle:vertical {
        background: #283548;
        min-height: 20px;
        border-radius: 5px;
    }
    QScrollBar::handle:vertical:hover {
        background: #38bdf8;
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0px;
    }
)";
    setStyleSheet(qss);
}

MainWindow::~MainWindow()
{}
