{ pkgs ? import <nixpkgs> {} }:
let
  settingspath = with pkgs; callPackage
    ({ stdenvNoCC }:
    stdenvNoCC.mkDerivation rec {
      pname = "qtcreator-settingspath";
      version = "unstable";
      dontUnpack = true;
      qtversion = ./qtcreator/qtversion.xml;
      qtfull = qt5.full;
      postPatch = ''
        cp "$qtversion" qtversion.xml
        substituteAllInPlace qtversion.xml
      '';
      installPhase = ''
        runHook preInstall
        install -Dm 644 qtversion.xml "$out/QtProject/qtcreator/qtversion.xml"
        runHook postInstall
      '';
    }) {};
in
with pkgs; (mkShell.override { stdenv = stdenvNoCC; }) rec {
  libs = [
    cppzmq
    zeromq
    avahi
  ];
  nativeBuildInputs = [
    gcc10
  ];
  buildInputs = [
    qtcreator
    qt5.full
    qt5.qtdeclarative
    qt5.qtquickcontrols2
  ] ++ libs;
  NIX_INCLUDEPATH = lib.concatMapStringsSep " " (p: p+"/include")
    (map (drv: toString drv) libs);
  shellHook = ''
    tempdir=$(mktemp -d)
    cp -r --no-preserve=mode,ownership "${settingspath}" "$tempdir/settingspath"
    echo $NIX_INCLUDEPATH
    qtcreator -theme dark -settingspath "$tempdir/settingspath" Protify.pro
    rm -rf $tempdir
    echo "tempdir is $tempdir"
  '';
}

