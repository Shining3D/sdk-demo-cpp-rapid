#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <cassert>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QSharedMemory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_progressDialog = new ProgressDialog(this);

	m_zmqContext = zmq_ctx_new();
	auto err = zmq_strerror(zmq_errno());

    m_subscriberThread = new QThread(this);//A thread to handle subscribe
    m_subscriber = new Subscriber(this, m_zmqContext);
    m_subscriber->moveToThread(m_subscriberThread);
    connect(m_subscriberThread, &QThread::finished, m_subscriber, &QObject::deleteLater);
    connect(m_subscriber, &Subscriber::heartbeat, this, &MainWindow::onHeartbeat, Qt::QueuedConnection);
	connect(m_subscriber, &Subscriber::publishReceived, this, &MainWindow::onPublishReceived, Qt::QueuedConnection);
    m_subscriberThread->start();
	

	m_dataProcesserThread = new QThread(this);
	m_dataProcesser = new DataProcesser(this, m_zmqContext);
	m_dataProcesser->moveToThread(m_dataProcesserThread);
	connect(m_dataProcesserThread, &QThread::finished, m_dataProcesser, &QObject::deleteLater);
	connect(m_dataProcesser, &DataProcesser::videoImageReady, this, &MainWindow::onVideoImageReady, Qt::QueuedConnection);
	m_dataProcesserThread->start();

	QMetaObject::invokeMethod(m_subscriber, "setup", Qt::QueuedConnection, Q_ARG(QString, "tcp://localhost:11398"));

	m_zmqReqSocket = zmq_socket(m_zmqContext, ZMQ_REQ);
    auto rc = zmq_connect(m_zmqReqSocket, "tcp://localhost:11399");
	assert(!rc);


	m_heartbeatTimer = new QTimer(this);
	m_heartbeatTimer->setInterval(210 );
	connect(m_heartbeatTimer, &QTimer::timeout, this, [&]{
		auto currentCount = ui->lcdNumber->intValue();
		if (currentCount == 0){
			m_heartbeatTimer->stop();
			QMessageBox::critical(this, "ERROR", "The platform died!");
		}
		else{
			currentCount--;
			ui->lcdNumber->display(currentCount);
		}
	});
	m_heartbeatTimer->start();

	QTimer::singleShot(0, this, [&]{
		m_progressDialog->onBeginAsync("Pulling...");
		QCoreApplication::processEvents();
		const char *sendData = "v1.0/pull";
		int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);

		char buf[MAX_DATA_LENGTH * 2+ 1] = { 0 };
		nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH * 2, 0);
		auto jsonDoc = QJsonDocument::fromJson(buf);
		qDebug() << "pull results:" << jsonDoc;
		assert(!hasMore(m_zmqReqSocket));
		m_progressDialog->onFinishAsync();
		QCoreApplication::processEvents();
	});

	m_newProject = new NewProject();

	m_newProject->hide();
	connect(m_newProject,&NewProject::newProject, this, &MainWindow::onNewProject);

	m_cancelScan=new cancelScan;
	m_endScan=new endScan;
	m_mesh=new mesh;
	m_save=new save;
	m_startScan=new startScan;
	m_simplify = new Simplify;

	connect(m_startScan, &startScan::startScanSignal, this, &MainWindow::onStartScan);
	connect(m_endScan, &endScan::endScanSignal, this, &MainWindow::onEndScan);
	connect(m_cancelScan, &cancelScan::cancelScanSignal, this, &MainWindow::onCancelScan);

	connect(m_mesh, &mesh::meshSignal, this, &MainWindow::onMesh);
	connect(m_save, &save::saveSignal, this, &MainWindow::onSave);
	connect(m_simplify, &Simplify::simplifySignal, this, &MainWindow::onSimplify);
	ui->widget->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
	zmq_close(m_zmqReqSocket);
	zmq_ctx_destroy(m_zmqContext);
}

bool MainWindow::request(const QString& cmd, const QJsonObject& jsonObj)
{
	return sendData(m_zmqReqSocket, cmd, jsonStr(jsonObj));
}

bool MainWindow::request(const QString& cmd)
{
	return sendData(m_zmqReqSocket, cmd, "");
}

bool MainWindow::hasMore(void* socket)
{
	int more = 0;
	size_t moreSize = sizeof(more);
	int rc = zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &moreSize);
	qDebug() << "hasMore:" << more <<"andRc:"<<rc<< endl;

	return more != 0;
}


