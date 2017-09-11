#pragma once
#include <vector>
#include <mutex>
#include "os.h"
#include "IOAccess.h"
#include "xstring.h"
#include "SmartPtr.h"
#include "SmartArr.h"
#include "Byte.h"
#include "SmartErr.h"

/************************************************************************
*�ļ�ͷ
*8�ֽ�			β������ƫ����
*4�ֽ�			��ʼʱ���(ms)
*4�ֽ�			��ĩʱ���(ms)
*4�ֽ�			ͷ�����ȣ����������ĸ��ֽڣ�ͨ�����ֽڿ�����ת�����������ݿ飩
*N�ֽ�			ͷ��������
*���ݿ�
*3�ֽ�			�鳤�ȣ��������������ֽڣ�
*1�ֽ�			������
*<128		�û���������
*>=128		ϵͳ��������
*ϵͳ����
*255		��һ����ֵ����֡ƫ������12�ֽڣ�
*254		��һ��10��ֵ����֡ƫ������12�ֽڣ�
*4�ֽ�	��һ������ƫ��������Ա���ͷ����
*4�ֽ�	��һ��ʱ���
*4�ֽ�	��һ������ƫ��������Ա���ͷ����
*�û�����
*4�ֽ�		ʱ���
*N�ֽ�		ý������
*4�ֽ�			�����ܳ��ȣ��������ĸ��ֽڣ�
*	*	*	*	*	*	*	*	*	*
*β��
*4�ֽ�			��һ��ʱ���
*4�ֽ�			���һ��ʱ���
*8�ֽ�			��һ���������Ӧ�ľ���ƫ����
*�������б�
*1�ֽ�		��ֵ����ֵ��������ֵ��
*3�ֽ�		��ֵ����֡ƫ������������ֵ��
************************************************************************/
struct event_base;
class CVpfReader : public IVpfReader, public CSmartErr {
public:
	CVpfReader(bool adjustTC = true);
	~CVpfReader(void);

public:
	bool Open(LPCTSTR url, event_base* base);
	bool Open(LPCTSTR file);
	bool IsValid();
	bool IsEof();
	bool GetInfo(uint32_t& uBeginTC, uint32_t& uEndTC);
	bool GetHeader(uint8_t*& lpHeader, int& szHeader);
	bool ReadBody(uint8_t& byType, uint8_t*& lpData, int& szData, uint32_t& uTimecode);
	bool SeekTo(uint32_t uTimecode);
	bool Tell(uint32_t& uTimecode);
	void Close();

protected:
	virtual void OnSetError();
	virtual LPCTSTR	GetLastErrorDesc() const { return m_strDesc; }
	virtual unsigned int GetLastErrorCode() const { return m_dwCode; }

protected:
	bool OpenReader(bool adjustTC);
	bool ReadHeader();
	bool ReadIndexes();
	bool ReadBlock();

protected:
	typedef int TCSecond;
	struct INDEXENTRY {
		INDEXENTRY(TCSecond s, int64_t o) : timecode(s), offset(o) {}
		friend bool operator<(const INDEXENTRY& left, const TCSecond& right) { return left.timecode < right; }
		friend bool operator<(const TCSecond& left, const INDEXENTRY& right) { return left < right.timecode; }
		TCSecond timecode;
		int64_t offset;
	};
	typedef std::vector<INDEXENTRY> CIndexEntry;

private:
	CSmartPtr<IIOReader> m_spReader;
	xtstring m_strUrl;
	const bool m_bIsHttp;
	bool m_bAdjustTC;
	bool m_bHasError;

	std::mutex m_lkDatas;
	int64_t m_llIndexes;
	CIndexEntry m_vIndexes;
	CByte m_byHeader;
	uint32_t m_uBeginTC;
	uint32_t m_uEndTC;
	uint32_t m_uReadTC;

	uint64_t m_llBodies;
	CSmartArr<uint8_t> m_saCache;
	int m_szRemain;
	int m_szBlock;
	bool m_bEof;
};

