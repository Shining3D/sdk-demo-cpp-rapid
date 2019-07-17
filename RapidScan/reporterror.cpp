#include"reporterror.h"
#include <QString>
#include <QTextBrowser>
#include <QGridLayout>

ReportError::ReportError(QTextBrowser *parent) :QTextBrowser(parent)
{
	m_reportError = new QTextBrowser(this);
	//m_cancelButton = new QPushButton("Cancel",this);

	QVBoxLayout *vlay = new QVBoxLayout(this);
	vlay->addWidget(m_reportError);
	//vlay->addWidget(m_cancelButton);
	m_reportError->setMinimumSize(180,60);

	//connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));

	this->setLayout(vlay);
	this->setMinimumSize(250, 100);
	//this->setModal(false);
	this->setWindowTitle("Report Error");
}
ReportError::~ReportError()
{
}
void ReportError::onCancelButtonClicked()
{
	this->hide();
}
