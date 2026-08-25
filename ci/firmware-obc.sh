#!/usr/bin/env bash

#
# Compile OBC firmware against THIS checkout of ecss-services.
#
# Host check-build and Catch2 do not prove the library still links into firmware.
# Firmware consumes ecss-services/1.2 as a Conan static library, with OBC platform
# headers, the ARM toolchain, and OBC's own logError.
#
# This script conan-creates the local tree with the ARM profile and OBC
# platform_definitions_path, then builds OBC against that local cache package.
# It must not conan install -u (that pulls Artifactory 1.2) and must not upload.
#
# Unrelated firmware/XPS/ATSAM failures can occur. That is acceptable.
#
# Usage:
#   PROJECT=obc-dev ci/firmware-obc.sh
#   docker run --rm -v "$PWD:/src" -w /src \
#     -e CONAN_USER -e CONAN_PASSWORD -e PROJECT=obc-dev \
#     spacedot/build-arm ci/firmware-obc.sh
#

set -eu

PROJECT="${PROJECT:-obc-dev}"
ECSS_DIR="${ECSS_DIR:-${CI_PROJECT_DIR:-$(cd "$(dirname "$0")/.." && pwd)}}"
WORKDIR="${WORKDIR:-${ECSS_DIR}/firmware-obc-ci}"
FIRMWARE_DIR="${FIRMWARE_DIR:-${WORKDIR}/embedded-software}"
CONAN_REMOTE_URL="https://artifactory.spacedot.gr/artifactory/api/conan/conan"

case "$PROJECT" in
  obc-dev | obc-eqm | obc-eqm-new) ;;
  *)
    echo "PROJECT must be obc-dev, obc-eqm, or obc-eqm-new (got: ${PROJECT})" >&2
    exit 1
    ;;
esac

if [ -z "${CONAN_USER:-}" ] || [ -z "${CONAN_PASSWORD:-}" ]; then
  echo "CONAN_USER and CONAN_PASSWORD must be set" >&2
  exit 1
fi

if [ -d /root/.conan2 ] && [ -z "${CONAN_HOME:-}" ]; then
  CONAN_HOME=/root/.conan2
else
  CONAN_HOME="${CONAN_HOME:-${WORKDIR}/conan-home}"
  export CONAN_HOME
fi
CONAN_PROFILES="${CONAN_HOME}/profiles"

echo "Creating ecss-services from ${ECSS_DIR} and linking OBC (${PROJECT}) against that local package, not Artifactory 1.2"

mkdir -p "$WORKDIR"

if [ ! -d "${FIRMWARE_DIR}/.git" ]; then
  git clone https://gitlab.com/acubesat/obc/embedded-software.git "$FIRMWARE_DIR"
else
  echo "Using existing firmware clone at ${FIRMWARE_DIR}"
fi

mkdir -p "${FIRMWARE_DIR}/common/lib"
if [ ! -d "${FIRMWARE_DIR}/common/lib/cross-platform-software/.git" ]; then
  git clone https://gitlab.com/acubesat/obc/cross-platform-software.git \
    "${FIRMWARE_DIR}/common/lib/cross-platform-software"
fi
if [ ! -d "${FIRMWARE_DIR}/common/lib/atsam-component-drivers/.git" ]; then
  git clone https://gitlab.com/acubesat/obc/atsam-component-drivers.git \
    "${FIRMWARE_DIR}/common/lib/atsam-component-drivers"
fi
git -C "${FIRMWARE_DIR}/common/lib/cross-platform-software" submodule update --init --recursive

PLATFORM_PATH="$(cd "${FIRMWARE_DIR}/common/inc/Platform" && pwd)"

mkdir -p "${FIRMWARE_DIR}/conan-build" "$CONAN_PROFILES"
conan profile detect --force
cp "${FIRMWARE_DIR}/conan-arm-profile" "$CONAN_PROFILES/"
conan remote add conan "$CONAN_REMOTE_URL" --force
conan remote login -p "$CONAN_PASSWORD" conan "$CONAN_USER"

# Drop cached recipes/binaries so firmware cannot silently resolve Artifactory 1.2.
conan remove "ecss-services/*" -c || true
conan create "$ECSS_DIR" --build=missing -pr conan-arm-profile \
  -o "ecss-services/*:platform_definitions_path=${PLATFORM_PATH}"

# No -u: an update would replace the just-created revision with published 1.2.
# Do not rebuild ecss-services: keep the binary produced by conan create above.
cd "$FIRMWARE_DIR"
conan install . --output-folder conan-build --build="*" --build="!ecss-services" \
  -pr conan-arm-profile \
  -o "project=${PROJECT}" \
  -o "ecss-services/*:platform_definitions_path=${PLATFORM_PATH}"

cmake . -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=conan-build/build/Debug/generators/conan_toolchain.cmake
make -j"$(nproc)"
