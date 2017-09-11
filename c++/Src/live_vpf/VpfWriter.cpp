#include "VpfWriter.h"
#include "Writelog.h"
#include "ConsoleUI.h"
#include "xsystem.h"
#include "file.h"

#define MAX_UINT32	(uint32_t)(~0)

CVpfWriter::CVpfWriter(void)
	: m_vpfFile(NULL)
	, m_strPath()
	, m_bHasError(false)
	, m_bWorking(false)
	, m_utcBegin(std::utc())

	, m_byCache(std::nothrow)
	, m_bInnerIndex(true)
	, m_uLast10s(-1)
	, m_uLast1s(-1)
	, m_ll10sOffset(-1LL)
	, m_ll1sOffset(-1LL)

	, m_llLastOffset(-1LL)
	, m_llBaseOffset(0LL)
	, m_bHasHeader(false)
	, m_uFirstTC(MAX_UINT32)
	, m_uLastTC(0)
	, m_byIndexes(std::nothrow, 1024 * 4) {
}
CVpfWriter::~CVpfWriter(void) {
	CloseFile();
}

void CVpfWriter::OnSetError() {
	m_bWorking = false;
	m_bHasError = true;
	::fclose(m_vpfFile);
	m_vpfFile = NULL;
	wle(_T("OnSetErrorT(): %s"), m_strDesc.c_str());
}
bool CVpfWriter::OpenFile(LPCTSTR file, const uint64_t& utcBegin /* = ~0 */) {
	if (!m_byCache.EnsureSize(20 * 1024)) {
		return SetErrorT(false, _T("Alloc cache failed."));
	}
	if (!m_byIndexes.EnsureSize(20 * 1024)) {
		return SetErrorT(false, _T("Alloc cache failed."));
	}
	m_vpfFile = ::_tfopen(file, _T("wb+"));
	if (m_vpfFile == NULL) {
		return SetErrorT(false, _T("Open file failed."));
	}
	m_bWorking = true;
	m_strName = file::filename(file);
	m_strPath = file;
	if (utcBegin == MAX_UINT32) m_utcBegin = std::utc();
	else m_utcBegin = utcBegin;
	return true;
}

bool CVpfWriter::WriteHead(const uint8_t* const lpData, const int szData) {
	if (!m_bWorking)return false;
	if (m_bHasError)return false;
	if (m_bHasHeader)return false;
	return WriteHeader(lpData, szData);
}
bool CVpfWriter::WriteData(const uint8_t btType, const uint8_t* const lpData, const int szData, const uint32_t uTimecode /* = uint32_t_MAX */) {
	if (!m_bWorking)return false;
	if (m_bHasError)return false;
	if (!m_bHasHeader)return false;
	if (m_uFirstTC == MAX_UINT32) {
		m_uFirstTC = uTimecode;
	}
	if (uTimecode != MAX_UINT32) {
		m_uLastTC = uTimecode;
		TCSecond uSecond(uTimecode / 1000);
		if (m_uLast10s < 0) {
			m_uLast10s = uSecond;
			if (m_bInnerIndex)Write10sIndex(uTimecode);
		}
		if (uSecond - m_uLast10s >= 10) {
			if (m_bInnerIndex)Write10sIndex(uTimecode);
			m_uLast10s = uSecond;
		}
		if (m_uLast1s < 0) {
			m_uLast1s = uSecond;
			if (m_bInnerIndex)Write1sIndex(uTimecode);
		}
		if (uSecond > m_uLast1s) {
			AppendIndex(uSecond);
			if (m_bInnerIndex)Write1sIndex(uTimecode);
			m_uLast1s = uSecond;
		}
	}
	return WriteMediaData(btType, uTimecode, lpData, szData);
}
uint32_t CVpfWriter::GetLength() const {
	return m_uLastTC - m_uFirstTC;
}

void CVpfWriter::CloseFile(bool* isValid /* = NULL */) {
	WriteFooter();
	bool bIsValid(!m_bHasError && (m_byIndexes.GetSize() > 0));
	if (!bIsValid)::_tremove(m_strPath);
	if (isValid)*isValid = bIsValid;
}
LPCTSTR CVpfWriter::GetInfo(uint64_t& utcBegin, uint64_t& utcEnd) {
	uint32_t duration(m_uLastTC - m_uFirstTC);
	utcBegin = m_utcBegin;
	utcEnd = utcBegin + duration;
	return m_strPath;
}

