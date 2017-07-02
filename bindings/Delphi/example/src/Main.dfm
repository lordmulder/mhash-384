object MainForm: TMainForm
  Left = 608
  Top = 189
  Width = 872
  Height = 271
  Caption = 'MHashDelphi384 - Example App'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  DesignSize = (
    856
    232)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 58
    Height = 13
    Caption = 'Input File:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 64
    Width = 65
    Height = 13
    Caption = 'File Digest:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Button_Compute: TButton
    Left = 711
    Top = 183
    Width = 123
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Compute Hash'
    TabOrder = 1
    OnClick = Button_ComputeClick
  end
  object Edit_InputFile: TEdit
    Left = 8
    Top = 24
    Width = 825
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    ReadOnly = True
    TabOrder = 2
  end
  object Edit_FileDigest: TEdit
    Left = 8
    Top = 80
    Width = 825
    Height = 22
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
  end
  object Button_Browse: TButton
    Left = 575
    Top = 183
    Width = 121
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Browse File'
    TabOrder = 0
    OnClick = Button_BrowseClick
  end
  object ProgressBar: TProgressBar
    Left = 8
    Top = 128
    Width = 825
    Height = 17
    Anchors = [akLeft, akTop, akRight]
    Smooth = True
    TabOrder = 4
  end
  object OpenDialog: TOpenDialog
    Left = 8
    Top = 168
  end
end
