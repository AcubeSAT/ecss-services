#!/usr/bin/env bash

#
# Contract checks for the firmware-obc CI job.
# Does not cross-compile; asserts the job builds the MR package, not Artifactory 1.2.
#
# Usage:
#   ci/test-firmware-obc-contract.sh
#

set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
YAML="${ROOT}/.gitlab-ci.yml"
SCRIPT="${ROOT}/ci/firmware-obc.sh"
README="${ROOT}/README.md"
fail=0

check() {
  local name="$1"
  local rc=0
  shift
  "$@" || rc=$?
  if [ "$rc" -eq 0 ]; then
    echo "PASS  ${name}"
  else
    echo "FAIL  ${name}" >&2
    fail=1
  fi
}

absent() {
  if [ -n "${2:-}" ]; then
    ! grep -q "$1" "$2"
  else
    ! grep -q "$1"
  fi
}

install_without_update() {
  ! grep -E '^[[:space:]]*conan install .*[[:space:]]-u([[:space:]]|$)' "$SCRIPT"
}

job_block() {
  awk '
    $0 ~ /^firmware-obc:/ {in_job=1; print; next}
    in_job && $0 ~ /^[^[:space:]#]/ {exit}
    in_job {print}
  ' "$YAML"
}

upload_job_block() {
  awk '
    $0 ~ /^conan:/ {in_job=1; print; next}
    in_job && $0 ~ /^[^[:space:]#]/ {exit}
    in_job {print}
  ' "$YAML"
}

JOB="$(job_block)"
UPLOAD="$(upload_job_block)"

check "firmware-obc uses spacedot/build-arm" \
  grep -q 'image: spacedot/build-arm' <<<"$JOB"
check "firmware-obc is in the build stage" \
  grep -q 'stage: build' <<<"$JOB"
check "firmware-obc runs ci/firmware-obc.sh" \
  grep -q 'ci/firmware-obc.sh' <<<"$JOB"
check "firmware-obc does not conan upload" \
  absent 'conan upload' <<<"$JOB"
check "default-branch conan job still uploads 1.2" \
  grep -q 'conan upload ecss-services/1.2' <<<"$UPLOAD"
check "script conan-creates this checkout" \
  grep -q 'conan create "$ECSS_DIR"' "$SCRIPT"
check "script does not conan upload" \
  absent 'conan upload' "$SCRIPT"
check "firmware conan install does not pass -u" \
  install_without_update
check "script sets OBC platform_definitions_path" \
  grep -q 'platform_definitions_path' "$SCRIPT"
check "script clones embedded-software" \
  grep -q 'embedded-software.git' "$SCRIPT"
check "script does not checkout XPS at CI_COMMIT_REF_NAME" \
  absent 'CI_COMMIT_REF_NAME' "$SCRIPT"
check "README says the job uses this checkout, not Artifactory 1.2" \
  grep -q 'Artifactory `ecss-services/1.2`' "$README"
check "CI comments mention Artifactory 1.2 and unrelated firmware failures" \
  grep -q 'not Artifactory ecss-services/1.2' "$YAML"

if [ "$fail" -ne 0 ]; then
  echo "firmware-obc contract checks failed" >&2
  exit 1
fi

echo "firmware-obc contract checks passed"
