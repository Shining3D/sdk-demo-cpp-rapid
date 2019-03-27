#include "DataProcesser.h"
#include <cassert>
#include "mainwindow.h"
#include <QtDebug>
#include <QSharedMemory>
#include <QMessageBox>
DataProcesser::DataProcesser(MainWindow *mainWindow, void *context, QObject *parent)
	: QObject(parent), m_mainWindow(mainWindow), m_context(context)
{

}

void DataProcesser::setup(int port)//12000
{
	m_socket = zmq_socket(m_context, ZMQ_REP);

	auto bindAddrBytes = QString("tcp://*:%1").arg(port).toLocal8Bit();
	int rc = zmq_bind(m_socket, bindAddrBytes);
	qDebug() << "rc" << rc << endl;
	qDebug() << "bindAddrBytes" << bindAddrBytes << endl;
	assert(!rc);
	const char * envelop = "v1.0/scan/register";
	auto nbytes = zmq_send(m_reqSocket, envelop, strlen(envelop), ZMQ_SNDMORE);
	if (nbytes != strlen(envelop)){
		qWarning() << "cannot send register envelop!";
		return;
	}
	auto connectAddrBytes = QString("tcp://localhost:%1").arg(port).toLocal8Bit();
	//qDebug() << connectAddrBytes << endl;
	nbytes = zmq_send(m_reqSocket, connectAddrBytes.constData(), connectAddrBytes.size(), 0);
	if (nbytes != connectAddrBytes.size()){
		qWarning() << "cannot send register processurl!";
		return;
	}

	char replybuf[MAX_DATA_LENGTH + 1] = { 0 };
	zmq_recv(m_reqSocket, replybuf, MAX_DATA_LENGTH, 0);

	while (true){
		char rawData[MAX_DATA_LENGTH + 1] = { 0 };
		nbytes = zmq_recv(m_socket, rawData, sizeof(rawData), 0);
		QByteArray data(rawData);
		auto jsonDoc = QJsonDocument::fromJson(data);
		if (jsonDoc.isNull()){
			qWarning() << "Invalid data processing json message!";
			continue;
		}
		auto jsonObj = jsonDoc.object();
		//qInfo() << "data process:" << jsonObj;
		QJsonObject backJsonObj{
			{ QStringLiteral("handled"), true }
		};
		auto backJsonObjStr = MainWindow::jsonStr(backJsonObj);
		processData(jsonObj);
		nbytes = zmq_send(m_socket, backJsonObjStr, backJsonObjStr.size(), 0);
	}
}

void DataProcesser::processData(QJsonObject jsonObj)
{
	auto type = jsonObj["type"].toString();
	auto key = jsonObj["key"].toString();
	auto name = jsonObj["name"].toString();
	auto props = jsonObj["props"].toObject();
	auto offset = jsonObj["offset"].toInt();

	QSharedMemory shm;
	shm.setNativeKey(key);
	shm.attach(QSharedMemory::ReadWrite);
	auto data = static_cast<unsigned char*>(shm.data()) + offset;

	QJsonDocument jsonDoc;
	jsonDoc.setObject(props);
	auto msg = jsonDoc.toJson();

	if (type == QStringLiteral("MT_VIDEO_DATA")){
		auto rotate = props["rotate"].toInt();
		auto width = props["width"].toInt();
		auto height = props["height"].toInt();
		auto channel = props["channel"].toInt();

		auto pixmap = createPixmap(data, width, height, channel, rotate);
		if (name == QStringLiteral("cam0")){
			emit videoImageReady(0, pixmap);
		}
		else if (name == QStringLiteral("cam1")){
			emit videoImageReady(1, pixmap);
		}
	}
	else if (type == QStringLiteral("MT_POINT_CLOUD")) {
		emit sharedMemoryMsg(type, msg);
	}
	else if (type == QStringLiteral("MY_DELETE_POINTS")) {
		emit sharedMemoryMsg(type, msg);
	}
	else if (type == QStringLiteral("MT_MARKERS")) {
		emit sharedMemoryMsg(type, msg);
	}
	else if (type == QStringLiteral("MY_TRI_MESH")) {
		emit sharedMemoryMsg(type, msg);
	}
	else if (type == QStringLiteral("MT_RANGE_DATA")) {
		emit sharedMemoryMsg(type, msg);
	}
}


QPixmap DataProcesser::createPixmap(unsigned char* data, int width, int height, int channel, int rotate)
{
	int sizeColor = width * height * 3;
	int sizeSingle = width * height * channel;
	int byte_per_line = 0;
	unsigned char* pDataColor = new unsigned char[sizeColor];
	if (3 == channel)
	{
		byte_per_line = width*channel;
		memcpy(pDataColor, data, sizeSingle);

	}
	else
	{
		for (int i = 0; i < sizeSingle; i++)
		{
			pDataColor[i * 3] = data[i];
			pDataColor[i * 3 + 1] = data[i];
			pDataColor[i * 3 + 2] = data[i];
			if (data[i] > 230)
			{
				pDataColor[i * 3] = 255;
				pDataColor[i * 3 + 1] = 0;
				pDataColor[i * 3 + 2] = 0;
			}
		}// ÏÔÊ¾ºìÉ« add by lzy
	}

	QMatrix left_matrix_;
	left_matrix_.rotate(rotate);


	QImage* image_ = 0;
	if (byte_per_line > 0)
		image_ = new QImage(pDataColor, width, height, byte_per_line, QImage::Format_RGB888);
	else
		image_ = new QImage(pDataColor, width, height, QImage::Format_RGB888);

	if (width != 1280)
		*image_ = image_->transformed(left_matrix_).mirrored(true); //sign 1121
	else
		*image_ = image_->transformed(left_matrix_); //sign 1121

	QPixmap pixmap = QPixmap::fromImage(*image_);

	delete image_;

	delete[]pDataColor;
	pDataColor = NULL;

	return pixmap;
}