#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <zmq.h>
#include <QSocketNotifier>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QPixmap>
#include "progressdialog.h"
#include "subscriber.h"
#include "dataprocesser.h"
#include "commonui.h"
#include "cancelscan.h"
#include "endscan.h"
#include "mesh.h"
#include "save.h"
#include "startscan.h"
#include "simplify.h"
namespace Ui {
class MainWindow;
}

#define MAX_ENVELOPE_LENGTH 255
#define MAX_DATA_LENGTH 1000

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
	/*
	cmd:strings prescribed in SDK document
	Encapsulate a ZMQ function without data
	*/
	bool request(const QString& cmd);

	/*
	cmd:strings prescribed in SDK document
	jsonObj:jsonobject prescribed in SDK document
	Encapsulate a ZMQ function to send data
	*/
	bool request(const QString& cmd, const QJsonObject& jsonObj);

	/*
	socket:ZMQ socket
	Check if there is any data received
	*/
	bool hasMore(void* socket);
private slots:
//There are some SDK test function ,  refer to SDK Document
	void on_pushButton_DeviceCheck_clicked();// The button on the interface press to trigger,refer to SDK Doc
	
	//Get scan information 
	void ScanStatus();
	void ScanTooFlat();
	void ScanTrackLost();
	void ScanNoMarkerDetected();
	void ScanDist();
	void ScanTriangleCount();
	void ScanFramerate();
	void ScanPointCount();
	void ScanMarkerCount();
	void on_pushButton_Refresh_clicked();
	void on_pushButton_ScanLastSimplifyParams_clicked();

	//Scan control  ---refer to SDK Doc
	void on_pushButton_Pre_clicked();
	void on_pushButton_start_clicked();
	void on_pushButton_Pause_clicked();


	//To receive shared memories,
	void on_pushButton_RegisterProcesser_clicked();
	void on_pushButton_UnregisterProcesser_clicked();
	

	
	void on_pushButton_ScanExitScan_clicked();


	//Send a  request with a json data
	void on_pushButton_ScanNewProject_clicked();
	void on_pushButton_scanEndScan_clicked();
	void on_pushButton_scanCancelScan_clicked();
	void on_pushButton_scanMesh_clicked();
	void on_pushButton_scanSave_clicked();
	void on_pushButton_scanSimplify_clicked();
	//Send a  request with a string data
	void on_pushButton_ScaneEnterScan_clicked();
	void on_pushButton_ScanOpenProject_clicked();
	void on_pushButton_ScanExportFile_clicked();


    void onHeartbeat();//When the heartbeat stops,count to zero and start reporting errors
	/*
	majorCmd: main command
	minorCmd:secondary command
	data:Refer to SDK document,the data is different
	SDK publish informations,in this function to handle
	*/
    void onPublishReceived(QString majorCmd, QString minorCmd, QByteArray data);
	/*
	camID: image area displayed on the main interface
	pixmap:image data
	This function to show video
	*/
	void onVideoImageReady(int camID, QPixmap pixmap);
private:
public:
	/*
	socket:ZMQ socket
	cmd:envelop cmd
	data:if you need to send additional parameters ,data is not empty.
	Encapsulate a ZMQ function to send data
	*/
	bool sendData(void* socket, const QString& cmd, const QByteArray& data);


	static inline QByteArray jsonStr(const QJsonObject& jo){ return QJsonDocument(jo).toJson(QJsonDocument::Compact); }//Turn QJsonObject to QByteArray
    static inline QJsonObject jsonObject(const QByteArray& data) {auto doc = QJsonDocument::fromJson(data); return doc.object();}//Turn QByteArray to QJsonObject
private:
    Ui::MainWindow *ui;
	void* m_zmqContext = nullptr;
    void* m_zmqReqSocket = nullptr;
	void* m_zmqDataProcesserSocket = nullptr;

	QTimer* m_heartbeatTimer = nullptr;

    QThread* m_subscriberThread = nullptr;
    Subscriber* m_subscriber = nullptr;
    ProgressDialog* m_progressDialog = nullptr;
	QThread* m_dataProcesserThread = nullptr;
	DataProcesser* m_dataProcesser = nullptr;
	NewProject *m_newProject;
	cancelScan *m_cancelScan;
	endScan *m_endScan;
	mesh *m_mesh;
	save *m_save;
	startScan *m_startScan;
	Simplify *m_simplify;

public slots://Response signal to  call SDK interface  
	void onNewProject(QByteArray);
	void onStartScan(QByteArray);
	void onEndScan(QByteArray);
	void onCancelScan(QByteArray);
	void onMesh(QByteArray);
	void onSave(QByteArray);
	void onSimplify(QByteArray);

protected:
	void closeEvent(QCloseEvent *event);//When the main thread  exit,it exits the sub-thread
	bool typeBool(QString type);//Get boolean values corresponding to characters.
	
};

#endif // MAINWINDOW_H
