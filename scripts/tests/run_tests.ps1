#!/usr/bin/env pwsh
# Minimal test runner for CopyCleaner

$exe = "..\..\cpp\build\copycleaner.exe"

if (-not (Test-Path $exe)) {
    Write-Host "Error: copycleaner.exe not found"
    exit 1
}

Write-Host "Running comprehensive test..."
& $exe comprehensive.ccl

if ($LASTEXITCODE -eq 0) {
    Write-Host "[PASS] All tests passed"
    exit 0
} else {
    Write-Host "[FAIL] Tests failed"
    exit 1
}
