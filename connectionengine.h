#ifndef CONNECTIONENGINE_H
#define CONNECTIONENGINE_H

#include <QObject>

class ConnectionEngine : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionEngine(QObject *parent = nullptr);

signals:
};

#endif // CONNECTIONENGINE_H
