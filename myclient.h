#pragma once
#include <QtWidgets>
#include <QTcpSocket>

class MyClient : public QWidget {
Q_OBJECT

public:
    MyClient(const QString& host, int port, QWidget *parent = nullptr);

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();

private:
    QTcpSocket* m_tcpSocket;
    QTextEdit* m_info;
    QLineEdit* m_input;
    quint16 m_nextBlockSize;
};

