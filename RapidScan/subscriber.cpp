#include "subscriber.h"
#include <cassert>
#include "mainwindow.h"
#include <QtDebug>

Subscriber::Subscriber(MainWindow *mainWindow, void *context, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow), m_context(context)
{

}

void Subscriber::setup(QString addr)
{
	qDebug() << "on_pushButton_RegisterProcesser_clicked Subscriber:: URL:" << addr << endl;
    m_socket = zmq_socket(m_context, ZMQ_SUB);
    auto addrBytes = addr.toLocal8Bit();
	
    int rc = zmq_connect(m_socket, addrBytes.constData());
    assert(!rc);
    auto err = zmq_strerror(zmq_errno());

    char *filter = "v1.0";
    rc = zmq_setsockopt(m_socket, ZMQ_SUBSCRIBE, filter, strlen(filter));
    assert(!rc);
    err = zmq_strerror(zmq_errno());

    while (true)
    {
        QStringList cmds;
        
        char envelop[MAX_ENVELOPE_LENGTH + 1] = { 0 };
        auto nbytes = zmq_recv(m_socket, envelop, sizeof(envelop), 0);
        if (nbytes == -1){
            qWarning() << "Request 0 bytes!";
			auto errorNo = zmq_errno();
			if (errorNo == ETERM){
				qWarning() << "server is terminated!";
				break;
			}
			else
				continue;
        }
		

		cmds = QString(envelop).split('/');
		const auto ver = cmds.front();
		if (ver != QString(filter)){
			qWarning() << "not this version!";
			continue;
		}
		cmds.pop_front();
		assert(cmds.size() >= 1);
		const auto majorCmd = cmds.front();
		const auto minorCmd = cmds.size() > 1 ? cmds[1] : "";
		if (majorCmd == QStringLiteral("hb")){
			emit heartbeat();
		}
		else{
			char rawData[MAX_DATA_LENGTH + 1] = { 0 };
			nbytes = zmq_recv(m_socket, rawData, sizeof(rawData), 0);
			QByteArray data(rawData);
			emit publishReceived(majorCmd, minorCmd, data);
		}
    }
}
