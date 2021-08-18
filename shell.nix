{ pkgs ? import <nixpkgs> {} }:
with pkgs; mkShell {
  buildInputs = [
    qtcreator
    cppzmq
    zeromq
    qt5.full
    avahi
    qt5.qtdeclarative
    qt5.qtquickcontrols2
  ];
  shellHook = ''
    qtcreator Protify.pro
  '';
}

