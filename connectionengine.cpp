#include "connectionengine.h"

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
