#include "widget.h"
#include "ui_widget.h"
#include <QRandomGenerator>
#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , connectionEngine(new ConnectionEngine(this))
{
    ui->setupUi(this);

    connect(ui->pbExit, &QPushButton::clicked, this, &Widget::close);
    connect(ui->pbHost, &QPushButton::clicked, this, &Widget::onHostButtonClicked);
    connect(ui->pbJoin, &QPushButton::clicked, this, &Widget::onJoinButtonClicked);
    connect(connectionEngine, &ConnectionEngine::wordStatusChanged, this, &Widget::onWordStatusChanged);
    connect(connectionEngine, &ConnectionEngine::connectedToHost, this, &Widget::onConnectedToHost);
    connect(connectionEngine, &ConnectionEngine::newClientConnected, this, &Widget::onNewClientConnected);

    // Verwenden Sie die Methode aus config, um die Begriffe zu erhalten
    sentences = config.getSentences();

    buttons[0][0] = ui->pushButton_01;
    buttons[0][1] = ui->pushButton_02;
    buttons[0][2] = ui->pushButton_03;
    buttons[0][3] = ui->pushButton_04;
    buttons[0][4] = ui->pushButton_05;
    buttons[1][0] = ui->pushButton_06;
    buttons[1][1] = ui->pushButton_07;
    buttons[1][2] = ui->pushButton_08;
    buttons[1][3] = ui->pushButton_09;
    buttons[1][4] = ui->pushButton_10;
    buttons[2][0] = ui->pushButton_11;
    buttons[2][1] = ui->pushButton_12;
    buttons[2][2] = ui->pushButton_13;
    buttons[2][3] = ui->pushButton_14;
    buttons[2][4] = ui->pushButton_15;
    buttons[3][0] = ui->pushButton_16;
    buttons[3][1] = ui->pushButton_17;
    buttons[3][2] = ui->pushButton_18;
    buttons[3][3] = ui->pushButton_19;
    buttons[3][4] = ui->pushButton_20;
    buttons[4][0] = ui->pushButton_21;
    buttons[4][1] = ui->pushButton_22;
    buttons[4][2] = ui->pushButton_23;
    buttons[4][3] = ui->pushButton_24;
    buttons[4][4] = ui->pushButton_25;

    setupWordWrapForButtons();
    setButtonTexts();
    populateScrollArea();

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            connect(buttons[i][j], &QPushButton::clicked, this, &Widget::onButtonClicked);
        }
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setupWordWrapForButtons()
{
    // Füge jedem Button ein QLabel mit WordWrap hinzu
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            QPushButton *btn = buttons[i][j];
            QLabel *label = new QLabel("", btn);
            label->setWordWrap(true);
            label->setAlignment(Qt::AlignCenter);

            QHBoxLayout *layout = new QHBoxLayout(btn);
            layout->addWidget(label);
            layout->setContentsMargins(0, 0, 0, 0);

            buttonLabels[btn] = label;
        }
    }
}

void Widget::setButtonTexts()
{
    // Zufälliges Mischen der Sätze und Auswahl von 25 zufälligen Sätzen
    QRandomGenerator *gen = QRandomGenerator::global();
    std::shuffle(sentences.begin(), sentences.end(), *gen);
    QStringList randomSentences = sentences.mid(0, 25);

    // Weise den Labels in den Buttons zufällig Texte zu
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            buttonLabels[buttons[i][j]]->setText(randomSentences[i * 5 + j]);
        }
    }
}

void Widget::onButtonClicked()
{
    if (!isHost) return; // Only host can click buttons

    // Holen des Buttons, der geklickt wurde
    QPushButton *button = qobject_cast<QPushButton *>(sender());

    if (button) {
        QString text = buttonLabels[button]->text();
        bool wasMarked = button->styleSheet().contains("background-color: red;");

        if (wasMarked) {
            button->setStyleSheet("");  // Reset style
        } else {
            button->setStyleSheet("background-color: red;");
        }

        // Bestimme die Position des Buttons in der Grid
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                if (buttons[i][j] == button) {
                    bingo[i][j] = !wasMarked;  // Toggle state

                    // Markiere oder demarkiere den entsprechenden Button in der ScrollArea
                    if (scrollAreaButtons.contains(text)) {
                        scrollAreaButtons[text]->setStyleSheet(wasMarked ? "" : "background-color: red;");
                    }

                    if (isHost) {
                        connectionEngine->sendWordStatus(text, !wasMarked);
                    }

                    break;
                }
            }
        }

        checkBingo();
    }
}

