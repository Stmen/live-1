// live_sink_flv.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "NalFactory.h"

IDecodeFactory* GetDecodeFactory() {
	return &CNalFactory::singleton();
}

