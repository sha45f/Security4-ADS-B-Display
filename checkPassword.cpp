#include <vcl.h>
#pragma hdrstop
#pragma resource "*.dfm"
#include "checkPassword.h"
#include "AwsConfig.h"  // ✨ 추가

// WinInet
#include <WinInet.h>
#pragma comment(lib, "wininet.lib")

// JSON
#include <System.JSON.hpp>

int failedAttempts = 0;

__fastcall TFormPassword::TFormPassword(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TFormPassword::btnOkClick(TObject *Sender)
{
    if (failedAttempts >= 5)
    {
        ShowMessage(L"5회 이상 비밀번호 오류! 로그인 차단.");
        return;
    }

    AnsiString password = edtPassword->Text;

    HINTERNET hInternet = InternetOpen(L"MyApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
    {
        ShowMessage(L"InternetOpen 실패!");
        return;
    }

    HINTERNET hConnect = InternetConnect(
        hInternet,
        API_HOST,  // ✨ config.h 사용
        INTERNET_DEFAULT_HTTPS_PORT,
        NULL, NULL,
        INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect)
    {
        ShowMessage(L"InternetConnect 실패!");
        InternetCloseHandle(hInternet);
        return;
    }

    HINTERNET hRequest = HttpOpenRequest(
        hConnect, L"POST", API_PATH,  // ✨ config.h 사용
        NULL, NULL, NULL,
        INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hRequest)
    {
        ShowMessage(L"HttpOpenRequest 실패!");
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    LPCWSTR headers =
        L"Content-Type: application/json\r\n"
        L"x-api-key: " API_KEY L"\r\n";  // ✨ config.h 사용
    if (!HttpAddRequestHeaders(hRequest, headers, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE))
    {
        ShowMessage(L"HttpAddRequestHeaders 실패!");
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    AnsiString jsonData = "{\"password\":\"" + password + "\"}";

    if (!HttpSendRequestA(hRequest, NULL, 0, (LPVOID)jsonData.c_str(), jsonData.Length()))
    {
        ShowMessage(L"HttpSendRequest 실패!");
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    char buffer[4096];
    DWORD bytesRead;
    String response = "";
    do {
        if (!InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) || bytesRead == 0)
            break;
        buffer[bytesRead] = 0;
        response += String(buffer);
    } while (bytesRead > 0);

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    try {
        TJSONObject *json = (TJSONObject*) TJSONObject::ParseJSONValue(response);
        if (json)
        {
            String bodyValue = json->GetValue("body")->Value();
            bodyValue = StringReplace(bodyValue, "\"", "", TReplaceFlags() << rfReplaceAll);

            wprintf(L"[Log] Lambda response receive: %s\n", response.c_str());

            if (bodyValue.Trim() == "true")
            {
                failedAttempts = 0;
                ModalResult = mrOk;
            }
            else
            {
                failedAttempts++;
                ShowMessage(L"Wrong Password.");
                edtPassword->Clear();
                edtPassword->SetFocus();

                if (failedAttempts >= 5)
                {
                    ShowMessage(L"5회 이상 비밀번호 오류! 로그인 차단.");
                }
            }
            delete json;
        }
        else
        {
            ShowMessage(L"JSON Parse Error");
        }
    }
    catch (const Exception &e)
    {
        ShowMessage(L"JSON Parse Exception: " + e.Message);
    }
}
void __fastcall TFormPassword::SetICAOText(AnsiString icao)
{
	flightNumberBox->Text = icao;  
}