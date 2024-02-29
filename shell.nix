{ pkgs ? import <nixpkgs> {} }: {
  default = pkgs.mkShell {
    name = "kiddy";
    nativeBuildInputs = with pkgs; [
      git gcc gnumake binutils utillinux pahole
    ];
    shellHook = ''
      export KDIR="$(nix-build -E '(import <nixpkgs> {}).linux.dev' --no-out-link)/lib/modules/*/build"
    '';
  };
}
