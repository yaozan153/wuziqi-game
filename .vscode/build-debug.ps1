$ErrorActionPreference = 'Stop'
$projectDirectory = Split-Path -Parent $PSScriptRoot
$debugDirectory = Join-Path $env:TEMP 'wuziqi-vscode-debug'
New-Item -ItemType Directory -Path $debugDirectory -Force | Out-Null
foreach ($name in @('main.cpp', 'board.cpp', 'board.h')) {
    Copy-Item -LiteralPath (Join-Path $projectDirectory $name) -Destination $debugDirectory -Force
}
# 调试程序也需要棋子素材，复制到调试程序所在目录。
Copy-Item -LiteralPath (Join-Path $projectDirectory 'assets') -Destination $debugDirectory -Recurse -Force
$buildTask = (Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'tasks.json') | ConvertFrom-Json).tasks |
    Where-Object { $_.label -eq 'Build wuziqi (MinGW)' } | Select-Object -First 1
$compilerArguments = @($buildTask.args | ForEach-Object {
    $_.Replace('${workspaceFolder}/', '')
})
Push-Location -LiteralPath $debugDirectory
try {
    & $buildTask.command @compilerArguments
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
finally { Pop-Location }
