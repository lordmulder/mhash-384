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
    ( $4C, $4B, $82, $D0, $7B, $36, $8E, $1C, $22, $D0, $DE, $37, $59, $C3, $2D, $44, $DA, $71, $BE, $62, $83, $E8, $55, $0A, $24, $68, $DC, $1F, $EC, $38, $91, $9F, $7E, $DB, $6C, $1B, $A0, $83, $78, $EC, $58, $3A, $E6, $12, $AB, $0E, $02, $BA ),
    ( $91, $71, $D8, $3E, $E7, $DE, $DE, $36, $CA, $F2, $7C, $26, $44, $89, $7F, $31, $14, $A0, $F6, $7B, $6E, $91, $93, $AA, $1A, $B2, $34, $62, $EA, $81, $5E, $DE, $A5, $35, $00, $26, $71, $E0, $86, $49, $3B, $41, $A5, $28, $A2, $6F, $D8, $B3 ),
    ( $29, $0B, $D2, $16, $2C, $21, $05, $A0, $82, $41, $72, $A8, $87, $5E, $E3, $3B, $B6, $5A, $98, $DC, $09, $28, $10, $04, $41, $B4, $1B, $93, $99, $F6, $A8, $EA, $09, $79, $48, $34, $50, $4A, $3E, $81, $7D, $49, $D2, $9B, $C2, $0A, $52, $0A ),
    ( $0B, $3F, $13, $A6, $8A, $A8, $D8, $F0, $C5, $B9, $BF, $8B, $E5, $AE, $CC, $B7, $3E, $0D, $13, $73, $2C, $92, $90, $00, $6B, $6D, $C9, $39, $AD, $A7, $9C, $48, $AE, $36, $2E, $54, $5A, $06, $7D, $2C, $1F, $B0, $74, $9C, $60, $A4, $92, $43 ),
    ( $56, $22, $8E, $94, $32, $47, $1B, $09, $A7, $F6, $96, $D0, $DE, $FA, $15, $E6, $64, $D3, $E7, $AC, $D2, $7E, $2D, $39, $F8, $64, $C0, $50, $06, $F1, $F7, $70, $12, $F4, $F4, $CC, $E7, $45, $0C, $52, $B6, $C1, $CF, $AB, $84, $FA, $EC, $63 ),
    ( $3A, $19, $9A, $67, $3F, $AB, $29, $00, $AB, $80, $FE, $C1, $18, $5F, $79, $35, $9F, $EC, $44, $B8, $87, $28, $E3, $D6, $2D, $C3, $1A, $93, $6C, $62, $DB, $05, $EF, $35, $71, $6F, $ED, $30, $74, $E9, $31, $0D, $DD, $F6, $9E, $D5, $67, $1C ),
    ( $79, $F7, $6C, $A5, $3D, $52, $91, $62, $E6, $32, $15, $2E, $DE, $82, $A4, $03, $F8, $F9, $96, $DE, $AA, $00, $9C, $C5, $12, $25, $0B, $AF, $F9, $10, $AC, $24, $DF, $13, $81, $F7, $EF, $1F, $43, $DA, $C2, $6F, $63, $EE, $0C, $FF, $3C, $DF ),
    ( $8A, $2A, $58, $B2, $00, $20, $F7, $70, $0F, $FF, $62, $9B, $0D, $72, $38, $D3, $D5, $31, $1A, $C2, $A9, $AD, $A6, $06, $E6, $9A, $D7, $BE, $BF, $2B, $62, $58, $AE, $C7, $40, $80, $DE, $C0, $4A, $D5, $9F, $3B, $93, $26, $12, $1D, $FF, $66 ),
    ( $D2, $E0, $7E, $A3, $7E, $F1, $E0, $E5, $2B, $B7, $04, $DE, $C3, $33, $0C, $33, $78, $B9, $43, $FE, $24, $2C, $F3, $B0, $8B, $93, $D1, $8D, $BD, $61, $D4, $AB, $7C, $42, $E5, $81, $DB, $FD, $BF, $D2, $F5, $D8, $ED, $F8, $2C, $3B, $35, $D6 ),
    ( $E9, $7C, $79, $0B, $19, $45, $32, $A5, $9B, $C8, $40, $90, $B5, $C6, $8C, $5B, $0D, $05, $0C, $6F, $E9, $37, $AB, $DF, $48, $0C, $C1, $9C, $34, $5B, $72, $FE, $F9, $25, $D8, $3B, $F9, $B4, $2D, $1A, $8F, $57, $2A, $DE, $7A, $50, $9F, $F9 ),
    ( $A7, $72, $D7, $B9, $84, $AB, $C7, $90, $A9, $FF, $F5, $1F, $3B, $D7, $C6, $A5, $38, $44, $A2, $33, $A5, $64, $A9, $70, $87, $2C, $41, $34, $5A, $FE, $19, $98, $3B, $8D, $3C, $E3, $0B, $90, $0F, $D7, $FD, $D6, $6C, $ED, $03, $D0, $CD, $6E ),
    ( $61, $4A, $6B, $25, $BD, $67, $32, $16, $ED, $EA, $B6, $A0, $51, $A8, $B4, $86, $9F, $9A, $D8, $0C, $C5, $DD, $4A, $E6, $29, $DD, $FB, $70, $CA, $A7, $0E, $49, $D5, $1E, $70, $27, $FF, $35, $A1, $83, $A2, $78, $FE, $97, $F8, $75, $9C, $F9 )
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

