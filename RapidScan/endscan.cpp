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
	bool globalOptimize = ui->checkBox_globaloptimize->isChecked();
	QString pointDist = ui->lineEdit_globalDist->text();
	bool rebuildData = ui->checkBox__rebuildData->isChecked();

	QJsonObject jsonobject;
	jsonobject.insert("globalOptimize", globalOptimize);
	jsonobject.insert("pointDist", pointDist.toDouble());
	jsonobject.insert("rebuildData", rebuildData);
	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray result = document.toJson();

	emit endScanSignal(result);
	this->hide();
}
