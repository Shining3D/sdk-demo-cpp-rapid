#include "startscan.h"
#include "ui_startscan.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
startScan::startScan(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::startScan)
{
    ui->setupUi(this);
	this->setWindowTitle("Start scan");
}

startScan::~startScan()
{
    delete ui;
}

void startScan::on_pushButton_clicked()
{
	QString enableHDR = ui->comboBox_enableHDR->currentText();
	QString alignType = ui->comboBox_alignType->currentText();
	QString subScanType = ui->comboBox_subScanType->currentText();
	QString turntableTimes = ui->lineEdit_turnableTimes->text();

	QJsonObject paramsJsonObject;
	paramsJsonObject.insert("enableHDR", typeBool(enableHDR));
	paramsJsonObject.insert("alignType", alignType);
	paramsJsonObject.insert("subScanType", subScanType);
	paramsJsonObject.insert("turntableTimes", turntableTimes.toInt());

	QJsonObject jsonObject;
	jsonObject.insert("action", m_action);
	jsonObject.insert("params", paramsJsonObject);

	QJsonDocument document;
	document.setObject(jsonObject);
	QByteArray result = document.toJson();

	emit startScanSignal(result);
	this->hide();
}

bool startScan::typeBool(QString type)
{

	if (type == "true")
	{
		return true;
	}
	else
	{
		return false;
	}
}

void startScan::setAction(QString action)
{
	m_action = action;
}

void startScan::setSubType(int index)
{
	ui->comboBox_subScanType->setCurrentIndex(index-1);
	ui->comboBox_subScanType->setEnabled(false);
}

