#include "server_box.h"

#include <QTcpServer>
#include <QTcpSocket>

ChatServer::ChatServer(QObject *parent) : QTcpServer(parent)
{}


/*void ChatServer::work(QString &command)
{
	if(command == QString("reset"))
		qDebug() << "Done";
}*/

void ChatServer::incomingConnection(int socketPort)
{
	QTcpSocket *client = new QTcpSocket;
	client->setSocketDescriptor(socketPort);

	qDebug() << "NEW CONNECTION --> " << client->peerAddress().toString();

	connect(client, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
	connect(client, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
}

void ChatServer::slotReadyRead()
{
	QTcpSocket *client = (QTcpSocket *)sender();
	while(client->canReadLine()) {
		QString line = QString::fromUtf8(client->readLine()).trimmed();

		QString type = line.section("/", 0, 0);

		if(type == QString("IN")) {
			QString user = line.section("/", 1, 1);

			bool stop = false;
			foreach(QString inside, users)
				if(inside == user) {
					client->write(QString("NO/\n").toUtf8());
					qDebug() << "Failure registration " << client->peerAddress().toString();
					stop = true;
					break;
				}
			if(stop)
				continue;

			client->write(QString("YES/\n").toUtf8());

			users.insert(client, user);
			clients.insert(client);
			foreach (QTcpSocket *all_clients, clients)
				all_clients->write(QString("NEW/" + user + "\n").toUtf8());

			slotSendUserList(client);
		}
		else if(type == QString("AllMes")) {

			QString user = users[client];
			QString message = line.section("/", 1, 1);
			qDebug() << user<< ": "<< message;

			foreach (QTcpSocket *all_clients, clients)
				all_clients->write(QString("AllMes/" + user + "/" + message + "\n").toUtf8());

			//saving
			QString from_BD_user = user + users.key(user)->peerAddress().toString();
			//save(from_BD_user, QString("ALL"), message);
		}
		else if(type == QString("PrMes")) {
			QString from_user = users[client];
			QString to_user = line.section("/", 1, 1);
			QTcpSocket *to_client = users.key(to_user);				//find socket by nick

			QString message = line.section("/", 2, 2);

			qDebug() << "(Private) "<<from_user << " -> " << to_user << ": " << message;
			to_client->write(QString("PrMes/" + from_user + "/" + message + "\n").toUtf8());

			//saving
			QString to_BD_user = to_user + users.key(to_user)->peerAddress().toString(),
					from_BD_user = from_user + users.key(from_user)->peerAddress().toString();
			//save(from_BD_user, to_BD_user, message);
		}
		else if(type == QString("UPD")) {
			QString from_user = users[client];
			QString type2 = line.section("/", 1, 1);


			//downloading
			if(type2 == "AllMes") {
				QString from_BD_user = from_user + users.key(from_user)->peerAddress().toString();
				//download(from_BD_user, QString("ALL"));
			}
			else if(type == "PrMes"){
				QString to_user = line.section("/", 2, 2);

				QString to_BD_user = to_user + users.key(to_user)->peerAddress().toString(),
						from_BD_user = from_user + users.key(from_user)->peerAddress().toString();
				//download(from_BD_user, to_BD_user);
			}

			client->write(QString("UPD/\n").toUtf8());
		}
		else
			qWarning() << "Bad message from " << client->peerAddress().toString();


	}
}

void ChatServer::slotSendUserList(QTcpSocket *client)
{
	QStringList userlist;
	foreach(QString user, users.values())
		userlist << user;

	//foreach (QTcpSocket *client, clients)
	client->write(QString("LIST/" + userlist.join("/") + "\n").toUtf8());
}

void ChatServer::slotDisconnected()
{
	QTcpSocket *client = (QTcpSocket *)sender();
	qDebug() << "DISCONNECTED --> " << client->peerAddress().toString();

	clients.remove(client);
	QString user = users[client];
	users.remove(client);

	foreach (QTcpSocket *all_clients, clients)
		all_clients->write(QString("OUT/" + user + "\n").toUtf8());
}
