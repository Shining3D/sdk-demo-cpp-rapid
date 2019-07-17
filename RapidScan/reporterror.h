#ifndef REPORTERROR_H
#define REPORTERROR_H

#include <QDialog>
#include <QByteArray>
#include <QTextBrowser>
#include <QPushButton>

class ReportError :public QTextBrowser
{
	Q_OBJECT
public:
	 ReportError(QTextBrowser *parent = nullptr);
	~ReportError();
	QTextBrowser *m_reportError;
	//QPushButton *m_cancelButton;
public slots:
    void onCancelButtonClicked();
};
#endif