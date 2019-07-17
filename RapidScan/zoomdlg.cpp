#include "zoomdlg.h"
#include "ui_zoomdlg.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

zoomDlg::zoomDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::zoomDlg)
{
    ui->setupUi(this);
	ui->lineEdit->setText("100");
}

zoomDlg::~zoomDlg()
{
    delete ui;
}

/////////////////////////
void zoomDlg::onBtnHandle()
{
	int level = -1, intScale = 0;
	double _scale = 0.0;
	QString comText = ui->comboBox->currentText();
	if (!comText.compare("high"))
	{
		level = 0;
	}
	else if (!comText.compare("mid")){
		level = 1;
	}
	else if (!comText.compare("low"))
	{
		level = 2;
	}

	intScale = ui->lineEdit->text().toInt();
	_scale = intScale/100.0;

	QJsonObject jsonobject;
	jsonobject.insert("level", level);
	jsonobject.insert("scale", _scale);
	//add button type 2019.4.22
	//jsonobject.insert("type", (int)type);

	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray ret = document.toJson();
	//emit smoothSharpZoomSignal(ret, type);
	emit zoomSignal(ret);
	this->hide();
}

void zoomDlg::on_ZoomBtn_clicked()
{
	onBtnHandle();
}
