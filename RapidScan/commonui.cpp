#include "commonui.h"
#include <QGridLayout>
#include <QFileDialog>
NewProject::NewProject(QDialog *parent /*= nullptr*/):QDialog(parent)
{
	m_button_path = new QPushButton("Path", this);
	m_globalMarkerFile =new QLabel("Global marker file",this);
	m_textureEnabled =new QLabel("Texture enabled",this);
	m_pointDist = new QLabel("Point distance", this);
	m_alignType = new QLabel("Align type", this);
	m_rapidMode = new QLabel("Rapid mode", this);
	m_fastSave = new QLabel("Fast save", this);

	m_pathR=new QLineEdit("", this);
	m_globalMarkerFileR = new QLineEdit("", this);
	m_textureEnabledR = new QComboBox(this);
	m_pointDistR = new QLineEdit("", this);
	m_alignTypeR = new QComboBox(this);
	m_rapidModeR = new QComboBox(this);
	m_fastSaveR = new QComboBox(this);
	m_textureEnabledR->addItem("true");
	m_textureEnabledR->addItem("false");
	m_rapidModeR->addItem("true");
	m_rapidModeR->addItem("false");
	m_fastSaveR->addItem("true");
	m_fastSaveR->addItem("false");

	m_alignTypeR->addItem("AT_FEATURES");
	m_alignTypeR->addItem("AT_MARKERS");
	m_alignTypeR->addItem("AT_HYBRID");
	m_alignTypeR->addItem("AT_AUTO");
	m_alignTypeR->addItem("AT_TURTABLE");
	m_alignTypeR->addItem("AT_CODE_POINT");
	m_alignTypeR->addItem("AT_GLOBAL_POINT");

	QHBoxLayout *hlay1=new QHBoxLayout();
	QHBoxLayout *hlay2 = new QHBoxLayout();
	QHBoxLayout *hlay3 = new QHBoxLayout();
	QHBoxLayout *hlay4 = new QHBoxLayout();
	
	hlay1->addWidget(m_button_path);
	hlay1->addWidget(m_pathR);
	hlay1->addWidget(m_globalMarkerFile);
	hlay1->addWidget(m_globalMarkerFileR);

	hlay2->addWidget(m_textureEnabled);
	hlay2->addWidget(m_textureEnabledR);
	hlay2->addWidget(m_pointDist);
	hlay2->addWidget(m_pointDistR);

	hlay3->addWidget(m_alignType);
	hlay3->addWidget(m_alignTypeR);
	hlay3->addWidget(m_rapidMode);
	hlay3->addWidget(m_rapidModeR);
	hlay4->addWidget(m_fastSave);
	hlay4->addWidget(m_fastSaveR);

	m_button_OK = new QPushButton("OK", this);
	
	hlay4->addWidget(m_button_OK);
	QVBoxLayout *vlay = new QVBoxLayout(this);
	vlay->addLayout(hlay1);
	vlay->addLayout(hlay2);
	vlay->addLayout(hlay3);
	vlay->addLayout(hlay4);

	
	this->setLayout(vlay);
	connect(m_button_OK, SIGNAL(clicked()), this, SLOT(onPushButtonClicked()));
	connect(m_button_path, SIGNAL(clicked()), this, SLOT(onPathButtonClicked()));
	this->setMinimumSize(320, 450);
	this->setModal(false);
	this->setWindowTitle("NewProject");
}

NewProject::~NewProject()
{

}

bool NewProject::typeBool(QString type)
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

void NewProject::onPushButtonClicked( )
{
	QString strPath = m_pathR->text();
	QString strGlovalMarkerFile = m_globalMarkerFileR->text();
	QString strtextureEnabled = m_textureEnabledR->currentText();
	QString strpointDist = m_pointDistR->text();
	QString stralignType = m_alignTypeR->currentText();
	QString strrapidMode = m_rapidModeR->currentText();
	QString strfastSave = m_fastSaveR->currentText();
	QJsonObject jsonobject;
	jsonobject.insert("path", strPath);
	jsonobject.insert("globalMarkerFile", strGlovalMarkerFile);
	jsonobject.insert("textureEnabled",typeBool(strtextureEnabled) );
	jsonobject.insert("pointDist", strpointDist.toDouble());
	jsonobject.insert("alignType", stralignType);
	jsonobject.insert("rapidMode", typeBool(strrapidMode));
	jsonobject.insert("fastSave", typeBool(strfastSave));
	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray result = document.toJson();
	emit newProject(result);
	this->hide();
}

void NewProject::onPathButtonClicked()
{
	QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("select a file path"));
	m_pathR->setText(path);
}

