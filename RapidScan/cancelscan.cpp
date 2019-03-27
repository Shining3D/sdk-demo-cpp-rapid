#include "cancelscan.h"
#include "ui_cancelscan.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QJsonArray>
#include <QStringList>
#include <QDebug>
cancelScan::cancelScan(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cancelScan)
{
    ui->setupUi(this);
}

cancelScan::~cancelScan()
{
    delete ui;
}

void cancelScan::on_pushButton_cancelScan_clicked()
{
	QString dataNames = ui->lineEdit_dataNames->text();
	QJsonArray array = QJsonArray::fromStringList(dataNames.split(","));
	
	QJsonObject jsonobject;
	jsonobject.insert("dataNames", array);
	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray result = document.toJson();

	emit cancelScanSignal(result);
	this->hide();
}




