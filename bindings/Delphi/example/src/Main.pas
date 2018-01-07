{ ---------------------------------------------------------------------------------------------- }
{ MHash-384 for Delphi/Pascal                                                                    }
{ Copyright(c) 2016-2018 LoRd_MuldeR <mulder2@gmx.de>                                            }
{                                                                                                }
{ Permission is hereby granted, free of charge, to any person obtaining a copy of this software  }
{ and associated documentation files (the "Software"), to deal in the Software without           }
{ restriction, including without limitation the rights to use, copy, modify, merge, publish,     }
{ distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  }
{ Software is furnished to do so, subject to the following conditions:                           }
{                                                                                                }
{ The above copyright notice and this permission notice shall be included in all copies or       }
{ substantial portions of the Software.                                                          }
{                                                                                                }
{ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  }
{ BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     }
{ NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   }
{ DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, }
{ OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        }
{ ---------------------------------------------------------------------------------------------- }

unit Main;

{===========================================================================}
{ INTERFACE                                                                 }
{===========================================================================}

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, SyncObjs, MHash384;

type
  TMainForm = class(TForm)
    Button_Compute: TButton;
    Edit_InputFile: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Edit_FileDigest: TEdit;
    Button_Browse: TButton;
    ProgressBar: TProgressBar;
    OpenDialog: TOpenDialog;
    procedure FormCreate(Sender: TObject);
    procedure Button_BrowseClick(Sender: TObject);
    procedure Button_ComputeClick(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
  protected
    procedure ThreadTerminated(Sender: TObject);
    procedure ProgressChanged(progress: Integer);
    procedure UpdateProgress(total: Integer; current: Integer; progress: Integer);
  private
    AbortFlag: TSimpleEvent;
  public
    { Public-Deklarationen }
  end;

type
  TProgressEvent = procedure(progress: Integer) of object;

type
  TComputeThread = class(TThread)
  private
    hwnd: HWND;
    inputFile, hashString: String;
    progressValue: Integer;
    progressEvent: TProgressEvent;
    abortFlag: TSimpleEvent;
  public
    constructor Create(const hwnd: HWND; const inputFile: String; const abortFlag: TSimpleEvent);
    function GetResult: String;
    property OnProgress: TProgressEvent read progressEvent write progressEvent;
  protected
    procedure Execute(); override;
    function FileSize64(const fileName: string): UInt64;
    procedure SetProgress(const processed: UInt64; const totalSize: UInt64);
    function ByteToHex(input: TByteArray):String;
    procedure UpdateProgress;
  end;

var
  MainForm: TMainForm;

{===========================================================================}
{ IMPLEMENTATION                                                            }
{===========================================================================}

implementation

{$R *.dfm}

{----------------------------------------------}
{ Main Form                                    }
{----------------------------------------------}

procedure TMainForm.FormCreate(Sender: TObject);
var
  VersionMajor, VersionMinor, VersionPatch: Word;
begin
  Constraints.MinHeight := Height;
  Constraints.MinWidth := Width;
  TMHash384.GetVersion(VersionMajor, VersionMinor, VersionPatch);
  Caption := 'MHashDelphi384 - Example App v' + Format('%d.%d.%d', [VersionMajor, VersionMinor, VersionPatch]);
  AbortFlag := TSimpleEvent.Create();
end;

procedure TMainForm.FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
const
  F12: Word = $7B;
  ESC: Word = $1B;
begin
  if Key = F12 then
  begin
    Button_Browse.Enabled := False;
    Button_Compute.Enabled := False;
    ProgressBar.Position := 0;
    Edit_FileDigest.Text := 'Self-test is running, please be patient...';
    Application.ProcessMessages();
    try
      try
        TMHash384.SelfTest(UpdateProgress, AbortFlag);
        ShowMessage('Self-test completed successfully.');
      except
        on E: Exception do
        begin
          MessageBox(Self.WindowHandle, PAnsiChar('Self-test has faild with an error!'#10#10'Details:'#10 + E.Message), PAnsiChar(String(E.ClassName)), MB_ICONERROR or MB_SYSTEMMODAL);
          Exit;
         end;
      end;
    finally
      Button_Browse.Enabled := True;
      Button_Compute.Enabled := True;
      ProgressBar.Position := 100;
      Edit_FileDigest.Text := 'Self-test completed.';
    end;
  end;
  if Key = ESC then
  begin
    AbortFlag.SetEvent();
  end;
end;

procedure TMainForm.UpdateProgress(total: Integer; current: Integer; progress: Integer);
begin
  Edit_FileDigest.Text := 'Self-test is running (step ' + IntToStr(current) +' of ' + IntToStr(total) + '), please be patient...';
  ProgressChanged(progress);
  Application.ProcessMessages();
end;

procedure TMainForm.Button_BrowseClick(Sender: TObject);
begin
  if OpenDialog.Execute then
  begin
    Edit_FileDigest.Text := '';
    Edit_InputFile.Text := OpenDialog.FileName;
    ProgressBar.Position := 0;
    Button_Compute.Enabled := True;
  end;
end;

procedure TMainForm.Button_ComputeClick(Sender: TObject);
var
  thread: TComputeThread;
begin
  Button_Browse.Enabled := False;
  Button_Compute.Enabled := False;
  ProgressBar.Position := 0;
  Edit_FileDigest.Text := 'Working, please wait...';

  thread := TComputeThread.Create(Self.WindowHandle, Edit_InputFile.Text, AbortFlag);
  thread.OnProgress := ProgressChanged;
  thread.OnTerminate := ThreadTerminated;

  AbortFlag.ResetEvent();
  thread.Resume;
end;

procedure TMainForm.ProgressChanged(progress: Integer);
begin
  ProgressBar.Position := progress;
end;

procedure TMainForm.ThreadTerminated(Sender: TObject);
begin
  if Sender is TComputeThread then
  begin
    Edit_FileDigest.Text := (Sender as TComputeThread).GetResult();
    Button_Compute.Enabled := True;
    Button_Browse.Enabled := True;
  end;
end;

procedure TMainForm.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
begin
  CanClose := Button_Compute.Enabled or Button_Browse.Enabled;
end;

{----------------------------------------------}
{ Compute Thread                               }
{----------------------------------------------}

constructor TComputeThread.Create(const hwnd: HWND; const inputFile: String; const abortFlag: TSimpleEvent);
begin
  inherited Create(True);
  Self.hwnd := hwnd;
  Self.inputFile := inputFile;
  Self.abortFlag := abortFlag;
  ReturnValue := 0;
  progressEvent := nil;
end;

function TComputeThread.GetResult: String;
begin
  Result := hashString;
end;

procedure TComputeThread.Execute();
var
  digest: TMHash384;
  inputFile: File;
  buffer, result: TByteArray;
  count, spinner: Integer;
  totalSize, processed: UInt64;
begin
  inherited;
  try
    ReturnValue := 0;
    processed := 0;
    spinner := 0;

    SetLength(buffer, 4096);
    SetProgress(0, 100);
    totalSize := FileSize64(Self.inputFile);

    AssignFile(inputFile, Self.inputFile);
    FileMode := fmOpenRead;
    Reset(inputFile, 1);
    try
      digest := TMHash384.Create();
      try
        while not Eof(inputFile) do
        begin
          BlockRead(inputFile, buffer[0], Length(buffer), count);
          if count > 0 then
          begin
            digest.Update(buffer, 0, count);
            spinner := spinner + 1;
            processed := processed + count;
            if (spinner and $1FF) = 0 then
            begin
              SetProgress(processed, totalSize);
              spinner := 0;
            end;
          end;
          if (abortFlag.WaitFor(0) = wrSignaled) then
          begin
            raise Exception.Create('Operation was aborted by the user!');
          end;
        end;
        digest.Result(result);
        HashString := ByteToHex(result);
        ReturnValue := 1;
      finally
        digest.Destroy();
        SetProgress(100, 100);
      end;
    finally
      CloseFile(inputFile);
    end;
  except
     on E: Exception do
     begin
       MessageBox(self.hwnd, PAnsiChar('Failed to compute hash, because an error occurred!'#10#10'Details:'#10 + E.Message), PAnsiChar(String(E.ClassName)), MB_ICONERROR or MB_SYSTEMMODAL);
       Exit;
     end;
  end;
end;

function TComputeThread.FileSize64(const fileName: string): UInt64;
var
  F: TSearchRec;
begin
  Result := 0;
  if FindFirst(fileName, faAnyFile, F) = 0 then
  begin
    try
      Result := UInt64(F.FindData.nFileSizeLow) or (UInt64(F.FindData.nFileSizeHigh) shl 32);
    finally
      FindClose(F);
    end;
  end;
end;

procedure TComputeThread.UpdateProgress;
begin
  if Assigned(progressEvent) then
  begin
    progressEvent(progressValue);
  end;
end;

procedure TComputeThread.SetProgress(const processed: UInt64; const totalSize: UInt64);
var
  nextProgress: Integer;
  totalSizeDbl, processedDbl: Double;
begin
  nextProgress := 0;
  if (processed > 0) and (totalSize > 0) then
  begin
    totalSizeDbl := totalSize;
    processedDbl := processed;
    nextProgress := Round((processedDbl / totalSizeDbl) * 100.0);
  end;
  if nextProgress <> progressValue then
  begin
    progressValue := nextProgress;
    Synchronize(updateProgress);
  end;
end;

function TComputeThread.ByteToHex(input: TByteArray):String;
const
  digits:array[0..15] of char='0123456789ABCDEF';
var
  i: Cardinal;
begin
  SetLength(Result, 0);
  for i := 0 to Length(input)-1 do
  begin
    Result := Result + digits[input[i] shr 4] + digits[input[i] and $0F];
  end;
end;

end.
