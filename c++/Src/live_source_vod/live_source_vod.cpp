// live_source_rtmp.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "VodFactory.h"

ISourceFactory* GetSourceFactory() {
	return &CVodFactory::Singleton();
}
