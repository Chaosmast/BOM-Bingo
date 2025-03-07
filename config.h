#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QStringList>

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    QStringList getSentences() const;

signals:
};

#endif // CONFIG_H
