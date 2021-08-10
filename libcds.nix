{ lib
, stdenv
, fetchFromGitHub
, cmake
}:

stdenv.mkDerivation rec {
  pname = "libcds";
  version = "2.3.3";

  src = fetchFromGitHub {
    owner = "khizmax";
    repo = "libcds";
    rev = "v${version}";
    sha256 = "1nfb662x2993gr3iyaxcb9gcss4b6jxam0x0qr360vr98n6gm4f9";
  };

  nativeBuildInputs = [ cmake ];
}
