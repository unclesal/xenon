#include <QUdpSocket>
#include <QByteArray>
#include <QDebug>

#include "Settings.h"

int main(int argc, char * argv[] ) {
    QUdpSocket * socket = new QUdpSocket();
    QByteArray dg = "Message";
    QHostAddress address = QHostAddress(NETWORK_MULTICAST_GROUP);
    qint64 res = socket->writeDatagram(dg.data(), dg.size(), address, NETWORK_PORT );
    qDebug() << "Sended, res=" << res;
    return 0;
}
