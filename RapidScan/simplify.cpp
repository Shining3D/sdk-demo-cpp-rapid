#include "simplify.h"
#include "ui_simplify.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

Simplify::Simplify(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
	ui->widget->setHidden(true);
}

Simplify::~Simplify()
{
    delete ui;
}


void Simplify::on_pushButton_clicked()
{
	//note: ¼æÈÝscanService3.2°æ±¾
// 	bool needMeshSmooth = ui->checkBox->isChecked();
// 	bool needMeshSharp = ui->checkBox_2->isChecked();
// 	bool hightQualityExtend = ui->checkBox_3->isChecked();
// 	QString strfillHolePerimeter = ui->lineEdit->text();
// 	QString strmeshSampleEdit = ui->lineEdit_2->text();
// 	bool fillHoleMarker = ui->checkBox_4->isChecked();
// 	bool fillHolePlaint = ui->checkBox_5->isChecked();
// 
// 	QJsonObject jsonobject;
// 	jsonobject.insert("needMeshSmooth", needMeshSmooth);
// 	jsonobject.insert("needMeshSharp", needMeshSharp);
// 	jsonobject.insert("hightQualityExtend", hightQualityExtend);
// 	jsonobject.insert("fillHolePerimeter", strfillHolePerimeter.toDouble());
// 	jsonobject.insert("meshSampleEdit", strmeshSampleEdit.toDouble());
// 	jsonobject.insert("fillHoleMarker", fillHoleMarker);
// 	jsonobject.insert("fillHolePlaint", fillHolePlaint);


	int intScale = ui->lineEdit_3->text().toInt();
	double _scale = (intScale / 100.0) * 100;

	QJsonObject jsonobject;
	//jsonobject.insert("level", level);
	jsonobject.insert("simplifyScale", _scale);

	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray result = document.toJson();
	emit simplifySignal(result);
	this->hide();
}