#include "startscan.h"
#include "ui_startscan.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QDebug>
startScan::startScan(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::startScan)
{
    ui->setupUi(this);
	ui->lineEdit_turnableTimes->setEnabled(false);
	ui->checkBox_turntable->setChecked(false);
	connect(ui->checkBox_turntable, SIGNAL(stateChanged(int)), this, SLOT(lineEditTurnableTimes_slots(int)));
}

startScan::~startScan()
{
    delete ui;
}

void startScan::on_pushButton_ok_clicked()
{
	bool enableHDR = ui->checkBox->isChecked();
	QString alignType = ui->comboBox_alignType->currentText();
	//QString subScanType = ui->comboBox_subScanType->currentText();
	QString turntableTimes = ui->lineEdit_turnableTimes->text();

	QJsonObject paramsJsonObject;
	paramsJsonObject.insert("enableHDR", enableHDR);
	paramsJsonObject.insert("alignType", alignType);
	//paramsJsonObject.insert("subScanType", subScanType);
	paramsJsonObject.insert("turntableTimes", turntableTimes.toInt());

	QJsonObject jsonObject;
	jsonObject.insert("action", m_action);
	jsonObject.insert("params", paramsJsonObject);

	QJsonDocument document;
	document.setObject(jsonObject);
	//QByteArray result = document.toJson();
	result = document.toJson();
	emit startScanSignal(result);
	this->hide();
}


void startScan::setAction(QString action)
{
	m_action = action;
}

//void startScan::setSubType(int index)
//{
//	ui->comboBox_subScanType->setCurrentIndex(index-1);
//	ui->comboBox_subScanType->setEnabled(false);
//}

void startScan::lineEditTurnableTimes_slots(int state)
{
	if (ui->checkBox_turntable->isChecked() == true)
	{
		ui->lineEdit_turnableTimes->setEnabled(true);
		qDebug() << "true";
	}
	else if (ui->checkBox_turntable->isChecked() == false)
	{
		ui->lineEdit_turnableTimes->setEnabled(false);
		qDebug() << "false";
	}
}
