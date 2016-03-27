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
  TMHash384Ver = record
    Major: LongWord;
    Minor: LongWord;
    Patch: LongWord;
  end;

type
  TMHash384 = Class
  public
    constructor Create();
    class procedure GetVer(var Version: TMHash384Ver);
  private
    handle: Pointer;
  end;

{===========================================================================}
{ IMPLEMENTATION                                                            }
{===========================================================================}

implementation

{----------------------------------------------}
{ Native Functions                             }
{----------------------------------------------}

const MHash384Lib = 'MHashDelphi384.dll';

function mhash384_native_create(): Pointer; cdecl; external MHash384Lib name 'mhash384_create';
procedure mhash384_native_getver(major: PLongWord; minor: PLongWord; patch: PLongWord); cdecl; external MHash384Lib name 'mhash384_getver';

{----------------------------------------------}
{ Public Functions                             }
{----------------------------------------------}

constructor TMHash384.Create();
begin
  handle := mhash384_native_create();
end;

class procedure TMHash384.GetVer(var Version: TMHash384Ver);
begin
  mhash384_native_getver(@Version.major, @Version.minor, @Version.patch);
end;

end.

