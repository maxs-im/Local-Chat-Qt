#include <QCoreApplication>
#include <server_box.h>

//#include <QTextStream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	ChatServer *server = new ChatServer();

	bool effort = server->listen(QHostAddress::Any, 49100);
	if(!effort)
		qFatal("Non-working port");

	qDebug() << "\t\t\t\tLoaded";

	/*QTextStream inp(stdin);
	QString command;
	inp>> command;
	server->work(command);
	*/
    return a.exec();
}
