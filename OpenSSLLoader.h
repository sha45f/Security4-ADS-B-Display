#pragma once

#include <windows.h>
#include <openssl/ssl.h>

class OpenSSLLoader
{
private:
    HMODULE hModule = nullptr;

public:
    // �Լ� ������ ���� (�ʿ��� �Լ��� ����)
    int (*OPENSSL_init_ssl)(uint64_t opts, const void* settings) = nullptr;
    SSL_CTX* (*SSL_CTX_new)(const SSL_METHOD* method) = nullptr;
    void (*SSL_CTX_free)(SSL_CTX* ctx) = nullptr;
    const SSL_METHOD* (*TLS_client_method)(void) = nullptr;
    int (*SSL_CTX_load_verify_locations)(SSL_CTX* ctx, const char* CAfile, const char* CApath) = nullptr;
    int (*SSL_CTX_use_certificate_file)(SSL_CTX* ctx, const char* file, int type) = nullptr;
    int (*SSL_CTX_use_PrivateKey_file)(SSL_CTX* ctx, const char* file, int type) = nullptr;
    int (*SSL_CTX_check_private_key)(SSL_CTX* ctx) = nullptr;
    void (*SSL_CTX_set_verify)(SSL_CTX* ctx, int mode, int (*callback)(int, void *)) = nullptr;
    // �ʿ��ϸ� �߰� �Լ� ����
    SSL* (*SSL_new)(SSL_CTX *ctx);
    int  (*SSL_set_fd)(SSL *ssl, int fd);
    int  (*SSL_connect)(SSL *ssl);
    int  (*SSL_read)(SSL *ssl, void *buf, int num);
    int  (*SSL_shutdown)(SSL *ssl);
    void (*SSL_free)(SSL *ssl);

public:
    OpenSSLLoader() = default;
    ~OpenSSLLoader();

    // DLL �ε� �� �Լ� �ּ� ���
    bool Load();
    void Unload();

    // ��� ���Ǹ� ���� ���� �Լ� ���� (���ϸ�)
};

