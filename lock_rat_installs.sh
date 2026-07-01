#!/usr/bin/env bash
set -euo pipefail

INSTALL_ROOT=/data/snoplus/software/snocave_Alma9
DRY_RUN=0

usage() {
  cat <<'USAGE'
Usage: lock_rat_installs.sh [options]

Set shared RAT installs to be readable/executable by everyone, but writable
only by the owner. Applies to rat and rat-* directories in the install root,
plus set_env.sh if present.

Options:
  --root DIR   Install root (default: /data/snoplus/software/snocave_Alma9)
  --dry-run    Print commands without running them
  -h, --help   Show this help
USAGE
}

die() {
  echo "error: $*" >&2
  exit 1
}

run() {
  printf '+'
  printf ' %q' "$@"
  printf '\n'
  if [[ "$DRY_RUN" -eq 0 ]]; then
    "$@"
  fi
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --root)
      [[ $# -ge 2 ]] || die "--root requires a directory"
      INSTALL_ROOT=$2
      shift 2
      ;;
    --dry-run)
      DRY_RUN=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      die "unknown option: $1"
      ;;
  esac
done

[[ -d "$INSTALL_ROOT" ]] || die "install root does not exist: $INSTALL_ROOT"

rat_dirs=()
if [[ -d "$INSTALL_ROOT/rat" ]]; then
  rat_dirs+=("$INSTALL_ROOT/rat")
fi
while IFS= read -r -d '' dir; do
  rat_dirs+=("$dir")
done < <(find "$INSTALL_ROOT" -mindepth 1 -maxdepth 1 -type d -name 'rat-*' -print0 | sort -z)

[[ ${#rat_dirs[@]} -gt 0 ]] || die "no rat or rat-* directories found in $INSTALL_ROOT"

echo "Install root: $INSTALL_ROOT"
echo "RAT dirs:"
printf '  %s\n' "${rat_dirs[@]}"
echo

for dir in "${rat_dirs[@]}"; do
  echo "==> $dir"
  run find "$dir" -type d -exec chmod 755 {} +
  run find "$dir" -type f -exec chmod a+r,go-w {} +
  run find "$dir" -type f -perm /111 -exec chmod 755 {} +
  echo
done

if [[ -e "$INSTALL_ROOT/set_env.sh" ]]; then
  echo "==> $INSTALL_ROOT/set_env.sh"
  run chmod 755 "$INSTALL_ROOT/set_env.sh"
else
  echo "No set_env.sh found in $INSTALL_ROOT"
fi
