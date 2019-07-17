#ifndef STARTSCAN_H
#define STARTSCAN_H

#include <QDialog>
#include <QByteArray>
/*
An interface corresponding to a "Start" button
*/
namespace Ui {
class startScan;
}

class startScan : public QDialog
{
    Q_OBJECT

public:
	void setAction(QString action);//Set the specified key("action") value in json
	/*
	index:    the combobox index
	Set text information for combobox,
	*/
	//void setSubType(int index);//去掉扫描子类型
    explicit startScan(QWidget *parent = nullptr);
    ~startScan();
signals:
	void startScanSignal(QByteArray);//The mainwindow receive this signal

private slots:
    void on_pushButton_ok_clicked();
	void lineEditTurnableTimes_slots(int k); 
private:
    Ui::startScan *ui;
	QString m_action;
public:
	QByteArray result;
};

#endif // STARTSCAN_H
