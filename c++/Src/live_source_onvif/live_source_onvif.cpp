// live_source_hxht.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "OnvifFactory.h"

ISourceFactory* GetSourceFactory() {
	return &COnvifFactory::Singleton();
}


