// live_source_rtmp.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "RtmpFactory.h"

ISourceFactory* GetSourceFactory() {
	return &CRtmpFactory::Singleton();
}
