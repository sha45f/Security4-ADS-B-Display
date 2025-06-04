#include <vcl.h>
#pragma hdrstop
#pragma resource "*.dfm"
#include "checkPassword.h"

void __fastcall TFormPassword::btnOkClick(TObject *Sender)
{
    const String VALID_PASSWORD = "1234";

    if (edtPassword->Text == VALID_PASSWORD) {
        ModalResult = mrOk; // 비밀번호 맞으면 OK
    } else {
        ShowMessage(L"비밀번호가 틀렸습니다.");
        edtPassword->Clear();
        edtPassword->SetFocus();
    }
}

