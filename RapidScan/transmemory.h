#ifndef TRANSMEMORY_H
#define TRANSMEMORY_H
#include <QSharedMemory>

struct MemoryMessage
{
	enum DataType
	{
		PointerVertex = 1,
		PointerNormal,
		PointerColor,
		faceID,
		texture,
		textureID,
		textureUV,
		pointNum,
		sendEnd
	};

	bool haveNext; //judge whether data is empty
	int size;
	int begin; //the begin position int the shared memory
	int end; ////the end position int the shared memory
	DataType type;
	int variable;
};

typedef MemoryMessage::DataType mesType;
class TransMemory
{
public:
	explicit TransMemory(QString key){ m_Memory.setKey(key); }
	QSharedMemory& operator()(){ return m_Memory; }

	void writeData(mesType type, void *data, int writeSize, int vectorSize, int variable = 0);
	MemoryMessage* readMessageData();
	int getSurplusSpace();
	bool writeEnd();
	bool fullMemory();
	void clearMemoryPos();
	void setVariable(int _variable){ m_proMess->variable = _variable; }

protected:
	void writeMessage(mesType type, int size, int variable = 0);

private:
	int m_shaderBufferPos = 0;
	MemoryMessage *m_proMess = nullptr;
	QSharedMemory  m_Memory;
};

#endif // TRANSMEMORY_H