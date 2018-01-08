#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	ui->Require_label->hide();
	ui->stackedWidget->setCurrentWidget(ui->Login_page);

	socket = new QTcpSocket(this);
	g_type = "AllMes/";

	connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
	connect(socket, SIGNAL(connected()), this, SLOT(slotConnected()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotConnected()
{
	socket->write(QString("IN/" + nickname + "\n").toUtf8());
}

void MainWindow::slotReadyRead()
{
	while(socket->canReadLine()) {
		QString line = QString::fromUtf8((socket->readLine())).trimmed();
		QString type = line.section("/", 0, 0);
		if(type == QString("NO")) {
			ui->Require_label->show();
		}
		else if(type == QString("YES")) {
			ui->stackedWidget->setCurrentWidget(ui->Message_page);
		}
		else if(type == QString("NEW")) {
			QString new_user = line.section("/", 1, 1);
			//ui->Users_listWidget->addItem(new_user);
			new QListWidgetItem(QPixmap(":/default-user-image.png"), new_user, ui->Users_listWidget);
			new_user = "Connected - " + new_user;
			ui->statusBar->showMessage(new_user, 5);
		}
		else if(type == QString("OUT")) {
			QString del_user = line.section("/", 1, 1);
			QList< QListWidgetItem* > items = ui->Users_listWidget->findItems(del_user, Qt::MatchFixedString);
			foreach( QListWidgetItem* item, items ) {
				if(item->text() == del_user){
					int row = ui->Users_listWidget->row( item );
					if(row == 0)		// if "COMMON"
						continue;
					ui->Users_listWidget->takeItem( row );
					delete item;
					break;
				}
			}
		}
		else if(type == QString("LIST")) {
			ui->Users_listWidget->clear();
			QStringList list = line.split("/");
			list.removeOne("LIST");
			ui->Users_listWidget->addItem("COMMON");
			//ui->Users_listWidget->addItems(list);
			foreach(QString label_list, list)
				new QListWidgetItem(QPixmap(":/default-user-image.png"), label_list, ui->Users_listWidget);
		}
		else if(type == QString("AllMes") ||
				type == QString("PrMes")) {
			QString user = line.section("/", 1, 1);
			QString message = line.section("/", 2, 2);
			if(user == nickname)
				user = "Me";

			if(type == g_type.section("/", 0, 0) ||
				(type == QString("PrMes") && user == g_type.section("/", 1, 1)))
				ui->Chat_textEdit->append("<b>" + user + "</b>: " + message);
		}
		else if(type == QString("UPD")) {
			ui->Chat_textEdit->clear();
		}
		else
			ui->statusBar->showMessage("Corrupted data!", 1);
	}
}

void MainWindow::on_Log_pushButton_clicked()
{
	nickname = ui->User_lineEdit_2->text();
	nickname.remove(QChar('/'));
	if(nickname.isEmpty())
		return;
	nickname = nickname;

	if(!socket->state())
		socket->connectToHost(ui->Server_lineEdit->text(), 49100);
	else
		slotConnected();
}

void MainWindow::on_Send_pushButton_2_clicked()
{
	QString message = ui->Message_lineEdit->text().trimmed();
	message.replace(QString("/"), QString("%"));
	if(!message.isEmpty()) {
		message = g_type + message;
		socket->write(QString(message + "\n").toUtf8());
	}
	ui->Message_lineEdit->clear();
	ui->Message_lineEdit->setFocus();
}

void MainWindow::on_Users_listWidget_itemClicked(QListWidgetItem *item)
{
	int row = ui->Users_listWidget->row( item );

	if(row == 0) g_type = "AllMes";
	else g_type = "PrMes/" + item->text();
	g_type = g_type + "/";

	socket->write(QString("UPD/" + g_type + "\n").toUtf8());
}
