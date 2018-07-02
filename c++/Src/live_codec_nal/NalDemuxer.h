#pragma once
#include <mutex>
#include "HttpClient.h"
#include "CodecInterface.h"
#include "json/value.h"
#include "xstring.h"
#include "Byte.h"
#include "xsystem.h"

/**
* ͷ��ʽ��  
* 1�ֽ�����Ƶ��ʶ�� V��Ƶ  A��Ƶ
* 4�ֽ�����Ƶ���룺 ' CVA' ' CAA' ' 3PM'
* 2�ֽ����ÿ鳤�ȣ� 
* N�ֽ����ÿ�����
* ѭ��

* ֡��ʽ��
* 4�ֽ�ʱ���
* N�ֽ����ݣ�H264��4�ֽ�ǰ��0x00000001��
************************************************************************/

struct event;
struct event_base;
class CHttpClient;
class CNalDemuxer : public IDecodeProxy {
public:
	CNalDemuxer(IDecodeProxyCallback* callback);
	~CNalDemuxer();

protected:
	virtual bool	Start();
	virtual bool	CrossThread() { return false; }
	virtual bool	AddHeader(const uint8_t* const data, const int size);
	virtual bool	AddVideo(const uint8_t* const data, const int size);
	virtual bool	AddAudio(const uint8_t* const data, const int size);
	virtual bool	AddCustom(const uint8_t* const data, const int size);
	virtual void	Stop();
	virtual bool	Discard();

private:
	IDecodeProxyCallback* m_lpCallback;
};

