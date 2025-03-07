#ifndef WIDGET_H
#define WIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QStringList>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include "config.h"
#include "connectionengine.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onButtonClicked();
    void onScrollAreaButtonClicked();
    void onWordStatusChanged(const QString &word, bool isActive);
    void onHostButtonClicked();
    void onJoinButtonClicked();
    void onConnectedToHost();
    void onNewClientConnected();

private:
    Ui::Widget *ui;

    void setupWordWrapForButtons();
    void setButtonTexts();
    void populateScrollArea();
    void checkBingo();

    QPushButton *buttons[5][5];                 // 5x5 Grid für Buttons
    QMap<QPushButton *, QLabel *> buttonLabels; // Map für Labels in den Buttons
    QMap<QString, QPushButton *> scrollAreaButtons; // Map für Buttons in der ScrollArea
    QStringList sentences;
    bool bingo[5][5] = {false};
    Config config;
    ConnectionEngine *connectionEngine;
    bool isHost = false;
};

#endif // WIDGET_H
