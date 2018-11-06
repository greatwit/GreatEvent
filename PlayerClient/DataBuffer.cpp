#include "stdafx.h"
#include "DataBuffer.h"


CDataBuffer::CDataBuffer(void)
{
	
}

void CDataBuffer::ClearBuffer()
{
	CSingleLock sLock(&m_cs,TRUE);
	while(m_listData.size())
	{
		m_listUnuse.push_back(*m_listData.begin());
		m_listData.pop_front();
	}
}
void CDataBuffer::ReleaseBuffer()
{
	while(m_listData.size())
	{
		delete (*m_listData.begin());
		m_listData.pop_front();
	}
	while(m_listUnuse.size())
	{
		delete (*m_listUnuse.begin());
		m_listUnuse.pop_front();
	}
}
CDataBuffer::~CDataBuffer(void)
{
	ReleaseBuffer();
}
DWORD CDataBuffer::GetDataCount()
{
	CSingleLock sLock(&m_cs,TRUE);
	return m_listData.size();
}
void CDataBuffer::WriteData(void* lpData,int nLength)
{
	CSingleLock sLock(&m_cs,TRUE);
	Buffer* lpBuffer = GetUnuseBuffer(nLength);
	if(!lpBuffer)
	{
		return;
	}
	lpBuffer->ClearData();
	lpBuffer->AppendData((PBYTE)lpData,nLength);
	
	m_listData.push_back(lpBuffer);

}

Buffer* CDataBuffer::GetUnuseBuffer(int len)
{
	Buffer* lpBuffer = NULL;
	list<Buffer*>::iterator it = m_listUnuse.begin();
	if(it == m_listUnuse.end())
	{
		lpBuffer = new Buffer;
		if(!lpBuffer)
			return NULL;
		lpBuffer->ExtendBuffer((len + 1023) / 1024 * 1024);
		return lpBuffer;
	}
	list<Buffer*>::iterator itMax = m_listUnuse.end();
	list<Buffer*>::iterator itRet = m_listUnuse.end();

	for(;it != m_listUnuse.end();it++)
	{
		if( (*it)->m_nBufferSize >= len )
		{
			if( itRet ==  m_listUnuse.end() || (*itRet)->m_nBufferSize > (*it)->m_nBufferSize)
			{
				itRet = it;
			}
		}
		if(itMax ==  m_listUnuse.end() || (*itMax)->m_nBufferSize < (*it)->m_nBufferSize)
		{
			itMax = it;
		}
	}
	if(itRet != m_listUnuse.end())
	{
		lpBuffer = (*itRet);
		m_listUnuse.erase(itRet);
		return lpBuffer;
	}

	(*itMax)->ExtendBuffer((len + 1023) / 1024 * 1024);
	lpBuffer = (*itMax);
	m_listUnuse.erase(itMax);
	return lpBuffer;
}

BOOL CDataBuffer::GetData(Buffer& dataBuf)
{

	list<Buffer*>::iterator it = m_listData.begin();
	if(it == m_listData.end())
	{
		return FALSE;
	}
	dataBuf.ClearData();
	dataBuf.AppendData((PBYTE)(*it)->m_pData,(*it)->m_nDataSize);
	m_listUnuse.push_back(*it);
	m_listData.erase(it);
	return TRUE;
}


void* CDataBuffer::GetHead()
{
	CSingleLock sLock(&m_cs,TRUE);
	if (m_listData.empty())
	{
		return NULL;
	}
	return (*m_listData.begin())->m_pData;
}
void CDataBuffer::RemoveHead()
{
	CSingleLock sLock(&m_cs,TRUE);
	list<Buffer*>::iterator it = m_listData.begin();
	if(it == m_listData.end())
	{
		return;
	}
	m_listUnuse.push_back(*it);
	m_listData.erase(it);
}

