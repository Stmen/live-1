#pragma once
#include <map>
#include "os.h"
#include "xchar.h"

#ifndef MKFCC
#define MKFCC(a, b, c, d) (((uint32_t)a << 0) + ((uint32_t)b << 8) + ((uint32_t)c << 16) + ((uint32_t)d << 24))
#endif

interface ISourceProxyCallback {
	virtual ~ISourceProxyCallback() {}
	virtual void			OnHeaderData(const uint8_t* const data, const int size) = 0;
	virtual void			OnVideoData(const uint8_t* const data, const int size, const bool key = true) = 0;
	virtual void			OnAudioData(const uint8_t* const data, const int size) = 0;
	virtual void			OnCustomData(const uint8_t* const data, const int size) = 0;
	virtual void			OnError(LPCTSTR reason) = 0;
	virtual void			OnStatistics(LPCTSTR statistics) = 0;
	virtual void			OnControlResult(const unsigned int token, bool result, LPCTSTR reason) = 0;
	virtual void			OnDiscarded() = 0;
};

/*����Դ������
* �������̣�
*	1��	���ػ�ͨ��Factory������ʽ����SourceProxy�Ķ���	
*	2��	SourceProxy�����ڲ��������Լ��Ĺ����̣߳�event����
*	3��	�����ص���ISourceProxy::Discardǰ��SourceProxy������Բ����Զ��ͷ��Լ���
*	4��	��SourceProxy�����ڲ�����ʱ�����ȵ���OnError֪ͨ���أ����ҿ�����ֹ�ڲ��̣߳����ǲ����ͷ��Լ���
*	5��	������ͨ��OnError��⵽������󣬻���������Ҫ�����ͷ�SourceProxy����ʱ�������Discard�ͷ�SourceProxy����
*	6��	��Discard�У�SourceProxy����ͬ���ͷ��Լ���Ҳ�������Լ����߳����첽�ͷ��Լ���
		�����ͷ��Լ���ɺ󣬱���ͨ��OnDestroied��֪�����Լ��Ѿ��ͷ���ɡ�
*	7��	�������յ�OnDestroied�󣬲Żᰲȫ���ͷ��ڲ���Source����

*PTZ��VideoEffect�������̣�
*	1�����ػ�ͨ��PTZControl����VideoEffect�������
*	2�����SourceProxy�ڲ�����ִ�������ֱ�ӷ���Failed����Finished����ִ����ɣ�
*	3�����SourceProxy��Ҫһ��ʱ�䣨�����첽��������ִ�������Ӧ���ȷ���Pending��
	   ������ִ����ɣ������첽��Ӧ����ͨ��ISourceProxyCallback.OnControlResult�ص�ִ�н��
*	4���ص�ģʽ�£��������ȴ�5s��������Ϊִ��ʧ��
*/
struct event_base;
interface ISourceProxy {
	enum PTZAction { Stop = 0x0000, ZoomIn = 0x0001, ZoomOut = 0x0002, FocusNear = 0x0004, FocusFar = 0x0008,
		IrisOpen = 0x0010, IrisClose = 0x0020, TiltUp = 0x0040, TiltDown = 0x0080, PanLeft = 0x0100, PanRight = 0x0200 };
	enum ControlResult { Failed = -1, Finished = 0, Pending = 1};
	virtual ~ISourceProxy() {}
	virtual LPCTSTR			SourceName() const = 0;
	virtual unsigned int    MaxStartDelay(const unsigned int def) { return def; }
	virtual unsigned int    MaxFrameDelay(const unsigned int def) { return def; }
	virtual bool			StartFetch(event_base* base) = 0;
	virtual void			WantKeyFrame() = 0;
	virtual ControlResult	PTZControl(const unsigned int token, const unsigned int action, const int speed) = 0;
	virtual ControlResult	VideoEffect(const unsigned int token, const int bright, const int contrast, const int saturation, const int hue) = 0;
	virtual bool			Discard() = 0;
};

interface ISourceFactoryCallback {
	virtual ~ISourceFactoryCallback() {}
	virtual event_base*		GetPreferBase() = 0;
	virtual event_base**	GetAllBase(int& count) = 0;
	virtual void			OnNewSource(LPCTSTR lpszDevice, LPCTSTR lpszMoniker, LPCTSTR lpszName, LPCTSTR lpszMeta = NULL) = 0;
	virtual void			OnSourceOver(LPCTSTR lpszDevice, LPCTSTR lpszMoniker) = 0;
};

struct event_base;
interface ISourceFactory {
	enum SupportState { unsupport, supported, maybesupport };
	virtual ~ISourceFactory() {}
	virtual LPCTSTR			FactoryName() const = 0;
	virtual bool			Initialize(ISourceFactoryCallback* callback) = 0;
	virtual SupportState	DidSupport(LPCTSTR device, LPCTSTR moniker, LPCTSTR param) = 0;
	virtual ISourceProxy*	CreateLiveProxy(ISourceProxyCallback* callback, LPCTSTR device, LPCTSTR moniker, LPCTSTR param = NULL) = 0;
	virtual ISourceProxy*	CreatePastProxy(ISourceProxyCallback* callback, LPCTSTR device, LPCTSTR moniker, LPCTSTR param, uint64_t beginTime, uint64_t endTime = 0) = 0;
	virtual bool			GetSourceList(LPTSTR* xml, void(**free)(LPTSTR), bool onlineOnly, LPCTSTR device = NULL) = 0;
	virtual bool			GetStatusInfo(LPSTR* json, void(**free)(LPSTR)) = 0;
	virtual void			Uninitialize() = 0;
	virtual void			Destroy() = 0;
};

ISourceFactory*				GetSourceFactory();
