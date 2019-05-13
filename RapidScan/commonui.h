#ifndef COMMON_H
#define COMMON_H
/*
An interface corresponding to a "New project" button
*/
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QHBoxLayout>
#include<QVBoxLayout>
#include <QCheckBox>
class NewProject : public QDialog
{
	Q_OBJECT
public:
	explicit NewProject(QDialog *parent = nullptr);
	~NewProject();
	QLabel *m_globalMarkerFile;
	QLabel * m_textureEnabled;
	QLabel * m_pointDist;
	QLabel * m_alignType;
	QLabel * m_rapidMode;
	QLabel * m_fastSave;

	QLineEdit *m_pathR;
	QLineEdit *m_globalMarkerFileR;
	QCheckBox * m_textureEnabledR;
	QLineEdit * m_pointDistR;
	QComboBox * m_alignTypeR;
	QCheckBox * m_rapidModeR;
	QCheckBox * m_fastSaveR;
	QPushButton *m_button_OK;
	QPushButton *m_button_path;

public slots:
	void onPushButtonClicked();
	void onPathButtonClicked();
 signals:
	void newProject(QByteArray);//The mainwindow receive this signal,the parameter is json data

	
};


#endif 
