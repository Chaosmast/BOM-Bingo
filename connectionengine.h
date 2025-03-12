#ifndef CONNECTIONENGINE_H
#define CONNECTIONENGINE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QMap>
#include <QString>
#include <QTimer>

class ConnectionEngine : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionEngine(QObject *parent = nullptr);
    ~ConnectionEngine();

    void startServer();
    void connectToHost(const QString &hostAddress, quint16 port);
    void sendWordStatus(const QString &word, bool isActive);
    void startDiscovery();
    void stopDiscovery();

    void processNewServer();

signals:
    void wordStatusChanged(const QString &word, bool isActive);
    void connectedToHost();
    void newClientConnected();
    void hostsFound(const QList<QHostAddress> &hosts);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void onConnected();
    void processPendingDatagrams();
    void sendDiscoveryRequest();

private:
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
    QUdpSocket *udpSocket = nullptr;
    QList<QTcpSocket *> clientSockets;
    QMap<QTcpSocket *, QString> socketToClient;
    QTimer *discoveryTimer = nullptr;

    void processMessage(const QString &message);
    void sendDiscoveryResponse();
};

#endif // CONNECTIONENGINE_H
