// live_sink_raw.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "RawFactory.h"

ISinkFactory* GetSinkFactory() {
	return &CNalFactory::singleton();
}

