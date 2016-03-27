program Example;

uses
  Forms,
  Main in 'src\Main.pas' {Form1},
  MHash384 in 'src\MHash384.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
