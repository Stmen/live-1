// live_source_hxht.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "HxhtFactory.h"

ISourceFactory* GetSourceFactory() {
	return &CHxhtFactory::Singleton();
}


