#include "widget.h"
#include <QColor>
#include <QPushButton>
#include <QRandomGenerator>
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->pbExit, &QPushButton::clicked, this, &Widget::close);

    sentences = {"Habt ihr was im Kaffee gehabt",
                 "Random Fact",
                 "Blätter einscannen",
                 "Wollt ihr mal vor die tür gehen",
                 "Das ist keine Akkordarbeit",
                 "Falsche namens aussprache 'Danaeee'",
                 "Rumgeben",
                 "Timo Richard Cameo",
                 "Drucken",
                 "Rassistische Bemerkung",
                 "Atmen",
                 "Frag ob wir x wissen obwohl wir nichts gemacht hatten",
                 "Free!",
                 "Böhmer geht in den Bastellraum",
                 "Studienfakten",
                 "Was habt ihr raus?",
                 "Wollt ihr mich etwa provozieren",
                 "Transistoren rumgeben",
                 "1 Blatt",
                 "2 Blätter",
                 "4 Blätter",
                 "6 Blätter",
                 "Hört es euch doch mit kopfhöhrern an",
                 "Dominik und Sergej brechen die schalwellen",
                 "Hohe Versprechen"};

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
    // Zufälliges Mischen der Sätze
    QRandomGenerator *gen = QRandomGenerator::global();
    QVector<int> indices = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                            13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    std::shuffle(indices.begin(), indices.end(), *gen);

    // Weise den Labels in den Buttons zufällig Texte zu
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            buttonLabels[buttons[i][j]]->setText(sentences[indices[i * 5 + j]]);
        }
    }
}

void Widget::onButtonClicked()
{
    // Holen des Buttons, der geklickt wurde
    QPushButton *button = qobject_cast<QPushButton *>(sender());

    if (button) {
        button->setStyleSheet("background-color: red;");

        // Bestimme die Position des Buttons in der Grid
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                if (buttons[i][j] == button) {
                    bingo[i][j] = true;
                    break;
                }
            }
        }

        checkBingo();
    }
}

void Widget::checkBingo()
{
    // Überprüfe alle Reihen und Spalten auf Bingo
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
}
