{
  description = "C++ project using Clang's libtooling";

  inputs.nixpkgs.url = "nixpkgs/nixos-unstable";
  inputs.graaf = {
    url = "github:bobluppes/graaf";
    flake = false;
  };

  outputs = { self, nixpkgs, graaf }: let
    llvm = nixpkgs.legacyPackages.x86_64-linux.llvmPackages_17;
  in {
    defaultPackage.x86_64-linux = with nixpkgs.legacyPackages.x86_64-linux; llvm.stdenv.mkDerivation {
      name = "outputcc";

      src = ./.;

      buildInputs = [ llvm.llvm llvm.libclang ];
      nativeBuildInputs = [ cmake pkg-config makeWrapper ];

      cmakeFlags = [
        "-DGRAAF_INCLUDE_DIRS=${graaf.outPath}/include"
      ];

      meta = {
        description = "Tool to enhance compilation databases with header files";
      };

      postInstall = ''
        source ${./nix-support/wrap}

        wrapProgram $out/bin/outputcc \
        --suffix PATH               : "${llvm.clang}/bin/"                            \
        --suffix CPATH              : $(buildcpath $${NIX_CFLAGS_COMPILE} $(<${llvm.clang}/nix-support/libc-cflags))     \
        --suffix CPATH              : ${llvm.clang}/resource-root/include          \
        --suffix CPLUS_INCLUDE_PATH : $(buildcpath $${NIX_CFLAGS_COMPILE} $(<${llvm.clang}/nix-support/libcxx-cxxflags)) \
        --suffix CPLUS_INCLUDE_PATH : $(buildcpath $${NIX_CFLAGS_COMPILE} $(<${llvm.clang}/nix-support/libc-cflags)) \
        --suffix CPLUS_INCLUDE_PATH : ${llvm.clang}/resource-root/include
      '';
    };
  };
}
