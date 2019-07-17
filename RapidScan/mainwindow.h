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
#include <QWidget>
#include <QVector>
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
#include "reporterror.h"
#include "smooth_sharp_zoom.h"
#include "sharpdlg.h"
#include "zoomdlg.h"
#include "fillhole.h"


namespace Ui {
class MainWindow;
}

#define MAX_ENVELOPE_LENGTH 255
#define MAX_DATA_LENGTH 1000

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);//explicit 禁止参数的隐式转换
    ~MainWindow();
public:
	/*
	cmd:strings prescribed in SDK document//SDK文档中规定的字符串
	Encapsulate a ZMQ function without data//封装了一个没有参数的ZMQ函数
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
	Check if there is any data received//检查是否接收到任何数据
	*/
	bool hasMore(void* socket);
private slots://槽
//There are some SDK test function ,  refer to SDK Document//SDK测试功能，参考SDK文档
    void on_pushButton_DeviceCheck_clicked();// The button on the interface press to trigger,refer to SDK Doc
	
	//Get scan information 
	void ScanStatus();//设备状态？
	void ScanTooFlat();//太平？复选框
	void ScanTrackLost();//设备跟踪丢失？复选框
	void ScanNoMarkerDetected();//设备没有检测到标记？复选框
	void ScanDist();//设备距离
	int ScanTriangleCount();//三角面片数量
	void ScanFramerate();//帧率
	void ScanPointCount();//点云数量
	void ScanMarkerCount();//标志点数量
	void ScanMeshPointCount();//网格数量
	void on_pushButton_Refresh_clicked();//刷新按钮的响应函数
	void on_pushButton_ScanLastSimplifyParams_clicked();//最后简化按钮的响应函数？？？？

	//Scan control  ---refer to SDK Doc
	void on_pushButton_Pre_clicked();//预览
	void on_pushButton_start_clicked();//开始
	void on_pushButton_Pause_clicked();//暂停


	//To receive shared memories,//分享共同的内存？
	void on_pushButton_RegisterProcesser_clicked();//注册按钮按下响应
	void on_pushButton_UnregisterProcesser_clicked();//关闭注册按钮
	
	void on_pushButton_ScanExitScan_clicked();//设备退出响应


	//Send a  request with a json data
	void on_pushButton_NewProFilePath_clicked();//新建项目按钮
	void on_pushButton_scanEndScan_clicked();//结束扫描按钮
	void on_pushButton_scanCancelScan_clicked();//取消扫描按钮
	void on_pushButton_scanMesh_clicked();//生成网格按钮
	void on_pushButton_scanSave_clicked();//保存数据按钮
	void on_pushButton_scanSimplify_clicked();//简化按钮的响应函数
	//Send a  request with a string data
	void on_pushButton_ScaneEnterScan_clicked();//进入设备扫描
	void on_pushButton_OpenProFilePath_clicked();//打开项目按钮

	void on_SmoothBtn_clicked();
	void on_SharpBtn_clicked();
	void on_ZoomBtn_clicked();
	void on_EditFillHoleBtn_clicked();
	void on_applyFillHoleBtn_clicked();
	void on_getAllHoleInfoBtn_clicked();

	void on_pushButton_Step1Next_clicked();
	void on_pushButton_Step2Next_clicked();
	void on_pushButton_Step2Back_clicked();
	void on_pushButton_Step3Next_clicked();
	void on_pushButton_Step3Back_clicked();
	void on_pushButton_Step4Next_clicked();
	void on_pushButton_Step4Back_clicked();
	void on_pushButton_Step5Back_clicked();
	void on_pushButton_NewProject_clicked();
	void on_pushButton_OpenProOpen_clicked();
	void on_pushButton_OpenProCancel_clicked();
	void on_pushButton_step5Refresh_clicked();
	void onHeartbeat();//When the heartbeat stops,count to zero and start reporting errors//心率，当心率停止时，数据为0，报错？

	void on_pushButton_pro_clicked();
	void on_pushButton_pro_plus_clicked();

	/*
	majorCmd: main command
	minorCmd: secondary command
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
	void execTerminate();
	void nextStep(int num);
	void backStep(int num);
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
	ReportError *m_reportError;
	bool m_bSimplify;
	smooth_sharp_zoom *m_pSmoothShapZoom;
	sharpDlg *m_pSharp;
	zoomDlg *m_pZoom;
	fillHole *m_pFillHole;

	QVector<QWidget*> widget_step;

public slots://Response signal to  call SDK interface  
	void onNewProject(QByteArray);
	void onStartScan(QByteArray);
	void onEndScan(QByteArray);
	void onCancelScan(QByteArray);
	void onMesh(QByteArray);
	void onSave(QByteArray);
	void onSimplify(QByteArray);

	void onSmoothBtn(QByteArray);
	void onSharpBtn(QByteArray);
	void onZoomBtn(QByteArray);

	void onFillHole(QByteArray, FillHoleType);
	void onEditFillHoleBtn(QByteArray);
	void onApplyFillHoleBtn(QByteArray);


protected:
	void closeEvent(QCloseEvent *event);//When the main thread  exit,it exits the sub-thread
	
signals:
	void newProject(QByteArray);//The mainwindow receive this signal,the parameter is json data
public:
	QString cancelPath;

};

#endif // MAINWINDOW_H