bool CVpfWriter::Write10sIndex(const uint32_t uNextTC) {
	/************************************************************************
	*3�ֽ�			�鳤�ȣ��������������ֽڣ�
	*1�ֽ�			������(253)
	*4�ֽ�		��һ������ƫ��������Ա���ͷ����
	*4�ֽ�		��һ��ʱ���
	*4�ֽ�		��һ������ƫ��������Ա���ͷ����
	*4�ֽ�			�����ܳ���
	************************************************************************/
	try {
		static const uint32_t uZero(0);
		static const uint32_t u10sFirstByte((1 + (4 + 4 + 4) + 4) | (253 << 24));
		static const uint32_t u10sTotalLen((3 + 1) + (4 + 4 + 4) + 4);
		const long llCurOffset(::ftell(m_vpfFile));
		const int nLastOffset(m_ll10sOffset < 0 ? -1 : (int)(llCurOffset - m_ll10sOffset));
		if (m_ll10sOffset > 0) {
			::fseek(m_vpfFile, m_ll10sOffset + (3 + 1 + 4 + 4), SEEK_SET);
			::fwrite(&nLastOffset, 4, 1, m_vpfFile);
			::fseek(m_vpfFile, llCurOffset, SEEK_SET);
		}
		m_ll10sOffset = llCurOffset;
		m_byCache.Clear();
		m_byCache.AppendData(&u10sFirstByte, 4);	//�鳤��&������
		m_byCache.AppendData(&nLastOffset, 4);		//��һ������ƫ����
		m_byCache.AppendData(&uNextTC, 4);			//��һ��ʱ���
		m_byCache.AppendData(&uZero, 4);			//��һ������ƫ����
		m_byCache.AppendData(&u10sTotalLen, 4);		//�����ܳ���		
		::fwrite(m_byCache, m_byCache, 1, m_vpfFile);
		cpm(_T("[%s] д��������[%.1f@0x%llx]"), m_strName.c_str(), uNextTC / 1000.0, llCurOffset);
	} catch (...) {
		return SetErrorT(false, _T("Write index failed."));
	}
	return true;
}

bool CVpfWriter::Write1sIndex(const uint32_t uNextTC) {
	/************************************************************************
	*3�ֽ�			�鳤�ȣ��������������ֽڣ�
	*1�ֽ�			������(254)
	*4�ֽ�		��һ������ƫ��������Ա���ͷ����
	*4�ֽ�		��һ��ʱ���
	*4�ֽ�		��һ������ƫ��������Ա���ͷ����
	*4�ֽ�			�����ܳ���
	************************************************************************/
	try {
		static const uint32_t uZero(0);
		static const uint32_t u1sFirstByte((1 + (4 + 4 + 4) + 4) | (254 << 24));
		static const uint32_t u1sTotalLen((3 + 1) + (4 + 4 + 4) + 4);
		const long llCurOffset(::ftell(m_vpfFile));
		const int nLastOffset(m_ll1sOffset < 0 ? -1 : (int)(llCurOffset - m_ll1sOffset));
		if (m_ll1sOffset > 0) {
			::fseek(m_vpfFile, m_ll1sOffset + (3 + 1 + 4 + 4), SEEK_SET);
			::fwrite(&nLastOffset, 4, 1, m_vpfFile);
			::fseek(m_vpfFile, llCurOffset, SEEK_SET);
		}
		m_ll1sOffset = llCurOffset;
		m_byCache.Clear();
		m_byCache.AppendData(&u1sFirstByte, 4);		//�鳤��&������
		m_byCache.AppendData(&nLastOffset, 4);		//��һ������ƫ����
		m_byCache.AppendData(&uNextTC, 4);			//��һ��ʱ���
		m_byCache.AppendData(&uZero, 4);			//��һ������ƫ����
		m_byCache.AppendData(&u1sTotalLen, 4);		//�����ܳ���		
		::fwrite(m_byCache, m_byCache, 1, m_vpfFile);
	} catch (...) {
		return SetErrorT(false, _T("Write index failed."));
	}
	return true;
}

