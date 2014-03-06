object Main: TMain
  Left = 192
  Top = 125
  Width = 552
  Height = 332
  Caption = 'Monolith - Material Builder'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Preview: TImage
    Left = 256
    Top = 16
    Width = 256
    Height = 256
    OnMouseMove = PreviewMouseMove
  end
  object LRed: TLabel
    Left = 16
    Top = 20
    Width = 20
    Height = 13
    Caption = 'Red'
  end
  object LGreen: TLabel
    Left = 16
    Top = 44
    Width = 29
    Height = 13
    Caption = 'Green'
  end
  object LBlue: TLabel
    Left = 16
    Top = 68
    Width = 21
    Height = 13
    Caption = 'Blue'
  end
  object LSpec: TLabel
    Left = 16
    Top = 108
    Width = 42
    Height = 13
    Caption = 'Specular'
  end
  object LShin: TLabel
    Left = 16
    Top = 132
    Width = 45
    Height = 13
    Caption = 'Shininess'
  end
  object LEmissive: TLabel
    Left = 16
    Top = 220
    Width = 41
    Height = 13
    Caption = 'Emissive'
  end
  object Label1: TLabel
    Left = 16
    Top = 260
    Width = 30
    Height = 13
    Caption = 'Result'
  end
  object ERed: TEdit
    Left = 112
    Top = 16
    Width = 121
    Height = 21
    Hint = 'Red [0,255]'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    Text = '255'
    OnChange = ERedChange
  end
  object EGreen: TEdit
    Left = 112
    Top = 40
    Width = 121
    Height = 21
    Hint = 'Green [0,255]'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    Text = '255'
    OnChange = ERedChange
  end
  object EBlue: TEdit
    Left = 112
    Top = 64
    Width = 121
    Height = 21
    Hint = 'Blue [0,255]'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    Text = '0'
    OnChange = ERedChange
  end
  object ESpec: TEdit
    Left = 112
    Top = 104
    Width = 121
    Height = 21
    Hint = 'Code [0,15]'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    Text = '0'
    OnChange = ERedChange
  end
  object EShin: TEdit
    Left = 112
    Top = 128
    Width = 121
    Height = 21
    Hint = 'Code [0,15]'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    Text = '0'
    OnChange = ERedChange
  end
  object RBTransparent: TRadioButton
    Left = 16
    Top = 168
    Width = 113
    Height = 17
    Caption = 'Transparent'
    TabOrder = 5
    OnClick = CBEmissiveClick
  end
  object RBRotating: TRadioButton
    Left = 16
    Top = 184
    Width = 113
    Height = 17
    Caption = 'Rotating'
    Checked = True
    TabOrder = 6
    TabStop = True
    OnClick = CBEmissiveClick
  end
  object EVar: TEdit
    Left = 112
    Top = 176
    Width = 121
    Height = 21
    Hint = 'Code [0,15]'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    Text = '0'
    OnChange = EVarChange
  end
  object CBEmissive: TCheckBox
    Left = 112
    Top = 216
    Width = 97
    Height = 17
    Caption = 'Yes'
    TabOrder = 8
    OnClick = CBEmissiveClick
  end
  object EResult: TEdit
    Left = 112
    Top = 256
    Width = 121
    Height = 21
    TabOrder = 9
    Text = '0'
  end
  object Imp: TButton
    Left = 64
    Top = 256
    Width = 41
    Height = 21
    Caption = 'Imp'
    TabOrder = 10
    OnClick = EResultChange
  end
  object Timer1: TTimer
    Interval = 16
    OnTimer = Timer1Timer
    Left = 440
    Top = 216
  end
end
