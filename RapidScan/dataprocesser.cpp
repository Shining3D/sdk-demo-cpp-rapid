#include "DataProcesser.h"
#include <cassert>
#include "mainwindow.h"
#include <QtDebug>
#include <QSharedMemory>
#include <QMessageBox>
#include <QCoreApplication>
TransMemory shareMemory("meshData1");

DataProcesser::DataProcesser(MainWindow *mainWindow, void *context, QObject *parent)
	: QObject(parent), m_mainWindow(mainWindow), m_context(context)
{

}

void DataProcesser::handleMeshData(QString key)
{
	if (!shareMemory().isAttached()){
		if (!shareMemory().attach(QSharedMemory::ReadWrite)){
			return;
		}
	}

	shareMemory().lock();
	MemoryMessage* sharedMes;
	do 
	{
		sharedMes = shareMemory.readMessageData();

		switch (sharedMes->type)
		{
		case MemoryMessage::PointerVertex:
		{
			qDebug() << "handle PointerVertex data";
			handlePointVertex(sharedMes, shareMemory);
		}break;
		case MemoryMessage::PointerNormal:
		{
			qDebug() << "handle PointerNormal data";
		}break;
		case MemoryMessage::faceID:
		{
			qDebug() << "handle faceID data";
		}break;
		case  MemoryMessage::texture:
		{
			qDebug() << "handle texture data";
			handleTexture(sharedMes, shareMemory);
		}break;
		case MemoryMessage::textureID:
		{
			qDebug() << "handle textureID data";
		}break;
		case MemoryMessage::textureUV:
		{
			qDebug() << "handle textureUV data";
		}break;

		default:
			break;
		}
	} while (sharedMes->haveNext);

	shareMemory.clearMemoryPos();
	shareMemory().unlock();
}

void DataProcesser::handlePointVertex(MemoryMessage *pMes, TransMemory &memory)
{
	//get the initial data address;
	float *data = (float*)((char*)memory().data() + pMes->begin);
	//get the data count
	int count = (pMes->end - pMes->begin)/(sizeof(float));
	//to do what you want

}

void DataProcesser::handleTexture(MemoryMessage *pMes, TransMemory &memory)
{
	static int pos; // record texture data offset
	uchar *pTextureImage = nullptr; //save texture data
	int textureWidth = pMes->variable;
	int textureHeight = pMes->size;
	if (nullptr == pTextureImage){
		pTextureImage = new uchar[textureHeight * textureWidth * sizeof(uchar) * 3];
	}

	//get the data count
	int count = (pMes->end - pMes->begin) / (sizeof(uchar));
	//get the initial data address;
	float *data = (float*)((char*)memory().data() + pMes->begin);
	memcpy(pTextureImage + pos, data, count);
	pos += count;
	
	//judge whether texture data is transported completely.
	if (textureWidth * textureHeight * 3){
		//construct your texture image
	}
}

