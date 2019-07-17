#include "smooth_sharp_zoom.h"
#include "ui_smooth_sharp_zoom.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

smooth_sharp_zoom::smooth_sharp_zoom(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::smooth_sharp_zoom)
{
    ui->setupUi(this);
	ui->lineEdit->setText("100");
	//qRegisterMetaType<smooth_sharp_zoom>();
}

smooth_sharp_zoom::~smooth_sharp_zoom()
{
    delete ui;
}

void smooth_sharp_zoom::onBtnHandle()
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
	}else if (!comText.compare("low"))
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
	emit smoothSharpZoomSignal(ret);
	this->hide();
}

// void smooth_sharp_zoom::on_SharpBtn_clicked()
// {
// 	onBtnHandle();
// }

void smooth_sharp_zoom::on_SmoothBtn_clicked()
{
	onBtnHandle();
}

// void smooth_sharp_zoom::on_ZoomBtn_clicked()
// {
// 	onBtnHandle(Zoom);
// }
