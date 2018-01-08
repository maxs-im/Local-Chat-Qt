#ifndef SERVER_BOX_H
#define SERVER_BOX_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>

class ChatServer : public QTcpServer
{
	Q_OBJECT

	public:
		ChatServer(QObject *parent = 0);

		//void work(QString &);

	private slots:
		void slotReadyRead();
		void slotSendUserList(QTcpSocket *);
		void slotDisconnected();

	protected:
		void incomingConnection(int);

	private:
		QSet<QTcpSocket *> clients;
		QMap<QTcpSocket *, QString> users;
};

#endif // SERVER_BOX_H
