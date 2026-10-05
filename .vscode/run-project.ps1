param([switch]$Run)

$ErrorActionPreference = 'Stop'
$projectDirectory = Split-Path -Parent $PSScriptRoot
$taskFile = Join-Path $PSScriptRoot 'tasks.json'
$buildTask = (Get-Content -Raw -LiteralPath $taskFile | ConvertFrom-Json).tasks |
    Where-Object { $_.label -eq 'Build wuziqi (MinGW)' } |
    Select-Object -First 1

if (-not $buildTask) {
    throw 'Project build task not found.'
}

$compilerArguments = @($buildTask.args | ForEach-Object {
    $_.Replace('${workspaceFolder}', $projectDirectory)
})

Push-Location -LiteralPath $projectDirectory
try {
    Write-Host 'Building the complete wuziqi project...'
    & $buildTask.command @compilerArguments
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }

    Write-Host 'Build succeeded.'
    if ($Run) {
        & (Join-Path $projectDirectory 'wuziqi.exe')
        exit $LASTEXITCODE
    }
}
finally {
    Pop-Location
}
