# 由 lyrics.txt 生成 src\warble_table.h
$ErrorActionPreference = 'Stop'

if (-not (Test-Path 'src')) { New-Item -ItemType Directory 'src' | Out-Null }

$out = New-Object System.Collections.Generic.List[string]
$out.Add('/* 由 lyrics.txt 生成，勿手改 */')
$out.Add('static const char *const WARBLE_LINES[] = {')

$n = 0
foreach ($line in Get-Content -LiteralPath 'lyrics.txt' -Encoding UTF8) {
    $n++
    if ($line -match '^\s*$' -or $line -match '^\s*#') { continue }
    if ($line -match '["\\]') { throw "gen_table: 第 $n 行含引号或反斜杠，无法嵌入" }
    $out.Add('    "' + $line + '",')
}
$out.Add('};')

$utf8 = New-Object System.Text.UTF8Encoding($false)   # 不写 BOM
[System.IO.File]::WriteAllLines((Join-Path $PWD 'src\warble_table.h'), $out, $utf8)