void Widget::onScrollAreaButtonClicked()
{
    if (!isHost) return;

    // Holen des Buttons, der geklickt wurde
    QPushButton *button = qobject_cast<QPushButton *>(sender());

    if (button) {
        QString text = button->text();
        bool wasMarked = button->styleSheet().contains("background-color: red;");

        if (wasMarked) {
            button->setStyleSheet("");  // Reset style
        } else {
            button->setStyleSheet("background-color: red;");
        }

        // Markiere oder demarkiere den entsprechenden Button im Bingo
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                if (buttonLabels[buttons[i][j]]->text() == text) {
                    buttons[i][j]->setStyleSheet(wasMarked ? "" : "background-color: red;");
                    bingo[i][j] = !wasMarked;  // Toggle state
                    break;
                }
            }
        }

        connectionEngine->sendWordStatus(text, !wasMarked);
        checkBingo();
    }
}

void Widget::onWordStatusChanged(const QString &word, bool isActive)
{
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (buttonLabels[buttons[i][j]]->text() == word) {
                buttons[i][j]->setStyleSheet(isActive ? "background-color: red;" : "");
                bingo[i][j] = isActive;
            }
        }
    }

    if (scrollAreaButtons.contains(word)) {
        scrollAreaButtons[word]->setStyleSheet(isActive ? "background-color: red;" : "");
    }

    checkBingo();
}

void Widget::onHostButtonClicked()
{
    isHost = true;
    connectionEngine->startServer();
    ui->pbHost->setEnabled(false);
    ui->pbJoin->setEnabled(false);
}

void Widget::onJoinButtonClicked()
{
    isHost = false;

    bool ok;
    QString ipAddress = QInputDialog::getText(this, tr("Join Game"),
                                              tr("IP Address:"), QLineEdit::Normal,
                                              "127.0.0.1", &ok);
    if (ok && !ipAddress.isEmpty()) {
        quint16 port = QInputDialog::getInt(this, tr("Join Game"),
                                            tr("Port:"), 12345, 1, 65535, 1, &ok);
        if (ok) {
            connectionEngine->connectToHost(ipAddress, port);
        }
    }

    ui->pbHost->setEnabled(false);
    ui->pbJoin->setEnabled(false);
}

void Widget::onConnectedToHost()
{
    // Disable all buttons for the client
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            buttons[i][j]->setEnabled(false);
        }
    }
}

void Widget::onNewClientConnected()
{
    // Handle actions when a new client connects to the host
}

void Widget::checkBingo()
{
    // Überprüfe alle Reihen, Spalten und Diagonalen auf Bingo
    for (int i = 0; i < 5; ++i) {
        bool rowComplete = true;
        bool colComplete = true;

        for (int j = 0; j < 5; ++j) {
            if (!bingo[i][j])
                rowComplete = false;
            if (!bingo[j][i])
                colComplete = false;
        }

        if (rowComplete) {
            for (int j = 0; j < 5; ++j) {
                buttons[i][j]->setStyleSheet("background-color: yellow;");
            }
        }

        if (colComplete) {
            for (int j = 0; j < 5; ++j) {
                buttons[j][i]->setStyleSheet("background-color: yellow;");
            }
        }
    }

    // Überprüfe die beiden Diagonalen
    bool diag1Complete = true;
    bool diag2Complete = true;

    for (int i = 0; i < 5; ++i) {
        if (!bingo[i][i])
            diag1Complete = false;
        if (!bingo[i][4 - i])
            diag2Complete = false;
    }

    if (diag1Complete) {
        for (int i = 0; i < 5; ++i) {
            buttons[i][i]->setStyleSheet("background-color: yellow;");
        }
    }

    if (diag2Complete) {
        for (int i = 0; i < 5; ++i) {
            buttons[i][4 - i]->setStyleSheet("background-color: yellow;");
        }
    }
}

void Widget::populateScrollArea()
{
    QScrollArea *scrollArea = ui->scrollArea;
    QWidget *scrollAreaWidgetContents = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollAreaWidgetContents);

    QStringList allSentences = config.getSentences();

    for (const QString &sentence : allSentences) {
        QPushButton *button = new QPushButton(sentence);
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        button->setFixedWidth(340);  // Set the maximum width for the button
        button->setStyleSheet("text-align: left; padding: 5px;");  // Align text to left and add padding
        connect(button, &QPushButton::clicked, this, &Widget::onScrollAreaButtonClicked);
        layout->addWidget(button);
        scrollAreaButtons[sentence] = button;
    }

    scrollAreaWidgetContents->setLayout(layout);
    scrollArea->setWidget(scrollAreaWidgetContents);
}
