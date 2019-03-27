#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <QObject>
#include <zmq.h>
//Subscribe with ZMQ
class MainWindow;
class Subscriber : public QObject
{
    Q_OBJECT
public:
    explicit Subscriber(MainWindow* mainWindow, void* context, QObject *parent = nullptr);

signals:
    void heartbeat();
	/*
	majorCmd: main command
	minorCmd:secondary command
	data:Refer to SDK document,the data is different
	*/
    void publishReceived(QString majorCmd, QString minorCmd, QByteArray data);
public slots:
	/*
	addr:through this addr,the connection of ZMQ is established.
	*/
    void setup(QString addr);
private:
    QString m_addr;
    void* m_context = nullptr;
    void* m_socket = nullptr;
    MainWindow* m_mainWindow = nullptr;
};

#endif // SUBSCRIBER_H
