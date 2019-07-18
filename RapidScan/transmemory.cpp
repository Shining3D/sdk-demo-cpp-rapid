#include "transmemory.h"
#include <QDebug>

void TransMemory::writeData(mesType type, void *data, int writeSize, int vectorSize, int variable /* = 0 */)
{
	char *Memory = (char*)m_Memory.data();
	writeMessage(type, vectorSize, variable);
	memcpy(Memory + m_shaderBufferPos, data, writeSize);
	m_shaderBufferPos += writeSize;
	m_proMess->end = m_shaderBufferPos;
}

bool TransMemory::fullMemory()
{
	return (sizeof(MemoryMessage) > (m_Memory.size() - m_shaderBufferPos)) ? false : true;
}

int TransMemory::getSurplusSpace()
{
	return m_Memory.size() - m_shaderBufferPos - 2 * sizeof(MemoryMessage);
}

MemoryMessage* TransMemory::readMessageData()
{
	return nullptr == m_proMess ? m_proMess = (MemoryMessage*)m_Memory.data() : m_proMess = (MemoryMessage*)((char*)m_Memory.data() + m_proMess->end);
}

void TransMemory::writeMessage(mesType type, int size, int variable /* = 0 */)
{
	char *data = (char*)m_Memory.data();
	if (nullptr != m_proMess){
		m_proMess->haveNext = true;
	}

	MemoryMessage  tmpMesss = {false, size, 0, 0, type, variable};
	memcpy(data + m_shaderBufferPos, &tmpMesss, sizeof(MemoryMessage));
	m_proMess = (MemoryMessage*)(data + m_shaderBufferPos);
	m_shaderBufferPos += sizeof(MemoryMessage);
	m_proMess->begin = m_shaderBufferPos;
}

bool TransMemory::writeEnd()
{
	if (sizeof(MemoryMessage) > (m_Memory.size() - m_shaderBufferPos)){
		return false;
	}

	writeMessage(MemoryMessage::sendEnd, 0);
	m_proMess->begin = m_proMess->end = 0;
	clearMemoryPos();
	return true;
}

void TransMemory::clearMemoryPos()
{
	m_proMess = nullptr;
	m_shaderBufferPos = 0;
}
