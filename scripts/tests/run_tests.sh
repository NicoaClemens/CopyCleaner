#!/bin/bash
# Minimal test runner for CopyCleaner

exe="../../cpp/build/copycleaner"

if [ ! -f "$exe" ]; then
    echo "Error: copycleaner not found"
    exit 1
fi

echo "Running comprehensive test..."
$exe comprehensive.ccl

if [ $? -eq 0 ]; then
    echo "[PASS] All tests passed"
    exit 0
else
    echo "[FAIL] Tests failed"
    exit 1
fi