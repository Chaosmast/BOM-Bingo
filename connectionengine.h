#ifndef CONNECTIONENGINE_H
#define CONNECTIONENGINE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>

class ConnectionEngine : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionEngine(QObject *parent = nullptr);
    ~ConnectionEngine();

    void startServer();
    void connectToHost(const QString &hostAddress, quint16 port);
    void sendWordStatus(const QString &word, bool isActive);

signals:
    void wordStatusChanged(const QString &word, bool isActive);
    void connectedToHost();
    void newClientConnected();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void onConnected();

private:
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
    QList<QTcpSocket *> clientSockets;
    QMap<QTcpSocket *, QString> socketToClient;
    void processMessage(const QString &message);
};

#endif // CONNECTIONENGINE_H
