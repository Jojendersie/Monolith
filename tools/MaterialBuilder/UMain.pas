unit UMain;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, Math, ComCtrls, StdCtrls;

  
type
  vec2 = record
    x,y: Real;
  end;

type
  vec3 = record 
    x,y,z: Real;
  end;
  
type
  TMain = class(TForm)
    Preview: TImage;
    Timer1: TTimer;
    ERed: TEdit;
    LRed: TLabel;
    EGreen: TEdit;
    LGreen: TLabel;
    EBlue: TEdit;
    LBlue: TLabel;
    ESpec: TEdit;
    LSpec: TLabel;
    EShin: TEdit;
    LShin: TLabel;
    RBTransparent: TRadioButton;
    RBRotating: TRadioButton;
    EVar: TEdit;
    LEmissive: TLabel;
    CBEmissive: TCheckBox;
    Label1: TLabel;
    EResult: TEdit;
    Imp: TButton;
    procedure Timer1Timer(Sender: TObject);
    procedure ERedChange(Sender: TObject);
    procedure EVarChange(Sender: TObject);
    procedure CBEmissiveClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormKeyPress(Sender: TObject; var Key: Char);
    procedure PreviewMouseMove(Sender: TObject; Shift: TShiftState; X,
      Y: Integer);
    procedure EResultChange(Sender: TObject);
  private
    { Private-Deklarationen }
    m_y, m_pb, m_pr: Integer;
    shini, spec, theta, phi, colrot: Real;
    em, m_transparent: Integer;
    Backbuffer: TBitmap;
    procedure UpdatePreview(time: Real);
    procedure RecalculateResult();
    function Lightning({_position, }_normal, _viewDir, _color: vec3; _shininess, _power: Real) : vec3;
  public
    { Public-Deklarationen }
  end;


var
  Main: TMain;

implementation

{$R *.DFM}

function RGB2TColor(R, G, B: Real): Integer;
begin
  Result := round(R) or (round(G) shl 8) or (round(B) shl 16);
end;


function dot(_a, _b: vec3): Real;
begin
  Result := _a.x*_b.x + _a.y*_b.y + _a.z*_b.z;
end;

function normalize(_a: vec3): vec3;
var len: Real;
begin
  len := 1.0/sqrt(dot(_a,_a));
  Result.x := _a.x * len;
  Result.y := _a.y * len;
  Result.z := _a.z * len;
end;

function add(_a, _b: vec3): vec3;
begin
  Result.x := _a.x + _b.x;
  Result.y := _a.y + _b.y;
  Result.z := _a.z + _b.z;
end;

function AshikminShirleyMod(_normal, _view, _light: vec3; _shininess, _power: Real): vec2;
var
  H: vec3;
  NdotL, NdotV, HdotL, NdotH: Real;
begin
	H := normalize(add(_light, _view));
	NdotL := min(1, max(0, dot(_normal, _light)));
	NdotV := min(1, max(0, dot(_normal, _view)));
	HdotL := dot(H, _light);
	NdotH := min(1, max(0, dot(_normal, H)));

	// Modified Ashikhmin-Shirley diffuse part
	Result.x := //(1-_shininess) *
                    0.387507688 * (1.0 - power(1.0 - NdotL * 0.5, 5.0)) * (1.0 - power(1.0 - NdotV * 0.5, 5.0));
        Result.x := max(0, 3.14 * Result.x );

	// Modified Ashikhmin-Shirley specular part
	Result.y := (_power+1) * power(NdotH, _power) / (25.132741229 * HdotL * max(NdotL, NdotV)+0.001);
	//Result.y := (_power+1) * power(NdotH, _power) / 25.132741229;
	Result.y := Result.y * (_shininess + (1-_shininess) * (power(1.0 - HdotL, 5.0)));
        Result.y := max(0, 3.14 * Result.y);
end;

// Compute the lighting for all existing lights
function TMain.Lightning({_position, }_normal, _viewDir, _color: vec3; _shininess, _power: Real) : vec3;
var light: vec3;
  ds: vec2;
begin
	// Test with
	//light.x := 0.57735; light.y := -0.57735; light.z := 0.57735;
        light.z := sin(theta) * cos(phi);
        light.y := sin(theta) * sin(phi);
        light.x := cos(theta);

	// Compute BRDF for this light
	ds := AshikminShirleyMod(_normal, _viewDir, light, _shininess, _power);
	// Combine lighting
	// TODO: Light color and emissive
        Result.x := min(255, max(0, (ds.x+em) * _color.x + ds.y));
        Result.y := min(255, max(0, (ds.x+em) * _color.y + ds.y));
        Result.z := min(255, max(0, (ds.x+em) * _color.z + ds.y));
end;


procedure TMain.UpdatePreview(time: Real);
var x,y: Integer;
  dist, yt: Real;
  l, color: vec3;
  n, v: vec3;
