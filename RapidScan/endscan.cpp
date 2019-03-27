#include "endscan.h"
#include "ui_endscan.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
endScan::endScan(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::endScan)
{
    ui->setupUi(this);
}

endScan::~endScan()
{
    delete ui;
}

void endScan::on_pushButton_endScan_clicked()
{
	QString globalOptimize = ui->comboBox_globalOptimize->currentText();
	QString pointDist = ui->lineEdit_globalDist->text();
	QString rebuildData = ui->comboBox_3->currentText();

	QJsonObject jsonobject;
	jsonobject.insert("globalOptimize", typeBool(globalOptimize));
	jsonobject.insert("pointDist", pointDist.toDouble());
	jsonobject.insert("rebuildData", typeBool(rebuildData));
	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray result = document.toJson();

	emit endScanSignal(result);
	this->hide();
}

bool endScan::typeBool(QString type)
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