//////////////////////////////////////////////////////////////////////
void DataProcesser::setup(int port)//12000    在invokeMeathod方法里有用的
{
	//note: you should call "zmq_close" again when you want to rebuild zmq connection. 
	if (m_socket != nullptr){
		zmq_close(m_socket);
	}
	//.end

	m_socket = zmq_socket(m_context, ZMQ_REP);//创建套接字

	auto bindAddrBytes = QString("tcp://*:%1").arg(port).toLocal8Bit();
	int rc = zmq_bind(m_socket, bindAddrBytes);//套接字与终结点绑定
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
			break;//use break instead of continue 
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

void pointCloudHandle::savePointToFile(QString filename, int pointcount, QString memKey, int offset)
{
	QMutexLocker mutex(&m_mutex);

	QSharedMemory shm;//一段共享内存
	shm.setNativeKey(memKey);//多进程或线程使用同一共享内存时  key值必须相同
	shm.attach(QSharedMemory::ReadWrite);
	auto data = static_cast<unsigned char*>(shm.data()) + offset;

	const float *pPointCloud = (const float*)data;
	QFile file_scan(filename);
	if (!file_scan.open(QFile::WriteOnly | QFile::Text))
	{
		return;
	}

	QString tst_strPointData = "";
	for (int i = 0; i < pointcount * 6;)
	{
		tst_strPointData = QString::number((float)pPointCloud[i], 'f') + "  " + QString::number((float)pPointCloud[i + 1], 'f')
			+ "  " + QString::number(pPointCloud[i + 2], 'f') + "  " //postion data x y z
			+ QString::number((float)pPointCloud[i + 3], 'f') + "  " + QString::number((float)pPointCloud[i + 4], 'f')
			+ "  " + QString::number((float)pPointCloud[i + 5], 'f') + "\n";

		qDebug() << " tst_strPointData: " << tst_strPointData;
		file_scan.write(tst_strPointData.toLocal8Bit());
		i += 6;
	}

	file_scan.close();
}

void DataProcesser::processData(QJsonObject jsonObj)
{
	auto type = jsonObj["type"].toString();
	auto key = jsonObj["key"].toString();
	auto name = jsonObj["name"].toString();
	auto props = jsonObj["props"].toObject();
	auto offset = jsonObj["offset"].toInt();

	auto pointcount = props["pointCount"].toInt();
	qDebug() << " pointcount: " << pointcount;

	QSharedMemory shm;//一段共享内存
	shm.setNativeKey(key);//多进程或线程使用同一共享内存时  key值必须相同
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

		//TODO: 需求定制，将每帧数据写入文件;
		static int num = 0; num++;

		void *p = shm.data();
		if(p == nullptr){
			qWarning() << "shared memory is null";
			return;
		}

		//auto data = static_cast<unsigned char*>(shm.data()) + offset;
		QString path = qApp->applicationDirPath() + "/" + QString::number(num).append(QStringLiteral("_pointcloud.txt"));
		m_vecThread.push_back(new std::thread(std::bind(&pointCloudHandle::savePointToFile, &m_pointObj, 
			path, pointcount, key, offset)));

#if DEBUG_POINTCLOUD
		//增加全局点云数据显示
		//note: 共享内存数据不上所有数据类型是char（有些是float），所以需要根据memtype进行对应转换
		//auto data = static_cast<unsigned char*>(shm.data()) + offset;
		void *p = shm.data();
		if (nullptr == p){
			qCritical() << "share memory Pointer is null\n";
			return;
		}

		//note: not consider texture
		auto data = static_cast<unsigned char*>(shm.data()) + offset;

		//for test float data
		float *p_data = (float*)data;
		QFile file_scan(m_strApp + "/scanService.txt");
		if (!file_scan.open(QFile::WriteOnly | QFile::Text))
		{
			return;
		}

		QString tst_strPointData = "";
		for (int i = 0; i < 1000;)
		{
			tst_strPointData = QString::number((float)p_data[i], 'f') + "  " + QString::number((float)p_data[i + 1], 'f')
				+ "  " + QString::number(p_data[i + 2], 'f') + "  " //postion data x y z
				+ QString::number((float)p_data[i + 3], 'f') + "  " + QString::number((float)p_data[i + 4], 'f')
				+ "  " + QString::number((float)p_data[i + 5], 'f') + "\n";

			file_scan.write(tst_strPointData.toLocal8Bit());
			//file.write(strNormalData.toLocal8Bit());
			i += 6;
		}

		file_scan.close();
		//.end

		if (m_bFirstPoint)
		{
			m_bFirstPoint = false;
			m_nFirstPointCount = pointcount;
			//first whole point byte size
			int data_len = pointcount * sizeof(float) * 6;

			memcpy(m_pPointCloud, data, data_len);
			float *p_PointClod = (float*)m_pPointCloud;
			//write init data to file
			//QString strApp = qApp->applicationDirPath();
			QFile file(m_strApp + "/origin.asc");
			if (!file.open(QFile::WriteOnly | QFile::Text))
			{
				return;
			}
			//if (file.open(QIODevice::WriteOnly))
			{
				QString strPointData = "";
				QString strNormalData = "";
				for (int i = 0; i < pointcount * 6;)
				{
					// 					strPointData = "point-x: " + QString::number(m_pPointCloud[i]) + " point-y: " + QString::number(m_pPointCloud[i + 1])
					// 						+ " point-z: " + QString::number(m_pPointCloud[i + 2]) + "\n";
					// 
					// 					strNormalData = "normal-x: " + QString::number(m_pPointCloud[i + 3]) + " normal-y: " + QString::number(m_pPointCloud[i + 4])
					// 						+ " normal-z: " + QString::number(m_pPointCloud[i + 5]) + "\n";

					//write file in .asc format
					strPointData = QString::number((float)p_PointClod[i], 'f') + "  " + QString::number((float)p_PointClod[i + 1], 'f')
						+ "  " + QString::number((float)p_PointClod[i + 2], 'f') + "  " //postion data x y z
						+ QString::number((float)p_PointClod[i + 3], 'f') + "  " + QString::number((float)p_PointClod[i + 4], 'f')
						+ "  " + QString::number((float)p_PointClod[i + 5], 'f') + "\n";

					file.write(strPointData.toLocal8Bit());
					//file.write(strNormalData.toLocal8Bit());
					i += 6;
				}

				file.close();
			}
		}
		else{
			//update one frame data for test to prove that the whole point cloud is different from current point cloud
			float *p_PointClod = (float*)m_pPointCloud;
			if (incremental && !m_bFistUpdate)
			{
				m_bFistUpdate = true;

				qDebug() << "point cloud update by id" << endl;
				//update use transmit pointcount
				for (int i = 0; i < pointcount * 6;)
				{
					//get id
					int id = data[i + 6];

					if (id >= strlen(m_pPointCloud)) { qDebug() << "point cloud id is error" << endl; break; }

					//update position
					m_pPointCloud[id] = data[i];
					m_pPointCloud[id + 1] = data[i + 1];
					m_pPointCloud[id + 2] = data[i + 2];
					//update normal
					m_pPointCloud[id + 3] = data[i + 3];
					m_pPointCloud[id + 4] = data[i + 4];
					m_pPointCloud[id + 5] = data[i + 5];

					i += 6;
				}

				//write update data to file
				//QString strApp = qApp->applicationDirPath();
				QFile file(m_strApp + "/update.asc");
				if (!file.open(QFile::WriteOnly | QFile::Text))
				{
					return;
				}

				//if (file.open(QIODevice::WriteOnly))
				{
					QString strPointData = "";
					QString strNormalData = "";
					//show by using the global pointcount
					for (int i = 0; i < m_nFirstPointCount * 6;)
					{
						// 						strPointData = "point-x: " + QString::number(m_pPointCloud[i]) + " point-y: " + QString::number(m_pPointCloud[i + 1])
						// 							+ " point-z: " + QString::number(m_pPointCloud[i + 2]) + "\n";
						// 
						// 						strNormalData = "normal-x: " + QString::number(m_pPointCloud[i + 3]) + " normal-y: " + QString::number(m_pPointCloud[i + 4])
						// 							+ " normal-z: " + QString::number(m_pPointCloud[i + 5])+ "\n";

						//write file in .asc format
						strPointData = QString::number((float)p_PointClod[i], 'f') + "  " + QString::number((float)p_PointClod[i + 1], 'f')
							+ "  " + QString::number((float)p_PointClod[i + 2], 'f') + "  " //postion data x y z
							+ QString::number((float)p_PointClod[i + 3], 'f') + "  " + QString::number((float)p_PointClod[i + 4], 'f')
							+ "  " + QString::number((float)p_PointClod[i + 5], 'f') + "\n";

						file.write(strPointData.toLocal8Bit());
						//file.write(strNormalData.toLocal8Bit());
						i += 6;
					}

					file.close();
				}
			}
			else{
				//新增的点云数据
			}
		}
#endif

	}
	else if (type == QStringLiteral("MY_DELETE_POINTS")) {
		emit sharedMemoryMsg(type, msg);
	}
	else if (type == QStringLiteral("MT_MARKERS")) {
		emit sharedMemoryMsg(type, msg);
	}
	else if (type == QStringLiteral("MT_TRI_MESH")) {
		emit sharedMemoryMsg(type, msg);
		//note: show example to how to parse mesh data lwj 2019.7.18
		//handleMeshData("meshData1");
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
		}// 显示红色 add by lzy
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