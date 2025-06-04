//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("checkPassword.cpp", FormPassword);
USEFORM("DisplayGUI.cpp", Form1);
USEFORM("AreaDialog.cpp", AreaConfirm);
//---------------------------------------------------------------------------
static FILE* pCout = NULL;
static void SetStdOutToNewConsole(void);
//---------------------------------------------------------------------------
static void SetStdOutToNewConsole(void)
{
    // Allocate a console for this app
    AllocConsole();
    //AttachConsole(ATTACH_PARENT_PROCESS);
	freopen_s(&pCout, "CONOUT$", "w", stdout);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "OpenSSLLoader.h"  // 앞서 만든 동적 로더 클래스 헤더
OpenSSLLoader ssl;
SSL_CTX* g_ssl_ctx = nullptr;  // 전역으로 SSL_CTX 보관 (필요시)

#pragma comment(lib, "ws2_32.lib")

bool myInitSSL()
{
    // 1. OpenSSL DLL 동적 로딩
    if (!ssl.Load()) {
        MessageBox(nullptr, _T("OpenSSL DLL 로드 실패!"), _T("Error"), MB_OK | MB_ICONERROR);
        return false;
    }

    // 2. OpenSSL 라이브러리 초기화
    if (ssl.OPENSSL_init_ssl(0, nullptr) != 1) {
        MessageBox(nullptr, _T("OPENSSL_init_ssl 실패!"), _T("Error"), MB_OK | MB_ICONERROR);
        return false;
    }

    // 3. TLS 클라이언트 메서드로 SSL_CTX 생성
    g_ssl_ctx = ssl.SSL_CTX_new(ssl.TLS_client_method());
    if (!g_ssl_ctx) {
        MessageBox(nullptr, _T("SSL_CTX_new 실패!"), _T("Error"), MB_OK | MB_ICONERROR);
        return false;
    }

    // 4. 서버 CA 인증서 설정 - 서버 인증서 검증용
	if (ssl.SSL_CTX_load_verify_locations(g_ssl_ctx, "lgess2025s4rpicert.pem", nullptr) != 1) {
        MessageBox(nullptr, _T("CA 인증서 로드 실패!"), _T("Error"), MB_OK | MB_ICONERROR);
        ssl.SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = nullptr;
        return false;
    }

    // 5. 클라이언트 인증서와 개인 키 설정 (mutual TLS)
	if (ssl.SSL_CTX_use_certificate_file(g_ssl_ctx, "lgess2025s4clientcert.pem", SSL_FILETYPE_PEM) != 1) {
		MessageBox(nullptr, _T("클라이언트 인증서 로드 실패!"), _T("Error"), MB_OK | MB_ICONERROR);
        ssl.SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = nullptr;
        return false;
    }

	if (ssl.SSL_CTX_use_PrivateKey_file(g_ssl_ctx, "lgess2025s4clientkey.pem", SSL_FILETYPE_PEM) != 1) {
        MessageBox(nullptr, _T("클라이언트 개인 키 로드 실패!"), _T("Error"), MB_OK | MB_ICONERROR);
        ssl.SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = nullptr;
        return false;
    }

    // 6. 개인 키와 인증서가 일치하는지 확인
	if (ssl.SSL_CTX_check_private_key(g_ssl_ctx) != 1) {
        MessageBox(nullptr, _T("개인 키와 인증서가 일치하지 않습니다!"), _T("Error"), MB_OK | MB_ICONERROR);
        ssl.SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = nullptr;
        return false;
    }

    // 7. 서버 인증서 검증 모드 설정
    ssl.SSL_CTX_set_verify(g_ssl_ctx, SSL_VERIFY_PEER, nullptr);

    MessageBox(nullptr, _T("OpenSSL 초기화 및 인증서 설정 완료"), _T("Info"), MB_OK | MB_ICONINFORMATION);
    return true;
}

int tclinet()
{
    // WinSock 초기화
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup 실패\n";
        return 1;
    }

    // TCP 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "소켓 생성 실패\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4343);
    inet_pton(AF_INET, "192.168.43.3", &serverAddr.sin_addr);

    // 서버 연결
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        std::cerr << "서버 연결 실패\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // SSL 객체 생성 및 핸드셰이크
    SSL* sslConn = ssl.SSL_new(g_ssl_ctx);
    ssl.SSL_set_fd(sslConn, (int)sock);

    if (ssl.SSL_connect(sslConn) != 1) {
        std::cerr << "SSL_connect 실패\n";
        ssl.SSL_free(sslConn);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "TLS 연결 성공. 데이터 수신 중...\n";

    // 수신 루프
    for (int i = 0; i < 5; ++i) {
        char buf[1024] = {0};
        int bytes = ssl.SSL_read(sslConn, buf, sizeof(buf) - 1);
        if (bytes > 0) {
            std::cout << "수신: " << buf << std::endl;
        } else {
            std::cerr << "데이터 수신 실패 또는 연결 종료\n";
            break;
        }
        Sleep(1000);
    }

    // 종료
    ssl.SSL_shutdown(sslConn);
    ssl.SSL_free(sslConn);
    closesocket(sock);
    WSACleanup();

    std::cout << "종료됨\n";
    return 0;
}

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    if (!myInitSSL()) {
		return -1;  // 초기화 실패 시 종료
	}
    tclinet();
	try
	{
        SetStdOutToNewConsole();
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->CreateForm(__classid(TAreaConfirm), &AreaConfirm);
		Application->Run();
	   if (pCout)
		{
		 fclose(pCout);
		 FreeConsole();
	    }
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
