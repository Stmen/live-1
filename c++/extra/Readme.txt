1��libevent����˵����
	linux:
		���ã�./configure --with-pic --enable-static
		���룺make
		����ɹ�����Դ��Ŀ¼��.lib��Ŀ¼���ܹ��ҵ�����õ��ĸ�.a�ļ���Ҳ����������Ҫ���ļ���
		--with-pic����Ϊ���ز���Ķ�̬����Ҫ-fPICѡ��֧�֡������޷����سɹ���
	windows��
		����VS2015 x64���ع���������ʾ����
		��libeventԴ��Ŀ¼��ִ��nmake -f Makefile.nmake
		����ɹ�����Դ�뵱ǰĿ¼���ܹ��ҵ���������õ�.lib�ļ���Ҳ����������Ҫ���ļ���
	
2��zlib����˵��
	linux��
		���ã���ִ�� export CFLAGS=-fPIC�� Ȼ��ִ��./configure --static
		���룺make
		����ɹ�����Դ��Ŀ¼���ܹ��ҵ�����õ�libz.a�ļ���Ҳ����������Ҫ���ļ���
	windows��
		����VS2015�������ߣ�
		��Դ��Ŀ¼��\contrib\vstudio\vc14�е�zlibvc.sln�����ļ���
		ѡ�񹤳�����ΪReleaseWithoutAsm x64���޻��֧�ֵ�X64�汾����Ȼ����빤��zlibstat������Ǳ���Ϊ��̬���ļ�����
		����ɹ��󣬰���VS2015����ʾ��Դ��Ŀ¼��\contrib\vstudio\vc14\x64\ZlibStatReleaseWithoutAsm\zlibstat.lib����������Ҫ���ļ���
	ע�⣺windows�����linux������໥Ӱ�������ļ���ÿ�α���ǰ��Ҫɾ��Ŀ¼���´�SVNͬ����
	������Ҫ����ͷ�ļ���zconf.h zlib.h
	
3��openssl����˵����
	linux:
		���ã���Դ��Ŀ¼��ִ��./config -fPIC
		���룺make
		����ɹ�����Դ��Ŀ¼���ܹ��ҵ�����õ�libssl.a��libcrypto.a�ļ���Ҳ����������Ҫ���ļ���
		������Ҫ������ͷ�ļ���Դ��Ŀ¼�µ�include�ļ����У�
	windows��
		1����װActivPerl 5.24��
		2������Visual Studio 2015 x64���ع���������ʾ����
		3������opensslԴ��Ŀ¼��ִ�У�
			perl Configure VC-WIN64A
			ms\do_win64a	#����Makefile�ļ�
			nmake -f ms\ntdll.mak  #���붯̬�⣨�ȴ�ʱ��ϳ�������ѡ
			nmake -f ms\ntdll.mak clean  #����ϴ�OpenSSL��̬��ı���
			nmake -f ms\nt.mak   #���뾲̬�⣨������Ҫ��ѡ���Ƿ���뾲̬�⣩
			nmake -f ms\nt.mak clean  #����ϴ�OpenSSL��̬��ı���
		����ɹ�����Դ��Ŀ¼�µ�out32Ŀ¼�е�ssleay32.lib��libeay32.lib����������Ҫ���ļ���
		������Ҫ������ͷ�ļ���Դ��Ŀ¼�µ�inc32�ļ����С