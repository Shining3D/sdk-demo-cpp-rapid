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
	bool p3 = ui->checkBox_p3->isChecked();
	bool asc = ui->checkBox__Asc->isChecked();
	bool sasc = ui->checkBox_Sasc->isChecked();
	bool stl = ui->checkBox_Stl->isChecked();
	bool obj = ui->checkBox_Obj->isChecked();
	bool ply = ui->checkBox_Ply->isChecked();
	bool tmf = ui->checkBox__3mf->isChecked();

	QJsonObject jsonobject;
	jsonobject.insert("path", path);
	jsonobject.insert("resizeRatio", resizeRatio.toDouble());
	jsonobject.insert("p3", p3);
	jsonobject.insert("asc", asc);
	jsonobject.insert("sasc", sasc);
	jsonobject.insert("stl", stl);
	jsonobject.insert("obj", obj);
	jsonobject.insert("ply", ply);
	jsonobject.insert("3mf", tmf);
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

