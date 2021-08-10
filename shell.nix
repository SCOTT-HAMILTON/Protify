{ pkgs ? import <nixpkgs> {} }:
let
  libcds = pkgs.callPackage ./libcds.nix { };
in
with pkgs; mkShell {
  buildInputs = [
    qtcreator
    cppzmq
    zeromq
    qt5.full
    avahi
    libcds
  ];
  shellHook = ''
    qtcreator Protify.pro
  '';
}