begin
  // Decode for correct preview
  if m_transparent = 0 then
    yt := m_y * ((sin(time) * 0.5 + 0.5) * colrot + 1 - colrot)
  else yt := y;
  color.x := min(255,max(0, yt + 0.22627 * (m_pb-15) + 11.472 * (m_pr-15)));
  color.y := min(255,max(0, yt - 3.0268 * (m_pb-15) - 5.8708 * (m_pr-15)));
  color.z := min(255,max(0, yt + 14.753 * (m_pb-15) + 0.0082212 * (m_pr-15)));

  for x:=0 to Preview.Width do
    for y:=0 to Preview.Height do begin
      dist := sqrt((x-Preview.Width*0.5) * (x-Preview.Width*0.5) + (y-Preview.Height*0.5) * (y-Preview.Height*0.5));
      dist := -(dist - Preview.Width*0.5) / (Preview.Width*0.5);
      if dist > 0 then begin
        n.x := (x-Preview.Width*0.5) / (Preview.Width*0.5);
        n.y := (y-Preview.Height*0.5) / (Preview.Height*0.5);
        n.z := sqrt(1 - n.x*n.x - n.y*n.y);
        v.x := -n.x; v.y := -n.y; v.z := 4 - n.z;
        v := normalize(v);
        l := Lightning(n, v, color, shini, spec);
        if( m_transparent = 1 ) then
           backbuffer.Canvas.Pixels[x,y] := RGB2TColor(colrot*l.x,colrot*l.y,colrot*l.z)
        else backbuffer.Canvas.Pixels[x,y] := RGB2TColor(l.x,l.y,l.z);
      end else
        backbuffer.Canvas.Pixels[x,y] := clBlack;
    end;
    Preview.Canvas.Draw(0, 0, backbuffer);
end;

procedure TMain.RecalculateResult();
var r, g, b, tp, yvar, sh, sp, code: Integer;
begin
  try
    r := strtointdef(ERed.Text, 0);
    g := strtointdef(EGreen.Text, 0);
    b := strtointdef(EBlue.Text, 0);
    yvar := strtointdef(EVar.Text, 0);
    sh := strtointdef(EShin.Text, 0);
    sp := strtointdef(ESpec.Text, 0);
  except r := 255; g := 0; b := 255; end;

  // Encode color to YPbPr:855 discretization.
  m_y := floor((r * 299000 + g * 587000 + b * 114000) / 1000000);
  m_y := min(255, m_y);
  m_pb := floor((- r * 20513 - g * 40271 + b * 60784 + 15500000) / 1000000);
  m_pr := floor((r * 60784 - g * 50899 - b * 9885 + 15500000) / 1000000);

  if CBEmissive.Checked then
    em := 1
  else em := 0;

  if RBTransparent.Checked then
    m_transparent := 1
  else m_transparent := 0;

  // Convert codes for preview
  spec := power(2, sp) * 3;
  shini := sh / 15.0;
  colrot := yvar / 15.0;

  // Concatenate
  code := m_y or (m_pb shl 18) or (m_pr shl 23) or (em shl 17) or (m_transparent shl 16)
       or (yvar shl 28) or (sp shl 8) or (sh shl 12);
  EResult.Text := IntToHex( code, 8 );
end;

procedure TMain.Timer1Timer(Sender: TObject);
begin
  UpdatePreview(Tag * 0.25);
  Tag := Tag + 1;
end;

procedure TMain.ERedChange(Sender: TObject);
begin
  RecalculateResult();
end;

procedure TMain.EVarChange(Sender: TObject);
begin
  RecalculateResult();
end;

procedure TMain.CBEmissiveClick(Sender: TObject);
begin
  RecalculateResult();
end;

procedure TMain.FormCreate(Sender: TObject);
begin
  backbuffer := TBitmap.Create();
  backbuffer.Width := 256;
  backbuffer.Height := 256;
  RecalculateResult();
end;

procedure TMain.FormKeyPress(Sender: TObject; var Key: Char);
begin
{  case Key of
    'w': theta := theta + 0.1;
    's': theta := theta - 0.1;
    'a': phi := phi + 0.1;
    'd': phi := phi - 0.1;
  end;    }
end;

procedure TMain.PreviewMouseMove(Sender: TObject; Shift: TShiftState; X,
  Y: Integer);
begin
  if ssLeft in Shift then begin
    theta := Pi + X * Pi / 256;
    phi := Y * 2 * Pi / 256;
  end;
end;

procedure TMain.EResultChange(Sender: TObject);
var r, g, b, tp, yvar, sh, sp, code: Integer;
begin
  // Decode and fill edit fields
  code := StrToInt('$' + EResult.Text);

  m_y := code and 255;
  m_pb := (code shr 18) and 31;
  m_pr := (code shr 23) and 31;
  em := (code shr 17) and 1;
  m_transparent := (code shr 16) and 1;
  yvar := (code shr 28) and 15;
  sp := (code shr 8) and 15;
  sh := (code shr 12) and 15;

  // Decode color to rgb
  r := min(255,max(0, Ceil(m_y + 0.22627 * (m_pb-15) + 11.472 * (m_pr-15))));
  g := min(255,max(0, Ceil(m_y - 3.0268 * (m_pb-15) - 5.8708 * (m_pr-15))));
  b := min(255,max(0, Ceil(m_y + 14.753 * (m_pb-15) + 0.0082212 * (m_pr-15))));

  ERed.Text := InttoStr(r);
  EGreen.Text := InttoStr(g);
  EBlue.Text := InttoStr(b);
  ESpec.Text := InttoStr(sp);
  EShin.Text := InttoStr(sh);
  EVar.Text := InttoStr(yvar);

  CBEmissive.Checked := em = 1;
  RBTransparent.Checked := m_transparent = 1; 
end;

end.
