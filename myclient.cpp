#include "myclient.h"

MyClient::MyClient(const QString& host, int port, QWidget *parent)
    : QWidget(parent), m_nextBlockSize{0}
{
    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->connectToHost(host, port);
    connect(m_tcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));
    m_info = new QTextEdit;
    m_input = new QLineEdit;
    m_info->setReadOnly(true);
    QPushButton* sendButton = new QPushButton("&Send");
    connect(sendButton, SIGNAL(clicked()), SLOT(slotSendToServer()));
    connect(m_input, SIGNAL(returnPressed()), this, SLOT(slotSendToServer()));

    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(new QLabel("<H1>Client</H1>"));
    vLayout->addWidget(m_info);
    vLayout->addWidget(m_input);
    vLayout->addWidget(sendButton);
    setLayout(vLayout);

}

void MyClient::slotReadyRead() {
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_5_15);
    while(true) {
        if (!m_nextBlockSize) {
            if (m_tcpSocket->bytesAvailable() < sizeof(quint16))
                break;
            in >> m_nextBlockSize;
        }
        if (m_tcpSocket->bytesAvailable() < m_nextBlockSize)
            break;

        QTime time;
        QString str;
        in >> time >> str;

        m_info->append(time.toString() + " " + str);
        m_nextBlockSize = 0;
    }
}

void MyClient::slotError(QAbstractSocket::SocketError err) {
    QString errorText = "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                     "The host was not found." :
                                     err == QAbstractSocket::RemoteHostClosedError ?
                                     "The remote host is closed." :
                                     err == QAbstractSocket::ConnectionRefusedError ?
                                     "The connection was refused." :
                                     QString(m_tcpSocket->errorString()));
    m_info->append(errorText);
}

void MyClient::slotSendToServer() {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << QTime::currentTime() << m_input->text();

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    m_tcpSocket->write(block);
    m_input->setText("");
}

void MyClient::slotConnected() {
    m_info->append("Recieved the connected() signal");
}
