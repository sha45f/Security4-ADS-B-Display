#include "OpenSSLLoader.h"

OpenSSLLoader::~OpenSSLLoader()
{
    Unload();
}

bool OpenSSLLoader::Load()
{
    if (hModule)
        return true;  // �̹� �ε��

	// OpenSSL DLL ��� (��ġ ��ġ�� �°� ����)
	// ���� C:\OpenSSL-Win64\bin ������ libssl-3.dll, libcrypto-3.dll ����
	// ���⼭�� libssl-3.dll�� �ε�, libcrypto-3.dll�� ���� �ε���
	SetDllDirectory(L"C:\\OpenSSL-Win64\\bin");

	hModule = LoadLibraryW(L"C:\\OpenSSL-Win64\\bin\\libssl-3-x64.dll");
	if (!hModule)
	{
			MessageBox(nullptr, _T("OpenSSL DLL �ε� ����! CP#1"), _T("Error"), MB_OK | MB_ICONERROR);

		return false;
	}

    OPENSSL_init_ssl = (decltype(OPENSSL_init_ssl))GetProcAddress(hModule, "OPENSSL_init_ssl");
    SSL_CTX_new = (decltype(SSL_CTX_new))GetProcAddress(hModule, "SSL_CTX_new");
    SSL_CTX_free = (decltype(SSL_CTX_free))GetProcAddress(hModule, "SSL_CTX_free");
    TLS_client_method = (decltype(TLS_client_method))GetProcAddress(hModule, "TLS_client_method");
    SSL_CTX_load_verify_locations = (decltype(SSL_CTX_load_verify_locations))GetProcAddress(hModule, "SSL_CTX_load_verify_locations");
    SSL_CTX_use_certificate_file = (decltype(SSL_CTX_use_certificate_file))GetProcAddress(hModule, "SSL_CTX_use_certificate_file");
    SSL_CTX_use_PrivateKey_file = (decltype(SSL_CTX_use_PrivateKey_file))GetProcAddress(hModule, "SSL_CTX_use_PrivateKey_file");
    SSL_CTX_check_private_key = (decltype(SSL_CTX_check_private_key))GetProcAddress(hModule, "SSL_CTX_check_private_key");
    SSL_CTX_set_verify = (decltype(SSL_CTX_set_verify))GetProcAddress(hModule, "SSL_CTX_set_verify");

    if (!OPENSSL_init_ssl || !SSL_CTX_new || !SSL_CTX_free || !TLS_client_method ||
        !SSL_CTX_load_verify_locations || !SSL_CTX_use_certificate_file || !SSL_CTX_use_PrivateKey_file ||
        !SSL_CTX_check_private_key || !SSL_CTX_set_verify)
    {
        Unload();
        return false;
    }

	SSL_new = (decltype(SSL_new))GetProcAddress(hModule, "SSL_new");
	SSL_set_fd = (decltype(SSL_set_fd))GetProcAddress(hModule, "SSL_set_fd");
	SSL_connect = (decltype(SSL_connect))GetProcAddress(hModule, "SSL_connect");
	SSL_read = (decltype(SSL_read))GetProcAddress(hModule, "SSL_read");
	SSL_shutdown = (decltype(SSL_shutdown))GetProcAddress(hModule, "SSL_shutdown");
	SSL_free = (decltype(SSL_free))GetProcAddress(hModule, "SSL_free");

	// ��� �ε� �����ߴ��� Ȯ��
	if (!SSL_new || !SSL_set_fd || !SSL_connect || !SSL_read || !SSL_shutdown || !SSL_free)
		return false;    return true;
	}

void OpenSSLLoader::Unload()
{
    if (hModule) {
        FreeLibrary(hModule);
        hModule = nullptr;

        OPENSSL_init_ssl = nullptr;
        SSL_CTX_new = nullptr;
        SSL_CTX_free = nullptr;
        TLS_client_method = nullptr;
        SSL_CTX_load_verify_locations = nullptr;
        SSL_CTX_use_certificate_file = nullptr;
        SSL_CTX_use_PrivateKey_file = nullptr;
        SSL_CTX_check_private_key = nullptr;
        SSL_CTX_set_verify = nullptr;
    }
}

