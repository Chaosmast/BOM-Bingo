#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QStringList>
#include <QMap>

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

private:
    Ui::Widget *ui;

    void setupWordWrapForButtons();
    void setButtonTexts();
    void onButtonClicked();
    void checkBingo();

    QPushButton *buttons[5][5]; // 5x5 Grid für Buttons
    QMap<QPushButton*, QLabel*> buttonLabels; // Map für Labels in den Buttons
    QStringList sentences;
    bool bingo[5][5] = {false};
};
#endif // WIDGET_H
