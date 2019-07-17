#include "fillhole.h"
#include "ui_fillhole.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QStringList>

fillHole::fillHole(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fillHole)
{
    ui->setupUi(this);
	ui->lineEdit_4->setText(QStringLiteral("80"));//ÉèÖÃÄ¬ÈÏÖµ£¬±ÜÃâ¿Í»§·¸´í;

}

fillHole::~fillHole()
{
    delete ui;
}

QLineEdit* fillHole::getEditLine()
{
	return ui->lineEdit_3;
}

QLineEdit* fillHole::getApplyLine()
{
	return ui->lineEdit;
}

QPushButton* fillHole::getEditBtn()
{
	return ui->EditFillHoleBtn;
}

QPushButton* fillHole::getApplyBtn()
{
	return ui->applyFillHoleBtn;
}

void fillHole::onBtnHandle(FillHoleType type)
{
	int level = -1, _fileDataType = 1000, _holeIndex = -1;
	double _perimeter;
	bool _markerFlag;

	//ÆÁ±Î_perimeter
	//ui->lineEdit_4->setHidden(true);
	//ui->label_6->setHidden(true);

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

	QString comFileDta = ui->comboBox_2->currentText();
	if (!comFileDta.compare("NO_SMOOTH"))
	{
		_fileDataType = 0;
	}
	else if (!comFileDta.compare("FLAT")){
		_fileDataType = 1;
	}
	else if (!comFileDta.compare("TANGENT"))
	{
		_fileDataType = 2;
	}
	else{
		_fileDataType = 3;
	}

// 	_fileDataType = ui->lineEdit_2->text().toInt();
 	_perimeter = ui->lineEdit_4->text().toDouble();
	if (!ui->lineEdit_3->text().isEmpty())
	{
		_holeIndex = ui->lineEdit_3->text().toInt();
	}
	_markerFlag = ui->checkBox->isChecked() == true ? true: false;

	//parse hole list
	QByteArray arrHoleList;
	QString strHoleList = ui->lineEdit->text();
	if (!strHoleList.isEmpty())
	{
		auto holelist = strHoleList.split(',');
		for (int i = 0; i < holelist.size(); i++){
			QString str = holelist.at(i);
			arrHoleList.append(str);
		}
	}
	//end

	QJsonObject jsonobject;
	jsonobject.insert("holeList", arrHoleList.constData());
	jsonobject.insert("fileDataType", _fileDataType);
	jsonobject.insert("level", level);
	jsonobject.insert("num", _holeIndex);//holeIndex
	jsonobject.insert("markerFlag", _markerFlag);
	jsonobject.insert("perimeter", _perimeter);

	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray retHole = document.toJson();
	emit fillHoleSignal(retHole, type);
	this->hide();
}

void fillHole::on_EditFillHoleBtn_clicked()
{
	//ui->lineEdit->setEnabled(false);
	onBtnHandle(Edit_Fill_Hole);
}

void fillHole::on_applyFillHoleBtn_clicked()
{
	//ui->lineEdit_3->setEnabled(false);
	onBtnHandle(Apply_Fill_Hole);
}