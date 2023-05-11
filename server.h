#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QVector>

class Server : public QObject
{
    Q_OBJECT
public:

    void StartServer();
    void SendMessageToClients(QString message);

public slots:
    void NewClientConnection();
    void SocketDisconnected();
    void SocketReadyRead();
private:
    QTcpServer*             chatServer_;
    QVector<QTcpSocket*>*   allClients_;
    quint16                 nextBlockSize_;
};

#endif // SERVER_H
