#pragma once



struct Buffer
{
	BYTE*	 m_pBuffer;
	UINT	 m_nBufferSize;

	BYTE*	 m_pData;
	UINT	 m_nDataSize;
	
	Buffer()
	{
		m_pBuffer = NULL;
		m_nBufferSize = 0;

		m_pData = NULL;
		m_nDataSize = 0;
	};

	virtual ~Buffer()
	{
		FreeBuffer();
	};

	virtual void FreeBuffer()
	{
		if (m_pBuffer == NULL)
			return;

		delete[] m_pBuffer;
		
		m_pBuffer = NULL;
		m_nBufferSize = 0;

		m_pData = NULL;
		m_nDataSize = 0;
	};

	virtual BOOL AllocateBuffer(UINT nBufferSize)
	{
		if ( nBufferSize == 0)
			return FALSE;

		FreeBuffer();
		
		m_pBuffer = new BYTE[nBufferSize];
		if ( m_pBuffer == NULL)
			return FALSE;
		m_nBufferSize = nBufferSize;

		m_pData = m_pBuffer;

		return true;
	};

	virtual BOOL ExtendBuffer(UINT nBufferSize)
	{
		if (nBufferSize <= m_nBufferSize) 
			return TRUE;

		return AllocateBuffer(nBufferSize);
	};

	virtual BOOL FillData(PBYTE pData, UINT nDataSize)
	{
		if (pData == NULL || nDataSize == 0) 
			return FALSE;

		if (m_nBufferSize < nDataSize)
			AllocateBuffer(nDataSize);
		
		memcpy(m_pBuffer, pData, nDataSize);
		m_nDataSize = nDataSize;

		return TRUE;
	};

	virtual BOOL AppendData(PBYTE pData, UINT nDataSize)
	{
		if (pData == NULL || nDataSize == 0) 
			return FALSE;

		if (!m_pBuffer && !m_nBufferSize)
			AllocateBuffer(nDataSize);

		if ( ( m_nBufferSize - ( m_pData - m_pBuffer)) < ( m_nDataSize + nDataSize))
		{
			PBYTE pBuffer = NULL;
			pBuffer = new BYTE[m_nDataSize + nDataSize];
			if ( pBuffer == NULL)
				return FALSE;

			if (m_nDataSize)
			{
				memcpy(pBuffer,m_pBuffer,m_nDataSize);
			}

			if ( m_pBuffer != NULL)
				delete [] m_pBuffer;

			m_pBuffer = pBuffer;
			m_pData = pBuffer;			
			m_nBufferSize = m_nDataSize + nDataSize;			
		}		
		memcpy( m_pData + m_nDataSize, pData, nDataSize);
		m_nDataSize += nDataSize;
		return TRUE;
	};

	virtual void ClearData()
	{
		m_nDataSize = 0;
		m_pData = m_pBuffer;
	};
};
class CDataBuffer
{
public:
	CDataBuffer(void);
	~CDataBuffer(void);

	void WriteData(void* lpData,int nLength);
	BOOL GetData(Buffer& dataBuf);
	DWORD GetDataCount();
	void ClearBuffer();
	void* GetHead();
	void RemoveHead();
	void ReleaseBuffer();
	CCriticalSection* GetCS()
	{
		return &m_cs;
	}
protected:
	Buffer* GetUnuseBuffer(int len);
private:
	list<Buffer*> m_listData;
	list<Buffer*> m_listUnuse;
	CCriticalSection m_cs;
	
};

