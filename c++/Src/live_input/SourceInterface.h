#pragma once
#include <map>
#include "os.h"
#include "xchar.h"

interface ISourceProxyCallback {
	virtual ~ISourceProxyCallback() {}
	virtual void			OnHeaderData(const uint8_t* const data, const int size) = 0;
	virtual void			OnVideoData(const uint8_t* const data, const int size, const bool key = true) = 0;
	virtual void			OnAudioData(const uint8_t* const data, const int size) = 0;
	virtual void			OnCustomData(const uint8_t* const data, const int size) = 0;
	virtual void			OnError(LPCTSTR reason) = 0;
	virtual void			OnStatistics(LPCTSTR statistics) = 0;
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
*/
struct event_base;
interface ISourceProxy {
	enum PTZAction { ZoomIn = 0, ZoomOut, FocusNear, FocusFar, IrisOpen, IrisClose, TiltUp, TiltDown, PanLeft, PanRight };
	virtual ~ISourceProxy() {}
	virtual LPCTSTR			SourceName() const = 0;
	virtual unsigned int    MaxStartDelay(const unsigned int def) { return def; }
	virtual unsigned int    MaxFrameDelay(const unsigned int def) { return def; }
	virtual bool			StartFetch(event_base* base) = 0;
	virtual void			WantKeyFrame() = 0;
	virtual bool			PTZControl(const PTZAction action, const int value) = 0;
	virtual bool			VideoEffect(const int bright, const int contrast, const int saturation, const int hue) = 0;
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
