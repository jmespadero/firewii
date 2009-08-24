(* Fuentes entregados como premio a una colaboraci¢n o por solicitar la *)
(* lecci¢n por correo. Por favor NO DISTRIBUIR, si incumples esta regla *)
(* consegiras que nunca m s se hagan proyectos como el CPV. Gracias por *)
(* tu colaboraci¢n. *)

program FX_Fueg2;                     (* Programa demostraci¢n creado para*)
                                      (* el CPV por JASM-BMP.*)
                                      (* Utilizaci¢n regulada seg£n las *)
                                      (* condiciones expuestas en la lecci¢n*)

uses crt,dos;                         (* Unidades que utilizo*)

Const LADO_CUADRADO = 5;

type
    TipoColor=record                  (* Tipo Color de la paleta*)
               Rojo:byte;
               Verde:byte;
               Azul:byte;
              end;
    TipoPaleta=array[0..255] of TipoColor;  (* Tipo para la paleta de 256*)
    TipoPantalla= Array[1..64000] of Byte;  (* Tipo para la pantalla virtual*)

var
    Paleta:TipoPaleta;                (* Variable para almacenar la paleta *)
    PantallaVirtual:^TipoPantalla;    (* Pantalla virtual *)
    Ch :char;                         (* Para el readkey *)
    Cadencia:integer;                 (* Almacenar  la cadencia del fuego *)
    i,j:integer;                      (* Indices para los bucles *)

(* Cambia a modo gr fico 13h *)
procedure Modo13h;assembler;
asm
    mov ah,0
    mov al,13h
    int 10h
end;

(* Cambia a modo texto*)
procedure ModoTexto;assembler;
asm
    mov ah,0
    mov al,03h
    int 10h
end;

(* Inicializa la paleta pasada como par metro *)
procedure InicializarPaleta(Paleta:TipoPaleta);
var r:registers;
begin
  r.ax:=$1012;
  r.bx:=0;
  r.cx:=256;
  r.es:=seg(Paleta);
  r.dx:=ofs(Paleta);
  intr($10,r);
end;

(*Procemiento que lee la imagen y la representa en pantalla*)
procedure VisualizaImagen;
var f:file;                            (*Fichero general*)
    Buffer:array[1..32] of byte;
begin
  assign(f,'fondo.pic');               (*Inicializa identificador de fichero*)
  reset(f,1);                          (*Abre el fichero para lectura*)
  blockread(f,Buffer,32);              (*Lee la cabecera*)
  blockread(f,Paleta,768);             (*Lee la paleta*)
  blockread(f,PantallaVirtual^,64000); (*Lee la imagen cruda*)
  move(PantallaVirtual^,mem[$a000:0000],64000);
  close(f);                            (*Cierra el fichero*)
end;

(* Visualiza un cuadro de 5x5 en las coordenadas y del color dados*)
Procedure PonCuadro(x,y:integer;Color:byte); assembler;
label bucle1,bucle2;
asm
        push ds                  (* Salva los registros de segmento*)
        push es

        lds  si,PantallaVirtual  (* ds:si direcci¢n de PantallaVirtual*)

        mov  ax,320              (* Anchura de la pantalla *)
        mul  y
        mov  bx,x
        add  ax,bx

        add  si,ax               (* En si el desplazamiento desde el origen*)
                                 (* de la pantalla donde visualizar el *)
                                 (* cuadro *)
        mov  dx,LADO_CUADRADO    (* En dx anchura del cuadro *)
        mov  bx,LADO_CUADRADO    (* En bx altura *)

        mov  cx,bx               (* Al registro cx (contador) la altura del*)
                                 (* cuadro*)
bucle1:
        mov  bx,cx               (* Salvamos en bx el valor del contador*)
        mov  cx,dx               (* AL registro cx la anchura del cuadro*)
