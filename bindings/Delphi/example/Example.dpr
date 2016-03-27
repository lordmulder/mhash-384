program Example;

uses
  Forms,
  Main in 'src\Main.pas' {MainForm},
  MHash384 in '..\wrapper\MHash384.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
