#include "connectionengine.h"
#include <QNetworkDatagram>
#include <QTimer>

ConnectionEngine::ConnectionEngine(QObject *parent)
    : QObject{parent}
{
}

ConnectionEngine::~ConnectionEngine()
{
    if (tcpServer) {
        tcpServer->close();
        delete tcpServer;
    }
    if (tcpSocket) {
        tcpSocket->close();
        delete tcpSocket;
    }
    if (udpSocket) {
        udpSocket->close();
        delete udpSocket;
    }
    for (QTcpSocket *clientSocket : clientSockets) {
        clientSocket->close();
        delete clientSocket;
    }
}

void ConnectionEngine::startServer()
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &ConnectionEngine::onNewConnection);
    tcpServer->listen(QHostAddress::Any, 12345);
}

void ConnectionEngine::connectToHost(const QString &hostAddress, quint16 port)
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &ConnectionEngine::onConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ConnectionEngine::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &ConnectionEngine::onDisconnected);
    tcpSocket->connectToHost(hostAddress, port);
}

void ConnectionEngine::sendWordStatus(const QString &word, bool isActive)
{
    QString message = QString("#word:%1:%2")
                          .arg(isActive ? "new" : "remove")
                          .arg(word);
    if (tcpSocket && tcpSocket->state() == QAbstractSocket::ConnectedState) {
        tcpSocket->write(message.toUtf8());
    } else {
        for (QTcpSocket *clientSocket : clientSockets) {
            clientSocket->write(message.toUtf8());
        }
    }
}

void ConnectionEngine::onNewConnection()
{
    QTcpSocket *newClientSocket = tcpServer->nextPendingConnection();
    clientSockets.append(newClientSocket);
    connect(newClientSocket, &QTcpSocket::readyRead, this, &ConnectionEngine::onReadyRead);
    connect(newClientSocket, &QTcpSocket::disconnected, this, &ConnectionEngine::onDisconnected);
    emit newClientConnected();
}

void ConnectionEngine::onReadyRead()
{
    QTcpSocket *senderSocket = qobject_cast<QTcpSocket *>(sender());
    if (senderSocket) {
        QString message = QString::fromUtf8(senderSocket->readAll());
        processMessage(message);
    }
}

void ConnectionEngine::onDisconnected()
{
    QTcpSocket *senderSocket = qobject_cast<QTcpSocket *>(sender());
    if (senderSocket) {
        clientSockets.removeAll(senderSocket);
        senderSocket->deleteLater();
    }
}

void ConnectionEngine::onConnected()
{
    emit connectedToHost();
}

void ConnectionEngine::processMessage(const QString &message)
{
    if (message.startsWith("#word:")) {
        QStringList parts = message.split(':');
        if (parts.size() == 3) {
            QString action = parts[1];
            QString word = parts[2];
            bool isActive = (action == "new");
            emit wordStatusChanged(word, isActive);
        }
    }
}

void ConnectionEngine::startDiscovery()
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 45454, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket, &QUdpSocket::readyRead, this, &ConnectionEngine::processPendingDatagrams);

    sendDiscoveryRequest();
}

void ConnectionEngine::stopDiscovery()
{
    if (udpSocket) {
        udpSocket->close();
        delete udpSocket;
        udpSocket = nullptr;
    }
}

void ConnectionEngine::processPendingDatagrams()
{
    QList<QHostAddress> hosts;

    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        if (QString::fromUtf8(datagram.data()) == "BOM-Bingo Discovery Response") {
            hosts.append(datagram.senderAddress());
        }
    }

    if (!hosts.isEmpty()) {
        emit hostsFound(hosts);
    }
}

void ConnectionEngine::sendDiscoveryRequest()
{
    QByteArray data = "BOM-Bingo Discovery Request";
    udpSocket->writeDatagram(data, QHostAddress::Broadcast, 45454);
}

void ConnectionEngine::sendDiscoveryResponse()
{
    QByteArray data = "BOM-Bingo Discovery Response";
    udpSocket->writeDatagram(data, QHostAddress::Broadcast, 45454);
}