void MainWindow::on_pushButton_DeviceCheck_clicked()
{
	const char *sendData = "v1.0/device/check";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);


    int result = 0;
    nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
    qDebug() << "recv reply data:" << (result == 0? false : true);
	if (result != 0)
	{
		ui->widget->setEnabled(true);
	}
    assert(!hasMore(m_zmqReqSocket));
}


void MainWindow::on_pushButton_RegisterProcesser_clicked()
{
	m_dataProcesser->setReqSocket(m_zmqReqSocket);
	qDebug() << "on_pushButton_RegisterProcesser_clicked" << endl;
	QMetaObject::invokeMethod(m_dataProcesser, "setup", Qt::QueuedConnection, Q_ARG(int, 12000));//12000
}

void MainWindow::on_pushButton_UnregisterProcesser_clicked()
{
	if (m_zmqDataProcesserSocket) return;
	m_zmqDataProcesserSocket = zmq_socket(m_zmqContext, ZMQ_REP);
	const char *processUrl = "tcp://localhost:12000";
// 	int rc = zmq_bind(m_zmqDataProcesserSocket, processUrl);
// 	assert(!rc);
	const char * envelop = "v1.0/scan/unregister";
	auto nbytes = zmq_send(m_zmqReqSocket, envelop, strlen(envelop), ZMQ_SNDMORE);
	if (nbytes != strlen(envelop)){
		qWarning() << "cannot send register envelop!";
		return;
	}
	nbytes = zmq_send(m_zmqReqSocket, processUrl, strlen(processUrl), 0);
	if (nbytes != strlen(processUrl)){
		qWarning() << "cannot send register processurl!";
		return;
	}
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
}




