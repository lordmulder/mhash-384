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

unit MHash384;

{===========================================================================}
{ INTERFACE                                                                 }
{===========================================================================}

interface

type
  TByteArray = array of byte;

type
  TMHash384Ver = record
    Major: LongWord;
    Minor: LongWord;
    Patch: LongWord;
  end;

type
  TMHash384 = Class
  public
    constructor Create();
    destructor Destroy(); override;
    procedure Update(const input: TByteArray); overload;
    procedure Update(const input: TByteArray; const offset: LongWord; const len: LongWord); overload;
    procedure Result(var hash: TByteArray);
    class procedure GetVer(var Version: TMHash384Ver);
  private
    handle: Pointer;
  end;

const
  MHASH384_LEN: Cardinal = 48;

{===========================================================================}
{ IMPLEMENTATION                                                            }
{===========================================================================}

implementation

{----------------------------------------------}
{ Native Functions                             }
{----------------------------------------------}

const MHash384Lib = 'MHashDelphi384.dll';

function  mhash384_native_create(): Pointer; cdecl; external MHash384Lib name 'mhash384_create';
procedure mhash384_native_getver(major: PLongWord; minor: PLongWord; patch: PLongWord); cdecl; external MHash384Lib name 'mhash384_getver';
procedure mhash384_native_update(handle: Pointer; const buffer: PByte; offset: LongWord; len: LongWord); cdecl; external MHash384Lib name 'mhash384_update';
procedure mhash384_native_result(handle: Pointer; buffer: PByte); cdecl; external MHash384Lib name 'mhash384_result';
procedure mhash384_native_freeup(handle: Pointer); cdecl; external MHash384Lib name 'mhash384_freeup';

{----------------------------------------------}
{ Public Functions                             }
{----------------------------------------------}

constructor TMHash384.Create();
begin
  handle := mhash384_native_create();
end;

destructor TMHash384.Destroy();
begin
  if Assigned(handle) then
  begin
    mhash384_native_freeup(handle);
    handle := nil;
  end;
end;

procedure TMHash384.Update(const input: TByteArray);
begin
  Update(input, 0, Length(input));
end;

procedure TMHash384.Update(const input: TByteArray; const offset: LongWord; const len: LongWord);
begin
  mhash384_native_update(handle, @input[0], offset, len);
end;

procedure TMHash384.Result(var hash: TByteArray);
begin
  SetLength(hash, MHASH384_LEN);
  mhash384_native_result(handle, @hash[0]);
end;

class procedure TMHash384.GetVer(var Version: TMHash384Ver);
begin
  mhash384_native_getver(@Version.major, @Version.minor, @Version.patch);
end;

end.

