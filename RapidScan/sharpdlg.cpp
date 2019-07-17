#include "sharpdlg.h"
#include "ui_sharpdlg.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

sharpDlg::sharpDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sharpDlg)
{
    ui->setupUi(this);
	ui->lineEdit->setText("100");
}

sharpDlg::~sharpDlg()
{
    delete ui;
}

///////////////////////
void sharpDlg::onBtnHandle()
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
	emit sharpSignal(ret);
	this->hide();
}

void sharpDlg::on_SharpBtn_clicked()
{
	onBtnHandle();
}