bucle2:
        mov  al, Color           (* Necesario para la siguiente instrucci¢n*)
        mov  [ds:si],al          (* Llevamos el color a la pantalla *)
        inc  si                  (* Incrementamos la posici¢n*)

        loop bucle2              (* Bucle de la anchura *)

        add  si, 320             (* Actualizamos el desplamiento para la *)
        sub  si, LADO_CUADRADO   (* siguiente l¡nea del cuadro *)

        mov  cx,bx               (* Recuperamos el contador de la altura *)

        loop bucle1              (* Bucle de la altura 1 *)

        pop  es                  (* Recuperamos los registros de segmento *)
        pop  ds
end;

(* Devuelve el Color del punto de la pantalla virtual en las coordenadas*)
(* dadas *)
function CogePunto(x,y:integer):byte;
begin
 CogePunto:=PantallaVirtual^[y*320+x];
end;

(* Genera los 200 colores fuego al comienzo de la paleta de la imagen *)
procedure GeneraPaleta;
var Conta:integer;
    i:integer;
begin
 Conta:=0;                              (* Inicializamos las variables *)
 i:=0;
 repeat                                 (* Primer Bloque *)

  if not odd(i) then                    (* Generamos colores de 2 en 2 *)
   Conta:=(Conta mod 63)+1;

  paleta[i].Rojo:=Conta;                (* Componentes del color*)
  paleta[i].Azul:=0;

  if Conta>30 then                      (* Componente de color condicionado*)
   paleta[i].Verde:=Conta-30
  else
   paleta[i].Verde:=0;

  i:=i+1;
 until i=126;                           (* 125 colores *)

 Conta:=30;                             (* Actualizamos el contador *)

 repeat                                 (* Segundo bloque *)

  if not odd(i) then                    (* Controla que la componente roja*)
   if Conta<63 then                     (* no se dispare *)
    Conta:=(Conta mod 63)+1
   else
    Conta:=63;

  paleta[i].Rojo:=63;                   (* Componentes de cada color *)
  paleta[i].Verde:=Conta;
  paleta[i].Azul:=0;

  i:=i+1;

 until i=200;                           (* Hasta el color 199 *)

end;

(* Genera la chispa del fuego virtual en cada proceso visualizando en la *)
(* £ltima l¡nea del mismo un cuadro del primer o £ltimo color de la paleta *)
(* fuego *)
procedure FondoPantalla;
var i:integer;
begin
 for i:=14 to 51 do         (* Ancho del fuego, m s un cuadro a cada lado *)
  if random(2)=0 then
   PonCuadro(i*LADO_CUADRADO,160-LADO_CUADRADO,0)   (* Primer color *)
  else
   PonCuadro(i*LADO_CUADRADO,160-LADO_CUADRADO,199) (* Ultimo color *)
end;

