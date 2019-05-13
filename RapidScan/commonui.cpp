#include "commonui.h"
#include <QGridLayout>
#include <QFileDialog>
NewProject::NewProject(QDialog *parent /*= nullptr*/):QDialog(parent)
{
	m_button_path = new QPushButton("Path", this);
	m_globalMarkerFile =new QLabel("Global marker file",this);
	m_pointDist = new QLabel("Point distance", this);
	m_alignType = new QLabel("Align type", this);

	m_pathR=new QLineEdit("", this);
	m_globalMarkerFileR = new QLineEdit("", this);
	m_textureEnabledR = new QCheckBox(this);
	m_pointDistR = new QLineEdit("", this);
	m_alignTypeR = new QComboBox(this);
	m_rapidModeR = new QCheckBox(this);
	m_fastSaveR = new QCheckBox(this);
	m_textureEnabledR->setCheckable(true);
	m_textureEnabledR->setText("Texture enabled          ");
	m_rapidModeR->setCheckable(true);
	m_rapidModeR->setText("Rapid mode           ");
	m_fastSaveR->setCheckable(true);
	m_fastSaveR->setText("Fast save");

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

	hlay2->addWidget(m_textureEnabledR);
	hlay2->addWidget(m_pointDist);
	hlay2->addWidget(m_pointDistR);

	hlay3->addWidget(m_rapidModeR);
	hlay3->addWidget(m_alignType);
	hlay3->addWidget(m_alignTypeR);
	
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
	m_pathR->setEnabled(false);
}

NewProject::~NewProject()
{

}



void NewProject::onPushButtonClicked( )
{
	QString strPath = m_pathR->text();
	QString strGlovalMarkerFile = m_globalMarkerFileR->text();
	bool strtextureEnabled = m_textureEnabledR->isChecked();
	QString strpointDist = m_pointDistR->text();
	QString stralignType = m_alignTypeR->currentText();
	bool strrapidMode = m_rapidModeR->isChecked();
	bool strfastSave = m_fastSaveR->isChecked();
	QJsonObject jsonobject;
	jsonobject.insert("path", strPath);
	jsonobject.insert("globalMarkerFile", strGlovalMarkerFile);
	jsonobject.insert("textureEnabled", strtextureEnabled);
	jsonobject.insert("pointDist", strpointDist.toDouble());
	jsonobject.insert("alignType", stralignType);
	jsonobject.insert("rapidMode", strrapidMode);
	jsonobject.insert("fastSave", strfastSave);
	QJsonDocument document;
	document.setObject(jsonobject);
	QByteArray result = document.toJson();
	emit newProject(result);
	this->hide();
}

void NewProject::onPathButtonClicked()
{
	QString path = QFileDialog::getSaveFileName(this, QStringLiteral("select a file"));
	m_pathR->setText(path);
}