bool CVpfWriter::WriteMediaData(const uint8_t btType, const uint32_t uTimecode, const uint8_t* const lpData, const int szData) {
	/************************************************************************
	*3�ֽ�			�鳤�ȣ��������������ֽڣ�
	*1�ֽ�			������
	*4�ֽ�		ʱ���
	*N�ֽ�		ý������
	*4�ֽ�			�����ܳ���
	************************************************************************/
	try {
		uint32_t uFirstByte((1 + 4 + szData + 4) | (btType << 24));
		uint32_t uTotalLen(3 + 1 + 4 + szData + 4);
		if (!m_byCache.EnsureSize(uTotalLen))return false;
		m_byCache.Clear();
		m_byCache.AppendData(&uFirstByte, 4);		//�鳤��&������
		m_byCache.AppendData(&uTimecode, 4);		//ʱ���
		m_byCache.AppendData(lpData, szData);		//ý������
		m_byCache.AppendData(&uTotalLen, 4);		//�����ܳ���
		::fwrite(m_byCache.GetData(), m_byCache.GetSize(), 1, m_vpfFile);
	} catch (...) {
		return SetErrorT(false, _T("Write media data block failed."));
	}
	return true;
}
bool CVpfWriter::WriteHeader(const uint8_t* const lpData, const int szData) {
	/************************************************************************
	*�ļ�ͷ
	*8�ֽ�			β������ƫ����
	*4�ֽ�			��ʼʱ���(ms)
	*4�ֽ�			��ĩʱ���(ms)
	*4�ֽ�			ͷ������
	*N�ֽ�			ͷ��������
	************************************************************************/
	const static long	ullZero(0LL);
	try {
		::fwrite(&ullZero, 8, 1, m_vpfFile);		//β������ƫ����
		::fwrite(&ullZero, 8, 1, m_vpfFile);		//��ʼʱ���&��ĩʱ���
		::fwrite(&szData, 4, 1, m_vpfFile);			//ͷ������
		::fwrite(lpData, szData, 1, m_vpfFile);		//ͷ����
	} catch (...) {
		return SetErrorT(false, _T("Write header failed."));
	}
	m_llBaseOffset = ::ftell(m_vpfFile);
	m_llLastOffset = -1;
	cpm(_T("[%s] д���ļ�ͷ��ɣ�����ַ[0x%llx]"), m_strName.c_str(), m_llBaseOffset);
	m_bHasHeader = true;
	return true;
}
bool CVpfWriter::AppendIndex(const TCSecond nextTC) {
	uint32_t uLast1s(m_uLast1s < 0 ? nextTC : m_uLast1s);
	long llOffset(::ftell(m_vpfFile));
	long llLastOffset(m_llLastOffset < 0 ? m_llBaseOffset : m_llLastOffset);
	if (nextTC - uLast1s > 255)return SetErrorT(true, _T("uNextTC(%d) - uLast1s(%d) > 255"), nextTC, uLast1s);
	if (llOffset - llLastOffset > 0x00ffffff)return SetErrorT(true, L"llOffset(%lld) - llLastOffset(%lld) > 0x00ffffff", llOffset, llLastOffset);
	const uint8_t bGrow((uint8_t)(nextTC - uLast1s));
	const uint32_t uGrow((uint32_t)(llOffset - llLastOffset));
	const uint32_t uValue((uGrow << 8) | bGrow);
	if (m_byIndexes.AppendData(&uValue, 4) == 0)return false;
	m_llLastOffset = llOffset;
	return true;
}
bool CVpfWriter::WriteFooter() {
	/************************************************************************
	*β��
	*3�ֽ�			�鳤�ȣ��������������ֽڣ�
	*1�ֽ�			������(255)
	*β������
	*4�ֽ�		��һ��ʱ���
	*4�ֽ�		���һ��ʱ���
	*8�ֽ�		��һ���������Ӧ�ľ���ƫ����
	*�������б�
	*1�ֽ�	��ֵ����ֵ��������ֵ��
	*3�ֽ�	��ֵ����֡ƫ������������ֵ��
	************************************************************************/
	if (!m_bWorking)return false;
	try {
		long llCurOffset(::ftell(m_vpfFile));
		uint32_t uFirstByte((1 + 4 + 4 + 8 + m_byIndexes.GetSize()) | (255 << 24));
		::fwrite(&uFirstByte, 4, 1, m_vpfFile);			//�鳤��&������
		::fwrite(&m_uFirstTC, 4, 1, m_vpfFile);			//��һ��ʱ���
		::fwrite(&m_uLastTC, 4, 1, m_vpfFile);			//���һ��ʱ���
		::fwrite(&m_llBaseOffset, 8, 1, m_vpfFile);		//��һ���������Ӧ�ľ���ƫ����
		::fwrite(m_byIndexes.GetData(), m_byIndexes.GetSize(), 1, m_vpfFile);	//�������б�

		::fseek(m_vpfFile, 0, SEEK_SET);
		::fwrite(&llCurOffset, 8, 1, m_vpfFile);
		::fwrite(&m_uFirstTC, 4, 1, m_vpfFile);
		::fwrite(&m_uLastTC, 4, 1, m_vpfFile);

		::fclose(m_vpfFile);
		m_vpfFile = NULL;
		m_bWorking = false;
		cpm(_T("[%s] д��β��������ɣ������ܳ�[%dBytes]"), m_strName.c_str(), (int)m_byIndexes);
	} catch (...) {
		return SetErrorT(false, _T("Write footer failed."));
	}
	return true;
}

IVpfWriter* CreateVpfWriter() {
	return new CVpfWriter;
}