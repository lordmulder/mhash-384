{* ---------------------------------------------------------------------------------------------- *}
{* MHash-384 - Simple fast portable secure hashing library                                        *}
{* Copyright(c) 2016-2020 LoRd_MuldeR <mulder2@gmx.de>                                            *}
{*                                                                                                *}
{* Permission is hereby granted, free of charge, to any person obtaining a copy of this software  *}
{* and associated documentation files (the "Software"), to deal in the Software without           *}
{* restriction, including without limitation the rights to use, copy, modify, merge, publish,     *}
{* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  *}
{* Software is furnished to do so, subject to the following conditions:                           *}
{*                                                                                                *}
{* The above copyright notice and this permission notice shall be included in all copies or       *}
{* substantial portions of the Software.                                                          *}
{*                                                                                                *}
{* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  *}
{* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     *}
{* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *}
{* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *}
{* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        *}
{* ---------------------------------------------------------------------------------------------- *}

program MHash384Test;

{$APPTYPE CONSOLE}

uses
  SysUtils,
  StrUtils,
  MHash384 in '..\libmhash384\MHash384.pas';

{ --------------------------------------------------------------------------- }
{ TEST DATA                                                                   }
{ --------------------------------------------------------------------------- }

type
  TTestVector = record
    count: LongWord;
    text: String;
  end;

