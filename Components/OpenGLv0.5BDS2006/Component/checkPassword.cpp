#include <vcl.h>
#pragma hdrstop
#pragma resource "*.dfm"
#include "checkPassword.h"

void __fastcall TFormPassword::btnOkClick(TObject *Sender)
{
    const String VALID_PASSWORD = "1234";

    if (edtPassword->Text == VALID_PASSWORD) {
        ModalResult = mrOk; // ��й�ȣ ������ OK
    } else {
        ShowMessage(L"��й�ȣ�� Ʋ�Ƚ��ϴ�.");
        edtPassword->Clear();
        edtPassword->SetFocus();
    }
}

