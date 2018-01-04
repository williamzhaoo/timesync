#include <stdio.h>
#include <time.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

#define NET_TIME_PORT 37
#define TIMEOUT_CONNECT 5
#define TIMEOUT_RECEIVE 1

char* sever[]=
{
	/*"159.226.154.47",
	"129.132.2.21",
	"192.43.244.18",
	"131.107.1.10",
	"66.243.43.21",
	"216.200.93.8",
	"208.184.49.9",
	"207.126.98.204",
	"207.200.81.113",
	"205.188.185.33",*/
	"129.6.15.28",
	"129.6.15.29",
	"132.163.4.101",
	"132.163.4.102",
	"132.163.4.103",
	"128.138.140.44",
	NULL
};

BOOL GetNetworkTime(char* szSever, unsigned long& ulTime)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET)
	{
		return FALSE;
	}

	sockaddr_in severAddr;
	severAddr.sin_family = AF_INET;
	severAddr.sin_port = htons(NET_TIME_PORT);
	severAddr.sin_addr.S_un.S_addr = inet_addr(szSever);

	if (4 == sendto(sock, (char*)&ulTime, 4, 0, (sockaddr*)&severAddr, sizeof(severAddr)))
	{
		unsigned long flag = 1;
		if (0 == ioctlsocket(sock, FIONBIO, &flag))
		{
			struct fd_set mask;
			FD_ZERO(&mask);
			FD_SET(sock, &mask);

			struct timeval timeout;
			timeout.tv_sec = TIMEOUT_RECEIVE;
			timeout.tv_usec = 0;

			if (1 == select(0, &mask, NULL, NULL, &timeout))
			{ 
				if (4 == recv(sock, (char*)&ulTime, 4, 0))
				{
					ulTime = ntohl(ulTime);
					closesocket(sock);
					return TRUE;
				}
			}
		}
	}
	closesocket(sock);

	return FALSE;
}

void SetNewTime(unsigned long ulTime)
{
	FILETIME ft;
	SYSTEMTIME st;

	st.wYear = 1900;
	st.wMonth = 1;
	st.wDay = 1;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;

	SystemTimeToFileTime(&st, &ft);
	LARGE_INTEGER li = *(LARGE_INTEGER*)&ft;
	li.QuadPart += (LONGLONG)10000000 * ulTime;
	ft = *(FILETIME*)&li;
	FileTimeToSystemTime(&ft, &st); 
	SetSystemTime(&st);
}


void main()
{
	printf("\n\t欢迎使用网络时间同步工具\n");

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("提示：网络初始化失败，请检查网络连接。");
	}

	int i = 0;
	unsigned long ntime = 0;
	while (sever[i] != NULL)
	{
		if (GetNetworkTime(sever[i], ntime))
		{
			SetNewTime(ntime);
			printf("\n\t提示：成功连接时间服务器 %15s 时间同步操作完成。", sever[i]);
		}
		else
		{
			char buff[100];
			printf("\n\t提示：无法连接时间服务器 %15s 时间同步操作失败。", sever[i]);
		}
		i++;
	}

	WSACleanup();

	system("pause");
}
