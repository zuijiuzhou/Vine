#!/usr/bin/env bash
# gfx_lavapipe_check.sh — Regression check of the vsg rendering backend on the
# lavapipe (Mesa software) Vulkan driver with the Khronos validation layer.
#
# This is the GPU-free end-to-end validation for vsg-backend changes. It runs:
#   1. vsg_color_probe in its raw-Phong and Builder-box modes — proves vsg
#      pipelines compile, record and present on a Vulkan device.
#   2. The Vine app (app_shell demo, 5 boxes) — proves the SceneBridge path
#      (default RenderStateMapper mapping) syncs and renders every frame with
#      no validation-layer errors.
#
# The RenderStateMapper unit mapping (incl. the non-default StateNode path) is
# pinned by tests/test_vsg/RenderStateMapperTest, which needs no device; the
# StateNode -> distinct-pipeline path was additionally validated once on
# lavapipe with a temporary demo hook (see .ai/memory/graphics.md). Pixel-level
# visuals (culling winding, blend result, reverse-Z depth look) still require a
# real GPU.
#
# Usage:
#   scripts/gfx_lavapipe_check.sh [BUILD_DIR]     BUILD_DIR defaults to <root>/build
#
# Env overrides:
#   VK_ICD_FILENAMES        Existing selection wins; otherwise lavapipe is
#                           auto-detected (lvp_icd.json).
#   VINE_CHECK_FRAMES       Frames per probe run   (default 20)
#   VINE_CHECK_SECONDS      Seconds to run Vine    (default 12)
#   VINE_SKIP_APP=1         Skip the Vine app run.
#   VINE_PROBE_MODE_EXTRA   Extra vsg_color_probe modes to run (space list).
#
# Exit code 0 when every stage is clean, 1 otherwise.

set -u

# ---- Locate root / build ----------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD="${1:-$ROOT/build}"

PROBE="$BUILD/bin/vsg_color_probe"
VINE_BIN="$BUILD/bin/Vine"

FRAMES="${VINE_CHECK_FRAMES:-20}"
SECONDS_V="${VINE_CHECK_SECONDS:-12}"

# ---- Select the lavapipe ICD -----------------------------------------------
ICD="${VK_ICD_FILENAMES:-}"
if [ -z "$ICD" ]; then
    for f in /usr/share/vulkan/icd.d/lvp_icd.json /etc/vulkan/icd.d/lvp_icd.json; do
        if [ -f "$f" ]; then
            ICD="$f"
            break
        fi
    done
fi
if [ -n "$ICD" ]; then
    export VK_ICD_FILENAMES="$ICD"
    echo "[info] Vulkan ICD: $ICD"
else
    echo "[warn] lavapipe ICD not found; relying on the default driver selection"
fi

# ---- Checks -----------------------------------------------------------------
FAILED=0
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

report() { # name  file  [ok_exit_codes...]
    local name="$1" file="$2"
    shift 2
    if grep -qiE "VUID-|UNASSIGNED-|\[Validation\].*error|validation layer.*error|exception|compile failed|failed to create window|vkCreateInstance.*fail|ERROR:.*Loader" "$file"; then
        echo "[FAIL] $name"
        grep -iE "VUID-|UNASSIGNED-|\[Validation\]|exception|compile failed|failed|ERROR" "$file" | head -20
        FAILED=1
    elif [ $# -gt 0 ]; then
        # Expected (non-zero) exit codes passed as ok; anything else is fatal.
        grep -q "done\|running\|sync" "$file" || true
        echo "[PASS] $name"
    else
        echo "[PASS] $name"
    fi
}

run_probe() { # mode  frames
    local mode="$1" frames="$2"
    local log="$TMP/probe_$mode.log"
    if [ "$mode" = "raw" ]; then
        (cd "$BUILD" && VINE_PROBE_FRAMES="$frames" ./bin/vsg_color_probe) >"$log" 2>&1
        rc=$?
    else
        (cd "$BUILD" && VINE_PROBE_MODE="$mode" VINE_PROBE_FRAMES="$frames" ./bin/vsg_color_probe) >"$log" 2>&1
        rc=$?
    fi
    echo "    (exit=$rc)"
    report "vsg_color_probe [$mode]" "$log" 0
    if [ "$rc" -ne 0 ]; then FAILED=1; fi
}

echo "== 1/4 vsg_color_probe raw-Phong =="
run_probe raw "$FRAMES"

echo "== 2/4 vsg_color_probe Builder box =="
run_probe box "$FRAMES"

# Runtime-compiled user program (glslang) -> hand-built ShaderSet -> pipeline.
echo "== 3/4 vsg_color_probe custom user shader =="
run_probe custom "$FRAMES"

for extra in ${VINE_PROBE_MODE_EXTRA:-}; do
    echo "== 3b/4 vsg_color_probe [$extra] =="
    run_probe "$extra" "$FRAMES"
done

echo "== 4/4 Vine app (default demo) =="
if [ "${VINE_SKIP_APP:-0}" = "1" ]; then
    echo "    (skipped, VINE_SKIP_APP=1)"
else
    if [ ! -x "$VINE_BIN" ]; then
        echo "[FAIL] Vine app not built at $VINE_BIN"
        FAILED=1
    else
        log="$TMP/vine.log"
        (cd "$BUILD" && timeout "$SECONDS_V" ./bin/Vine) >"$log" 2>&1
        rc=$?
        echo "    (exit=$rc; 124 = still running when the timeout fired, i.e. OK)"
        # Vine is a GUI app: it runs until killed. A timeout (124) is success;
        # any other non-zero exit indicates a startup crash.
        if [ "$rc" -ne 0 ] && [ "$rc" -ne 124 ]; then
            echo "[FAIL] Vine exited early with $rc"
            tail -30 "$log"
            FAILED=1
        else
            report "Vine app (default demo)" "$log"
        fi
    fi
fi

echo
if [ "$FAILED" -eq 0 ]; then
    echo "RESULT: PASS — lavapipe validation clean (no VUID/validation errors)."
    exit 0
else
    echo "RESULT: FAIL — see messages above (logs kept until script exit)."
    exit 1
fi
