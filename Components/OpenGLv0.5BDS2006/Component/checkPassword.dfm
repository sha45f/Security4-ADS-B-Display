object FormPassword: TFormPassword
  Left = 0
  Top = 0
  Caption = 'Protected Flight Information'
  ClientHeight = 194
  ClientWidth = 199
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  TextHeight = 15
  object password: TLabel
    Left = 8
    Top = 112
    Width = 53
    Height = 15
    Caption = 'Password:'
  end
  object title: TLabel
    Left = 8
    Top = 8
    Width = 155
    Height = 46
    Caption = 'PROTECTED FLIGHT '#13#10'    INFORMATION'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
    OnClick = titleClick
  end
  object flightNumber: TLabel
    Left = 8
    Top = 60
    Width = 78
    Height = 15
    Caption = 'Flight number:'
  end
  object edtPassword: TEdit
    Left = 67
    Top = 110
    Width = 55
    Height = 23
    PasswordChar = '*'
    TabOrder = 0
    Text = 'Edit1'
  end
  object btnOk: TButton
    Left = 67
    Top = 160
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = btnOkClick
  end
  object flightNumberBox: TEdit
    Left = 8
    Top = 81
    Width = 178
    Height = 23
    TabOrder = 2
    Text = 'FLIGHTNUMB'
  end
end
