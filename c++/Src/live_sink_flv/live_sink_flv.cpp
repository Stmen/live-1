// live_sink_flv.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "FlvFactory.h"

ISinkFactory* GetSinkFactory() {
	return &CFlvFactory::singleton();
}

