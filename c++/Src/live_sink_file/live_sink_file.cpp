// live_sink_flv.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "FileFactory.h"

ISinkFactory* GetSinkFactory() {
	return &CFileFactory::singleton();
}

