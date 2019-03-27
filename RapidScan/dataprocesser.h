#ifndef DATA_PROCESSER_H
#define DATA_PROCESSER_H

#include <QObject>
#include <zmq.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>
#include <QByteArray>
/*
Get data from shared memory
*/
class MainWindow;
class DataProcesser : public QObject
{
    Q_OBJECT
public:
	explicit DataProcesser(MainWindow* mainWindow, void* context, QObject *parent = nullptr);

	void setReqSocket(void* s)
	{ m_reqSocket = s; }
signals:
	/*Send image data to mainwindow
	camID: image area displayed on the main interface
	pixmap:image data
	*/
	void videoImageReady(int camID, QPixmap pixmap);

	void sharedMemoryMsg(QString ,QByteArray);
public slots:
	/*
	port:Port number for registering shared memory
	Communicate with SDK through ZMQ to deal with shared memory.
	*/
    void setup(int port);
private:
	/*
	jsonObj:shared memory data
	Processing shared meory for specific situations
	*/
	void processData(QJsonObject jsonObj);
	/*
	data:The data of picture
	rotate£ºThe rotation angle of the picture
	*/
	QPixmap createPixmap(unsigned char* data, int width, int height, int channel, int rotate);
private:
    QString m_addr;
    void* m_context = nullptr;
    void* m_socket = nullptr;
	void* m_reqSocket = nullptr;
    MainWindow* m_mainWindow = nullptr;
};

#endif // DATA_PROCESSER_H
