#include "save.h"
#include "ui_save.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QFileDialog>
save::save(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::save)
{
    ui->setupUi(this);
}

save::~save()
{
    delete ui;
}

void save::on_pushButton_clicked()
{
	QString path = ui->lineEdit_path->text();
	QString resizeRatio = ui->lineEdit_resizeRatio->text();
	QString p3 = ui->comboBox_p3->currentText();
	QString asc = ui->comboBox_asc->currentText();
	QString sasc = ui->comboBox_sasc->currentText();
	QString stl = ui->comboBox_stl->currentText();
	QString obj = ui->comboBox_obj->currentText();
	QString ply = ui->comboBox_ply->currentText();
	QString tmf = ui->comboBox_5->currentText();

	QJsonObject jsonobject;
	jsonobject.insert("path", path);
	jsonobject.insert("resizeRatio", resizeRatio.toDouble());
	jsonobject.insert("p3", typeBool(p3));
	jsonobject.insert("asc", typeBool(asc));
	jsonobject.insert("sasc", typeBool(sasc));
	jsonobject.insert("stl", typeBool(stl));
	jsonobject.insert("obj", typeBool(obj));
	jsonobject.insert("ply", typeBool(ply));
	jsonobject.insert("3mf", typeBool(tmf));
	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray result = document.toJson();
	emit saveSignal(result);
	this->hide();
}

void save::on_pushButton_Path_clicked()
{
	QString path = QFileDialog::getOpenFileName(this, "select a file");
	ui->lineEdit_path->setText(path);
}

bool save::typeBool(QString type)
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
