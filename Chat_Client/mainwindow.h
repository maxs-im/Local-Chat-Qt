#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QListWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
	void slotReadyRead();
	void slotConnected();

	void on_Log_pushButton_clicked();

	void on_Send_pushButton_2_clicked();

	void on_Users_listWidget_itemClicked(QListWidgetItem *item);

	private:
    Ui::MainWindow *ui;

	QString g_type;
	QString nickname;
	QTcpSocket *socket;
};

#endif // MAINWINDOW_H
