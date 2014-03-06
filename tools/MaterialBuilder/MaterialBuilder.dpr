program MaterialBuilder;

uses
  Forms,
  UMain in 'UMain.pas' {Main};

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TMain, Main);
  Application.Run;
end.
