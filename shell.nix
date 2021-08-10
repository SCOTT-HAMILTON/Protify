{ pkgs ? import <nixpkgs> {} }:
with pkgs; mkShell {
  buildInputs = [
    qtcreator
    cppzmq
    zeromq
    qt5.full
    avahi
  ];
  shellHook = ''
    qtcreator Protify.pro
  '';
}