const
  TestVectors: array[0..11] of TTestVector =
  (
    ( count: $00000001; text: '' ),
    ( count: $00000001; text: 'abc' ),
    ( count: $00000001; text: 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq' ),
    ( count: $00000001; text: 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu' ),
    ( count: $000186A0; text: 'aaaaaaaaaa' ),
    ( count: $01000000; text: 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno' ),
    ( count: $00000001; text: 'The quick brown fox jumps over the lazy dog' ),
    ( count: $00000001; text: 'The quick brown fox jumps over the lazy cog' ),
    ( count: $00000001; text: 'Franz jagt im komplett verwahrlosten Taxi quer durch Bayern' ),
    ( count: $00000001; text: 'Frank jagt im komplett verwahrlosten Taxi quer durch Bayern' ),
    ( count: $00000001; text: 'Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. ' + 'Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. ' + 'Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.' ),
    ( count: $00000001; text: 'Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. ' + 'Ut enim ad minim veniam, quis nostrud exercitation ullamc0 laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. ' + 'Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.' )
  );

  Expected: array[0..11] of TMHash384Digest =
  (
    ( $D6, $29, $CB, $37, $F7, $61, $56, $62, $1E, $09, $C9, $3B, $8D, $10, $29, $C5, $F0, $F7, $B7, $78, $DE, $5B, $D9, $57, $82, $30, $67, $3F, $E7, $FA, $4C, $45, $A2, $30, $8A, $B4, $E9, $D9, $9A, $A0, $EA, $19, $4C, $BF, $62, $30, $C2, $02 ),
    ( $1D, $7B, $15, $93, $D7, $AA, $4D, $D7, $DC, $8D, $A6, $D6, $BD, $59, $1D, $8B, $50, $7C, $A5, $14, $E1, $C7, $EC, $37, $F4, $3E, $78, $DD, $6B, $83, $25, $3C, $35, $54, $03, $F6, $10, $09, $58, $28, $90, $48, $0C, $4C, $D5, $27, $F4, $05 ),
    ( $9B, $98, $CE, $4C, $3F, $4E, $A9, $01, $8E, $1E, $D3, $03, $47, $17, $E3, $E5, $D8, $DA, $20, $AB, $37, $5B, $AF, $E3, $72, $D0, $77, $80, $FF, $DE, $FD, $A3, $2B, $A3, $C8, $22, $15, $5E, $28, $83, $E3, $33, $AF, $CA, $AE, $AF, $A6, $14 ),
    ( $45, $41, $6D, $1F, $FD, $44, $D3, $94, $0C, $5A, $F5, $B4, $25, $B5, $DA, $07, $FD, $8C, $06, $BA, $B7, $C5, $8C, $D1, $33, $89, $BC, $28, $46, $50, $ED, $60, $90, $A9, $A1, $95, $2C, $29, $FE, $B1, $B2, $C6, $AF, $BF, $5F, $99, $61, $08 ),
    ( $20, $77, $4C, $57, $0A, $E2, $E6, $A4, $9A, $01, $DA, $E9, $48, $67, $2A, $E9, $4D, $46, $5D, $91, $67, $13, $0F, $E8, $7B, $F2, $41, $48, $64, $9C, $F1, $21, $E8, $A5, $07, $59, $50, $7B, $F9, $71, $34, $F0, $D2, $8C, $EE, $91, $DC, $A2 ),
    ( $C1, $88, $43, $D4, $DD, $9E, $04, $79, $D3, $8A, $55, $E8, $C7, $5E, $22, $01, $F0, $E9, $3B, $6E, $F1, $F9, $16, $EB, $AF, $3C, $40, $74, $D1, $88, $94, $5C, $7B, $96, $57, $F4, $52, $69, $A1, $DA, $AB, $4A, $8F, $B6, $F6, $3D, $0B, $EE ),
    ( $E2, $90, $1E, $DB, $76, $AA, $83, $F2, $D5, $2F, $EA, $9A, $34, $EB, $2B, $C0, $5E, $DF, $A9, $57, $1C, $81, $26, $20, $24, $5B, $BE, $BA, $95, $FA, $2D, $82, $CB, $B5, $BA, $D9, $3E, $B6, $4D, $4B, $72, $B4, $66, $35, $D5, $65, $40, $0B ),
    ( $8B, $81, $CD, $9C, $1D, $EE, $8B, $CC, $64, $2F, $8B, $1E, $A9, $82, $CE, $79, $D1, $5F, $50, $E6, $69, $39, $FF, $34, $29, $4E, $63, $D9, $3A, $E3, $C6, $10, $F9, $46, $4F, $EF, $EF, $30, $29, $74, $09, $B6, $22, $31, $F0, $28, $AB, $C0 ),
    ( $A5, $45, $E6, $31, $9F, $6A, $CE, $0C, $EF, $62, $BC, $AA, $F5, $D4, $88, $85, $87, $51, $B9, $6C, $2A, $48, $8D, $5B, $E6, $EC, $63, $D5, $00, $06, $C9, $D5, $4D, $AD, $66, $16, $84, $88, $F0, $3B, $4A, $86, $8D, $3B, $A6, $10, $CC, $F8 ),
    ( $2B, $EC, $B8, $9D, $D5, $5F, $C9, $F2, $F3, $38, $4D, $F6, $1D, $62, $40, $DD, $A5, $4F, $C2, $75, $D1, $86, $A4, $5A, $88, $3C, $03, $AA, $87, $3D, $DA, $F4, $FA, $D4, $74, $42, $12, $B9, $2D, $26, $05, $4B, $92, $85, $2D, $EA, $5F, $1D ),
    ( $62, $83, $CE, $91, $DA, $8A, $F5, $09, $1C, $7B, $06, $E1, $35, $2D, $C9, $47, $B9, $E3, $B8, $96, $14, $80, $7C, $04, $C4, $3D, $A3, $22, $32, $1F, $BA, $DA, $D3, $CC, $CA, $BE, $CB, $5C, $1B, $1B, $26, $54, $32, $A6, $03, $91, $21, $E1 ),
    ( $5D, $CB, $7C, $4B, $27, $BA, $84, $16, $88, $FB, $6A, $86, $0E, $97, $A4, $52, $56, $91, $75, $33, $F6, $34, $48, $CF, $66, $67, $81, $EA, $4B, $5F, $AB, $20, $E1, $C7, $46, $C8, $82, $10, $99, $A9, $15, $22, $2E, $0D, $77, $62, $40, $98 )
  );

{ --------------------------------------------------------------------------- }
{ UTILITY FUNCTIONS                                                           }
{ --------------------------------------------------------------------------- }

function BytesToHex(input: PMHash384Digest): String;
const
  Digits: array[0..15] of char = '0123456789ABCDEF';
var
  i: Integer;
begin
  Result := '';
  for i := 0 to High(input^) do
  begin
    Result := Result + digits[input^[i] shr 4] + digits[input^[i] and $0F];
  end;
end;

function RunTest(const count: LongWord; const text: String; const expected: PMHash384Digest): Boolean;
var
  hash: TMHash384;
  digest: TMHash384Digest;
  i: LongWord;
begin
  if count = 1 then
  begin
    digest := TMHash384.Compute(text);
  end
  else begin
    hash := TMHash384.Create();
    for i := 0 to count-1 do
    begin
      hash.Update(text);
    end;
    digest := hash.GetDigest();
    FreeAndNil(hash);
  end;
  Result := CompareMem(@digest, expected, MHASH384_SIZE);
  WriteLn(Format('%s - %s', [BytesToHex(@digest), IfThen(Result, 'OK', 'Error!')]));
end;

{ --------------------------------------------------------------------------- }
{ MAIN FUNCTION                                                               }
{ --------------------------------------------------------------------------- }

var
  i: Integer;
  success: Boolean;
begin
{$IFNDEF FPC}
  DecimalSeparator := '.';
  WriteLn(Format('MHash-384 v%d.%.2d-%d [Delphi Compiler %f]' + sLineBreak, [MHASH384_VERSION_MAJOR, MHASH384_VERSION_MINOR, MHASH384_VERSION_PATCH, CompilerVersion]));
{$ELSE}
  {$MACRO ON}
  WriteLn(Format('MHash-384 v%d.%.2d-%d [Free Pascal Compiler %d.%d.%d]' + sLineBreak, [MHASH384_VERSION_MAJOR, MHASH384_VERSION_MINOR, MHASH384_VERSION_PATCH, FPC_FULLVERSION div 10000, (FPC_FULLVERSION mod 10000) div 100, FPC_FULLVERSION mod 100]));
{$ENDIF}
  success := True;
  for i := 0 to High(TestVectors) do
  begin
    if not RunTest(TestVectors[i].count, TestVectors[i].text, @Expected[i]) then
    begin
      success := False;
    end;
  end;
  if success then
  begin
    WriteLn(sLineBreak + 'Self-test completed successfully :-)');
  end
  else begin
    WriteLn(sLineBreak + 'Error: Self-test has failed! :-(');
  end;
end.

