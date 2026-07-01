#!/usr/bin/env bash
set -euo pipefail

INSTALL_ROOT=/data/snoplus/software/snocave_Alma9
REPO_URL=git@github.com:snoplus/rat.git
DEPS_ENV=
JOBS=1
SKIP_EXISTING=1
DRY_RUN=0
TAGS=(8.0.4 8.0.5 8.0.6 8.2.0 8.2.1)

usage() {
  cat <<'USAGE'
Usage: install_rat_tags.sh [options] [TAG...]

Clone, checkout, configure, and build RAT releases under rat-TAG directories.

Options:
  --root DIR          Install root (default: /data/snoplus/software/snocave_Alma9)
  --repo URL          RAT git repository (default: git@github.com:snoplus/rat.git)
  --deps-env FILE     Dependency environment to source before each build
                      (default: ROOT/set_env.sh if it exists)
  -j, --jobs N        Number of SCons jobs (default: 1)
  --no-skip-existing  Fail if rat-TAG already exists instead of skipping it
  --dry-run           Print commands without running them
  -h, --help          Show this help

If TAG arguments are provided, they replace the default missing tag list:
  8.0.4 8.0.5 8.0.6 8.2.0 8.2.1
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
    --repo)
      [[ $# -ge 2 ]] || die "--repo requires a URL"
      REPO_URL=$2
      shift 2
      ;;
    --deps-env)
      [[ $# -ge 2 ]] || die "--deps-env requires a file"
      DEPS_ENV=$2
      shift 2
      ;;
    -j|--jobs)
      [[ $# -ge 2 ]] || die "--jobs requires a number"
      JOBS=$2
      [[ "$JOBS" =~ ^[1-9][0-9]*$ ]] || die "--jobs must be a positive integer"
      shift 2
      ;;
    --no-skip-existing)
      SKIP_EXISTING=0
      shift
      ;;
    --dry-run)
      DRY_RUN=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    --)
      shift
      if [[ $# -gt 0 ]]; then
        TAGS=("$@")
      fi
      break
      ;;
    -*)
      die "unknown option: $1"
      ;;
    *)
      TAGS=("$@")
      break
      ;;
  esac
done

[[ ${#TAGS[@]} -gt 0 ]] || die "at least one tag is required"
[[ -d "$INSTALL_ROOT" ]] || die "install root does not exist: $INSTALL_ROOT"

if [[ -z "$DEPS_ENV" && -f "$INSTALL_ROOT/set_env.sh" ]]; then
  DEPS_ENV=$INSTALL_ROOT/set_env.sh
fi
if [[ -n "$DEPS_ENV" && ! -f "$DEPS_ENV" ]]; then
  die "dependency environment does not exist: $DEPS_ENV"
fi

for tag in "${TAGS[@]}"; do
  [[ "$tag" =~ ^[0-9]+(\.[0-9]+)+([-.][A-Za-z0-9_]+)?$ ]] || die "unexpected tag format: $tag"
done

echo "Install root: $INSTALL_ROOT"
echo "Repository:   $REPO_URL"
echo "Tags:         ${TAGS[*]}"
if [[ -n "$DEPS_ENV" ]]; then
  echo "Deps env:     $DEPS_ENV"
else
  echo "Deps env:     none"
fi
echo "SCons jobs:   $JOBS"
echo

for tag in "${TAGS[@]}"; do
  target=$INSTALL_ROOT/rat-$tag
  log=$INSTALL_ROOT/rat-$tag.build.log

  echo "==> RAT $tag"
  if [[ -e "$target" ]]; then
    if [[ "$SKIP_EXISTING" -eq 1 ]]; then
      echo "Skipping existing $target"
      echo
      continue
    fi
    die "$target already exists"
  fi

  if [[ "$DRY_RUN" -eq 1 ]]; then
    run git clone --branch "$tag" --single-branch "$REPO_URL" "$target"
    run bash -lc "cd '$target' && ./configure && source env.sh && scons -j '$JOBS'"
    echo
    continue
  fi

  run git clone --branch "$tag" --single-branch "$REPO_URL" "$target"

  (
    set -euo pipefail
    cd "$target"
    if [[ -n "$DEPS_ENV" ]]; then
      # shellcheck disable=SC1090
      set +u
      source "$DEPS_ENV"
      set -u
    fi
    ./configure
    # shellcheck disable=SC1091
    source env.sh
    scons -j "$JOBS"
  ) 2>&1 | tee "$log"

  echo "Built RAT $tag; log: $log"
  echo
done
