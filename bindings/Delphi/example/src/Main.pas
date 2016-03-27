{ ---------------------------------------------------------------------------------------------- }
{ MHash-384 - Language bindings for Delphi                                                       }
{ Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                 }
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

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, MHash384, StdCtrls;

type
  TForm1 = class(TForm)
    Button1: TButton;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private-Deklarationen }
  public
    { Public-Deklarationen }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.Button1Click(Sender: TObject);
var
  mhash: TMHash384;
begin
  mhash := TMHash384.Create();
end;

procedure TForm1.FormCreate(Sender: TObject);
var
  Version: TMHash384Ver;
begin
  FillChar(Version, SizeOf(TMHash384Ver), #0);
  TMHash384.GetVer(Version);
  Caption := 'MHashDelphi384 - Example App v' + Format('%d.%d.%d', [Version.Major, Version.Minor, Version.Patch]);
end;

end.
