#include "connectionengine.h"
#include <QNetworkDatagram>
#include <QDebug>

ConnectionEngine::ConnectionEngine(QObject *parent)
    : QObject{parent}
{
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &ConnectionEngine::processPendingDatagrams);
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
    if (!tcpServer->listen(QHostAddress::Any, 12345)) {
        qCritical() << "Server could not start!";
    } else {
        qDebug() << "Server started!";
    }

    if (!udpSocket->bind(45454, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qCritical() << "UDP socket could not bind!";
    } else {
        qDebug() << "UDP socket bound!";
    }
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
    qDebug() << "New client connected!";
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
    qDebug() << "Connected to host!";
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
    discoveryTimer = new QTimer(this);
    connect(discoveryTimer, &QTimer::timeout, this, &ConnectionEngine::sendDiscoveryRequest);
    discoveryTimer->start(3000); // Send discovery request every 3 seconds

    qDebug() << "Discovery started!";
}

void ConnectionEngine::stopDiscovery()
{
    if (discoveryTimer) {
        discoveryTimer->stop();
        delete discoveryTimer;
        discoveryTimer = nullptr;
    }
    qDebug() << "Discovery stopped!";
}

void ConnectionEngine::processPendingDatagrams()
{
    QList<QHostAddress> hosts;

    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        if (QString::fromUtf8(datagram.data()) == "BOM-Bingo Discovery Request") {
            sendDiscoveryResponse();
            hosts.append(datagram.senderAddress());
            qDebug() << "Discovery response received from:" << datagram.senderAddress().toString();
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
    qDebug() << "Discovery request sent!";
}

void ConnectionEngine::sendDiscoveryResponse()
{
    QByteArray data = "BOM-Bingo Discovery Response";
    udpSocket->writeDatagram(data, QHostAddress::Broadcast, 45454);
    qDebug() << "Discovery response sent!";
}