(*Procedimiento que se lleva parte de la pantalla virtual a la pantalla real
(* de una tacada. *)
procedure VolcarPantalla; assembler;
label bucle1,bucle2;
asm
        push ds                    (* Salva los registros de segmento*)
        push es

        lds  si, PantallaVirtual   (* Ponemos en ds:si la direcci¢n de la *)
                                   (* pantalla virtual*)

        mov  ax,0A000h             (* En es:di la direcci¢n de la memoria de*)
        mov  es,ax                 (* v¡deo*)
                                   (*11200*)
        mov  si,8000              (* Comienzo del bloque que nos vamos a *)
        mov  di,8000              (* llevar a la pantalla real. L¡nea 35*)
                                   (*18400*)
        mov  cx,20000              (* Tama¤o del bloque que nos vamos a *)
                                   (* llevar. 115 l¡neas*)

        cld                        (* Indica que se incremente los registros*)
                                   (* de desplazamiento*)

        rep  movsw                 (* Mueve el n£mero de words indicado en*)
                                   (* cx de ds:si a es:di *)

        pop  es                    (* Recupera los registros de segmento*)
        pop  ds
end;

(* Realiza el filtrado de imagen de la pantalla virtual seg£n se explica en*)
(* art¡culo de la lecci¢n *)
procedure HazMedia;
var i,j:integer;                            (* Indices de los bucles *)
    Acumulador:integer;
begin
 for j:=30 downto 5 do                      (* Altura del fuego *)
  begin
   for i:=15 to 50 do                       (* Ancho del fuego *)
    begin
     Acumulador:=CogePunto((i+1)*LADO_CUADRADO,(j+1)*LADO_CUADRADO);
     Acumulador:=Acumulador+CogePunto(i*LADO_CUADRADO,(j+1)*LADO_CUADRADO);
     Acumulador:=Acumulador+CogePunto((i-1)*LADO_CUADRADO,(j+1)*LADO_CUADRADO);
     Acumulador:=Acumulador+CogePunto((i+1)*LADO_CUADRADO,j*LADO_CUADRADO);
     Acumulador:=Acumulador+CogePunto(i*LADO_CUADRADO,j*LADO_CUADRADO);
     Acumulador:=Acumulador+CogePunto((i-1)*LADO_CUADRADO,j*LADO_CUADRADO);

     Acumulador:=Acumulador div 6;          (* Media Aritm‚tica *)

     if (Acumulador > Cadencia - 1) then    (* Disminuye el valor resultado *)
      Acumulador:=Acumulador - Cadencia;    (* para acelerar la cadencia *)

     PonCuadro((i-1)*LADO_CUADRADO,j*LADO_CUADRADO,Acumulador);
                                            (* Cuadro a cuadro.... *)
    end;
  end;
end;

(* Muestra la introducci¢n y los cr‚ditos de los autores *)
procedure Creditos;
begin
 clrscr;
 writeln('Programa Fx-Fueg2 creado para el CPV, como demostraci¢n de',
         ' la realizaci¢n');
 writeln('de distintos efectos especiales (Fuego) en el modo 13h.');
 writeln;
 writeln('Por JASM-BMP.');
 writeln;
 writeln('El efecto mostrado en esta demostraci¢n se conoce como Fuego ',
         'virtual y su');
 writeln('objetivo es simular al fuego a base de tratamientos gr ficos y',
         ' trucos de ');
 writeln('programaci¢n.');

 writeln;
 writeln('Pulsa la tecla Q para avivar el fuego.');
 writeln('Pulsa la tecla A para hacer de bombero. ;)');
 writeln;
 writeln('Pulsa Escape cuando quieras salir de la demostraci¢n.');
 writeln;
 writeln('Pulsa una tecla para continuar.');
 Ch:=readkey;
end;

begin                                   (* Programa Principal *)
 Creditos;
 new(PantallaVirtual);                  (* Asigna memoria a la p.virtual *)
 Modo13h;                               (* Cambiamos a modo 13h *)
 randomize;                             (* Inicializa la semilla para los *)
                                        (* n£meros aleatorios *)
 VisualizaImagen;                       (* Visualiza el fondo del efecto *)
 GeneraPaleta;                          (* Crea la paleta fuego *)
 InicializarPaleta(Paleta);             (* Inicializa la paleta del programa*)

 Cadencia := 2;
 repeat                                 (* Bucle principal del efecto *)
  FondoPantalla;                        (* Proceso en la £ltimo l¡nea *)
  HazMedia;                             (* Filtro average *)
  VolcarPantalla;                       (* Volcado de la pantalla virtual *)

  if keypressed then                    (* Si se puls¢ una tecla *)
   begin
    ch:=readkey;                                   (* La leemos *)
    if (upcase(ch) = 'A') and (Cadencia<20) then
     Cadencia := Cadencia +1;                      (* Atenua fuego *)
    if (upcase(ch) = 'Q') and (Cadencia>0) then
     Cadencia := Cadencia -1;                      (* Aviva fuego *)
   end;
 until ord(ch) = 27;                     (* Hasta que se pulse escape *)

 i:=Cadencia;                            (* Atenuamos el fuego hasta su *)
 for Cadencia:=1 to 10 do                (* desaparici¢n *)
  begin
   for j:= 1 to 5 do                     (* Damos tiempo a que se vea *)
    begin
     FondoPantalla;
     Hazmedia;
     VolcarPantalla;
    end;
  end;

 ModoTexto;                             (* Cambiamos a modo texto *)
 dispose(PantallaVirtual);              (* Libera memoria asignada *)
 writeln('Puedes ejecutar este programa desde las lecciones del CPV. ');
end.