void MainWindow::ScanTriangleCount()
{
	const char *sendData = "v1.0/scan/triangleCount";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	int num;
	memcpy(&num, buf, sizeof(int));
	qDebug() << "scan triangleCount:" << num;
	ui->label_triangleCountR->setText(QString::number(num));
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::ScanFramerate()
{
	const char *sendData = "v1.0/scan/framerate";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	int num;
	memcpy(&num, buf, sizeof(int));
	qDebug() << "scan framerate:" << num;
	
	ui->label_frameRateR->setText(QString::number(num));
	assert(!hasMore(m_zmqReqSocket));
}


void MainWindow::ScanPointCount()
{
	const char *sendData = "v1.0/scan/pointCount";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	int num;
	memcpy(&num, buf, sizeof(int));
	qDebug() << "scan pointCount:" << num;
	ui->label_pointCountR->setText(QString::number(num));
	assert(!hasMore(m_zmqReqSocket));
}


void MainWindow::ScanMarkerCount()
{
	const char *sendData = "v1.0/scan/markerCount";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	int num;
	memcpy(&num, buf, sizeof(int));
	qDebug() << "scan markerCount:" << num;
	ui->label_markerCountR->setText(QString::number(num));
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::on_pushButton_Refresh_clicked()
{
	 ScanStatus();
	 ScanTooFlat();
	 ScanTrackLost();
	 ScanNoMarkerDetected();
	 ScanDist();
	 ScanTriangleCount();
	 ScanFramerate();
	 ScanPointCount();
	 ScanMarkerCount();
}



void MainWindow::on_pushButton_ScanLastSimplifyParams_clicked()
{
	const char *sendData = "v1.0/scan/lastSimplifyParams";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	qDebug() << "scan lastSimplifyParams:" << buf;
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::on_pushButton_Pre_clicked()
{
	m_startScan->setAction("pre");
	m_startScan->show();
	m_startScan->setSubType(0);
}

void MainWindow::on_pushButton_start_clicked()
{
	m_startScan->setAction("start");
	m_startScan->show();
	m_startScan->setSubType(0);
}

void MainWindow::on_pushButton_Pause_clicked()
{
	m_startScan->setAction("pause");
	m_startScan->show();
	m_startScan->setSubType(0);
}

void MainWindow::ScanNoMarkerDetected()
{
	const char *sendData = "v1.0/scan/noMarkerDetected";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);

	int valInt;
	memcpy(&valInt, buf, sizeof(int));
	auto valBool = valInt == 0 ? false : true;
	qDebug() << "scan noMarkerDetected:" << valBool;
	ui->checkBox_noMarkerDetected->setChecked(valBool);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::ScanStatus()
{
	const char *sendData = "v1.0/scan/status";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	qDebug() << "scan status:" << buf;
	ui->label_scanStatusR->setText(buf);
	assert(!hasMore(m_zmqReqSocket));
}





void MainWindow::on_pushButton_ScanNewProject_clicked()
{
	m_newProject->show();
}

void MainWindow::on_pushButton_scanEndScan_clicked()
{
	m_endScan->show();
}

void MainWindow::on_pushButton_scanCancelScan_clicked()
{
	m_cancelScan->show();
}


void MainWindow::on_pushButton_scanMesh_clicked()
{
	m_mesh->show();
}

void MainWindow::on_pushButton_scanSave_clicked()
{
	m_save->show();
}

void MainWindow::on_pushButton_scanSimplify_clicked()
{
	m_simplify->show();
}

void MainWindow::on_pushButton_ScaneEnterScan_clicked()
{
	const char *sendData = "v1.0/scan/enterScan";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send enterScan ZMQ_SNDMORE" << "nbytes" << nbytes;
		return;
	}
	QString str = QStringLiteral("ST_RAPID");
	nbytes = zmq_send(m_zmqReqSocket, str.toLocal8Bit().constData(), str.size(), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send enterScan";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "enterScan recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::onNewProject(QByteArray sendMore)
{
	char buf[MAX_DATA_LENGTH + 1] = { "v1.0/scan/newProject" };
	const char *sendData = buf;
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send newProject"<<"nbytes"<< nbytes;
		return;
	}
	QString str(sendMore);
	char buf2[MAX_DATA_LENGTH + 1] = { 0 };
	for (int i = 0; i < str.size(); i++)
	{
		buf2[i] = str.at(i).toLatin1();
	}
	const char *sendData2 = buf2;
	nbytes = zmq_send(m_zmqReqSocket, sendData2, strlen(sendData2), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send newProject";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "onNewProject recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::onStartScan(QByteArray sendMore)
{
	char buf[MAX_DATA_LENGTH + 1] = { "v1.0/scan/control" };
	const char *sendData = buf;
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send controlScan";
		return;
	}
	qDebug() << sendMore << endl;
 	QString str(sendMore);
	QByteArray dd = sendMore;
	nbytes = zmq_send(m_zmqReqSocket, dd.constData(), dd.size(), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send  controlScan";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "onStartScan recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::onEndScan(QByteArray sendMore)
{
	char buf[MAX_DATA_LENGTH + 1] = { "v1.0/scan/endScan" };
	const char *sendData = buf;
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send endScan" << "nbytes" << nbytes;
		return;
	}
	QString str(sendMore);
	char buf2[MAX_DATA_LENGTH + 1] = { 0 };
	for (int i = 0; i < str.size(); i++)
	{
		buf2[i] = str.at(i).toLatin1();
	}
	const char *sendData2 = buf2;
	nbytes = zmq_send(m_zmqReqSocket, sendData2, strlen(sendData2), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send endScan";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "onEndScan recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::onCancelScan(QByteArray sendMore)
{
	char buf[MAX_DATA_LENGTH + 1] = { "v1.0/scan/cancelScan" };
	const char *sendData = buf;
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send cancelScan";
		return;
	}
	QString str(sendMore);
	qDebug() << "onCancelScan" << str << endl;
	char buf2[MAX_DATA_LENGTH + 1] = { 0 };
	for (int i = 0; i < str.size(); i++)
	{
		buf2[i] = str.at(i).toLatin1();
	}
	const char *sendData2 = buf2;
	nbytes = zmq_send(m_zmqReqSocket, sendData2, strlen(sendData2), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send cancelScan";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "onCancelScan recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::onMesh(QByteArray sendMore)
{
	char buf[MAX_DATA_LENGTH + 1] = { "v1.0/scan/mesh" };
	const char *sendData = buf;
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send mesh";
		return;
	}
	QString str(sendMore);
	char buf2[MAX_DATA_LENGTH + 1] = { 0 };
	for (int i = 0; i < str.size(); i++)
	{
		buf2[i] = str.at(i).toLatin1();
	}
	const char *sendData2 = buf2;
	nbytes = zmq_send(m_zmqReqSocket, sendData2, strlen(sendData2), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send mesh";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "onMesh recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::onSave(QByteArray sendMore)
{
	char buf[MAX_DATA_LENGTH + 1] = { "v1.0/scan/save" };
	const char *sendData = "v1.0/scan/save";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send onSave";
		return;
	}
	QString str(sendMore);
	char buf2[MAX_DATA_LENGTH + 1] = { 0 };
	for (int i = 0; i < str.size(); i++)
	{
		buf2[i] = str.at(i).toLatin1();
	}
	const char *sendData2 = buf2;
	nbytes = zmq_send(m_zmqReqSocket, sendData2, strlen(sendData2), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send save";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "onSave recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::onSimplify(QByteArray sendMore)
{
	const char *sendData = "v1.0/scan/lastSimplifyParams/set";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send scan onSimplify";
		return;
	}

	nbytes = zmq_send(m_zmqReqSocket, sendMore.constData(), strlen(sendMore.constData()), 0);
	if (nbytes != sendMore.size()) {
		qWarning() << "cannot send scan onSimplify";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "scan onSimplify recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	
	m_subscriberThread->quit();
	m_subscriberThread->exit(0);
	connect(m_subscriberThread, SIGNAL(finished()), m_subscriberThread, SLOT(deleteLater()));
	
	exit(0);
	MainWindow::closeEvent(event);
}

bool MainWindow::typeBool(QString type)
{
	if (type=="true")
	{
		return true;
	}
	else
	{
		return false;
	}
}
void MainWindow::on_pushButton_ScanOpenProject_clicked()
{
	QString str = QFileDialog::getOpenFileName(this, QStringLiteral("select a file"));
	ui->lineEdit_OpenProject->setText(str);
	const char *sendData = "v1.0/scan/openProject";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send ScanOpenProject  ZMQ_SNDMORE " << "nbytes" << nbytes;
		return;
	}
	nbytes = zmq_send(m_zmqReqSocket, str.toLocal8Bit().constData(), str.size(), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send ScanOpenProject";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "openProject recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::on_pushButton_ScanExportFile_clicked()
{
	const char *sendData = "v1.0/scan/exportFile";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), ZMQ_SNDMORE);
	if (nbytes != strlen(sendData)) {
		qWarning() << "cannot send ScanExportFile ZMQ_SNDMORE " << "nbytes" << nbytes;
		return;
	}
	QString str = ui->comboBox_scanExportFile->currentText();
	nbytes = zmq_send(m_zmqReqSocket, str.toLocal8Bit().constData(), str.size(), 0);
	if (nbytes != str.size()) {
		qWarning() << "cannot send ScanExportFile";
		return;
	}
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "exportFile recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::ScanDist()
{
	const char *sendData = "v1.0/scan/dist";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	int num;
	memcpy(&num, buf, sizeof(int));
	qDebug() << "scan dist:" << num;
	
	ui->label_scandistR->setText(QString::number(num));
	assert(!hasMore(m_zmqReqSocket));
}


void MainWindow::ScanTooFlat()
{
	const char *sendData = "v1.0/scan/tooFlat";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	
	int valInt;
	memcpy(&valInt, buf, sizeof(int));
	auto valBool = valInt == 0 ? false : true;
	qDebug() << "scan tooFlat:" << valBool;
	ui->checkBox_tooFlat->setChecked(valBool);
	assert(!hasMore(m_zmqReqSocket));
}




void MainWindow::ScanTrackLost()
{
	const char *sendData = "v1.0/scan/trackLost";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(m_zmqReqSocket, buf, MAX_DATA_LENGTH, 0);
	
	int valInt;
	memcpy(&valInt, buf, sizeof(int));
	auto valBool = valInt == 0 ? false : true;
	qDebug() << "scan trackLost:" << valBool;
	ui->checkBox_trackLost->setChecked(valBool);
	assert(!hasMore(m_zmqReqSocket));
}

void MainWindow::on_pushButton_ScanExitScan_clicked()
{
	const char *sendData = "v1.0/scan/exitScan";
	int nbytes = zmq_send(m_zmqReqSocket, sendData, strlen(sendData), 0);
	int result = 0;
	nbytes = zmq_recv(m_zmqReqSocket, &result, sizeof(int), 0);
	qDebug() << "exitScan recv reply data:" << (result == 0 ? false : true);
	assert(!hasMore(m_zmqReqSocket));
}



void MainWindow::onHeartbeat()
{
    m_heartbeatTimer->start();
    ui->lcdNumber->display(10);
}

void MainWindow::onPublishReceived(QString majorCmd, QString minorCmd, QByteArray data)
{
	if (majorCmd == QStringLiteral("beginAsyncAction")) {
		auto jsonObj = jsonObject(data);
		qDebug() << "beginAsyncAction json object:" << jsonObj;
		auto type = jsonObj["type"].toString();
		auto props = jsonObj["props"].toObject();

		ui->label_AsyBeginTypeR->setText(type);
		QJsonDocument document;
		document.setObject(props);
		QByteArray propsByte = document.toJson();

		ui->label_AsyBeginPropsR->setText(propsByte);
		m_progressDialog->onBeginAsync(type);
		qDebug() << "type" << type << "\n" << "props" << propsByte << endl;
	}
	else if (majorCmd == QStringLiteral("finishAsyncAction")) {
		auto jsonObj = jsonObject(data);
		qDebug() << "finishAsyncAction json object:" << jsonObj;
		auto type = jsonObj["type"].toString();
		auto props = jsonObj["props"].toObject();
		auto result = jsonObj["result"].toString();
		if (props["type"] != QJsonValue::Undefined) {
			
		}
		ui->label_AsyFinishTypeR->setText(type);
		QJsonDocument document;
		document.setObject(props);
		QByteArray propsByte = document.toJson();

		ui->label_AsyFinishPropsR->setText(propsByte);
		m_progressDialog->onFinishAsync();
		qDebug() << "type" << type << "\n" << "props" << propsByte << endl;
	}
	else if (majorCmd == QStringLiteral("progress")) {
		int  value = 0;
		memcpy(&value, data.constData(), data.size());
		m_progressDialog->onProgress(value);
	}
	else if (majorCmd == QStringLiteral("scan")) {
			if (minorCmd == QStringLiteral("framerate")) {
				int num = 0;
				memcpy(&num, data.constData(), data.size());

				ui->label_frameRateR->setText(QString::number(num));
			}
			if (minorCmd == QStringLiteral("pointCount")) {
				int num = 0;
				memcpy(&num, data.constData(), data.size());
				ui->label_pointCountR->setText(QString::number(num));
			}
			if (minorCmd == QStringLiteral("markerCount")) {
				int num = 0;
				memcpy(&num, data.constData(), data.size());
				ui->label_markerCountR->setText(QString::number(num));
			}
			if (minorCmd == QStringLiteral("triangleCount")) {
				int num = 0;
				memcpy(&num, data.constData(), data.size());
				ui->label_triangleCountR->setText(QString::number(num));
			}



			
		
			if (minorCmd == QStringLiteral("status")) {
				ui->label_scanStatusR->setText(data);
			}
			if (minorCmd == QStringLiteral("dist")) {
				int num = 0;
				memcpy(&num, data.constData(), data.size());
				ui->label_scandistR->setText(QString::number(num));
			}
			
			
			if (minorCmd == QStringLiteral("noMarkerDetected")) {
				bool valBool = 0;
				memcpy(&valBool, data.constData(), data.size());
				ui->checkBox_noMarkerDetected->setChecked(valBool);
			}
			if (minorCmd == QStringLiteral("tooFlat")) {
				bool valBool = 0;
				memcpy(&valBool, data.constData(), data.size());
				ui->checkBox_tooFlat->setChecked(valBool);
			}
			if (minorCmd == QStringLiteral("trackLost")) {
				bool valBool = 0;
				memcpy(&valBool, data.constData(), data.size());
				ui->checkBox_trackLost->setChecked(valBool);
			}
	}
}

void MainWindow::onVideoImageReady(int camID, QPixmap pixmap)
{
	if (camID == 0){
		ui->label_Cam0->setScaledContents(true);
		ui->label_Cam0->setPixmap(pixmap);
	}
	else if (camID == 1){
		ui->label_Cam1->setScaledContents(true);
		ui->label_Cam1->setPixmap(pixmap);
	}
}

bool MainWindow::sendData(void* socket, const QString& cmd, const QByteArray& data)
{
	int nbytes = 0;

	auto envelop = ("v1.0/" + cmd).toLocal8Bit();
	if (data != ""){
		nbytes = zmq_send(socket, envelop.constData(), envelop.size(), ZMQ_SNDMORE);
		if (nbytes != envelop.size()){
			qCritical() << "Send envelop error! nbytes:" << nbytes;
			return false;
		}
		nbytes = zmq_send(socket, data.constData(), data.size(), 0);
		if (nbytes != data.size()){
			qCritical() << "Send data error!"
				<< " nbytes:" << nbytes
				<< " data size:" << data.size()
				<< " data : " << data;

			return false;
		}
	}
	else{
		nbytes = zmq_send(socket, envelop.constData(), envelop.size(), 0);
		if (nbytes != envelop.size()){
			qCritical() << "Send envelop error! nbytes:" << nbytes;
			return false;
		}
	}

	char buf[MAX_DATA_LENGTH + 1] = { 0 };
	nbytes = zmq_recv(socket, buf, MAX_DATA_LENGTH, 0);
	return true;
}



