#include "Server.h"

void Server::StartServer()
{
    allClients_ = new QVector<QTcpSocket*>;
    chatServer_ = new QTcpServer();
    chatServer_->setMaxPendingConnections(10);
    connect(chatServer_, SIGNAL(newConnection()), this, SLOT(NewClientConnection()));
    if (chatServer_->listen(QHostAddress::Any, 8001))
    {
        qDebug() << "Server has started. Listening to port 8001.";
    }
    else
    {
        qDebug() << "Server failed to start. Error: " + chatServer_->errorString();
    }
    nextBlockSize_ = 0;
}

void Server::SendMessageToClients(QString message)
{
    QByteArray  arrBlock;
    arrBlock.clear();
    QDataStream out(&arrBlock, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_6_5);
    out << quint16(0);
    out << message;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    for (int i = 0; i < allClients_->size(); i++)
    {
        if (allClients_->at(i)->isOpen() && allClients_->at(i)->isWritable()
            && allClients_->at(i) != static_cast<QTcpSocket*>(sender()))
        {
            allClients_->at(i)->write(arrBlock);
        }
    }
}

void Server::NewClientConnection()
{
    QTcpSocket* client = chatServer_->nextPendingConnection();
    QString ipAddress = client->peerAddress().toString();
    int port = client->peerPort();
    connect(client, &QTcpSocket::readyRead, this, &Server::SocketReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &Server::SocketDisconnected);
    allClients_->push_back(client);
    qDebug() << "Сокет подключен из " + ipAddress + " : " + QString::number(port);
}

void Server::SocketReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());
    QDataStream in(client);
    in.setVersion(QDataStream::Qt_6_5);
    while (true)
    {
        if (nextBlockSize_ == 0)
        {
            if (client->bytesAvailable() < (int)sizeof(quint16))
            {
                break;
            }
            in >> nextBlockSize_;
        }
        if (client->bytesAvailable() < nextBlockSize_)
        {
            break;
        }
        QString message;
        in >> message;
        qDebug() << message;
        SendMessageToClients(message);
        nextBlockSize_ = 0;
        break;
    }
}

void Server::SocketDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(QObject::sender());
    QString socketIpAddress = client->peerAddress().toString();
    int port = client->peerPort();
    qDebug() << "сокет отключился " + socketIpAddress + ":" + QString::number(port);
}


