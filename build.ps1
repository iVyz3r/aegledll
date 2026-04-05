[Console]::OutputEncoding = [System.Text.Encoding]::UTF8


$ProjectName = "AegleInternal"
$BuildDir    = "obj"
$BinDir      = "build"
$Output      = Join-Path $BinDir "internal_hook.dll"

$Sources =  "dllmain.cpp", 
            "ImGui/imgui.cpp", 
            "ImGui/imgui_draw.cpp", 
            "ImGui/imgui_widgets.cpp", 
            "ImGui/imgui_tables.cpp", `
            "ImGui/backend/imgui_impl_dx11.cpp", 
            "ImGui/backend/imgui_impl_win32.cpp", 
            "Modules/Combat/Reach/Reach.cpp",
            "Modules/Combat/Hitbox/Hitbox.cpp",
            "Modules/Movement/AutoSprint/AutoSprint.cpp",
            "Modules/Movement/Timer/Timer.cpp",
            "Modules/Visuals/FullBright/FullBright.cpp",
            "Modules/Visuals/RenderInfo/RenderInfo.cpp",
            "Modules/Visuals/Watermark/Watermark.cpp",
            "Modules/Visuals/MotionBlur/MotionBlur.cpp",
            "Modules/Visuals/Keystrokes/Keystrokes.cpp",
            "Modules/Visuals/CPSCounter/CPSCounter.cpp",
            "Modules/Misc/UnlockFPS/UnlockFPS.cpp",
            "GUI/GUI.cpp",
            "Hook/Hook.cpp",
            "Input/Input.cpp",
            "Animations/Animations.cpp",
            "minhook/buffer.c", 
            "minhook/hook.c", 
            "minhook/trampoline.c", 
            "minhook/hde64.c"

$Flags = "-O2", "-s", "-fpermissive", "-m64", "-march=x86-64", "-static", "-static-libgcc", "-static-libstdc++", "-I."
$Libs  = "-ld3d11", "-ldxgi", "-ld3dcompiler", "-ldwmapi", "-limm32", "-luser32", "-lgdi32", "-lpsapi"

function Write-Log([string]$Msg, [string]$Type = "Info") {
    $Colors = @{ "Info" = "Cyan"; "Success" = "Green"; "Warn" = "Yellow"; "Error" = "Red"; "Link" = "Magenta" }
    $Icons  = @{ "Info" = "[i]"; "Success" = "[+]"; "Warn" = "[!]"; "Error" = "[X]"; "Link" = "[>]" }
    
    $Color = if ($Colors.ContainsKey($Type)) { $Colors[$Type] } else { "White" }
    $Icon  = if ($Icons.ContainsKey($Type)) { $Icons[$Type] } else { "[-]" }

    Write-Host ("{0} {1}" -f $Icon, $Msg) -ForegroundColor $Color
}

Clear-Host
Write-Log "Starting Build of $ProjectName (x64)..." "Info"

if (!(Test-Path $BuildDir)) { New-Item -ItemType Directory -Path $BuildDir | Out-Null }
if (!(Test-Path $BinDir))   { New-Item -ItemType Directory -Path $BinDir | Out-Null }

$ObjectFiles = @()
$Total = $Sources.Count
$Count = 0

foreach ($File in $Sources) {
    $Count++
    $CleanPath = $File -replace "^\.\\", ""
    $ObjFile = Join-Path $BuildDir ($CleanPath -replace '\.cpp$|\.c$', '.o')
    
    $ParentDir = Split-Path $ObjFile
    if (!(Test-Path $ParentDir)) { New-Item -ItemType Directory -Path $ParentDir | Out-Null }
    
    $ObjectFiles += $ObjFile

    if (!(Test-Path $ObjFile) -or (Get-Item $File).LastWriteTime -gt (Get-Item $ObjFile).LastWriteTime) {
        $Status = "[{0}/{1}]" -f $Count, $Total
        Write-Log "$Status Compiling: $File" "Warn"
        
        & g++ -c $File -o $ObjFile $Flags 2>&1 | Tee-Object -Variable Err | Out-Null
        if ($LASTEXITCODE -ne 0) {
            Write-Log "Fatal error in $File." "Error"
            Write-Host ($Err | Out-String) -ForegroundColor Gray
            exit 1
        }
    }
}

Write-Log "Linking binary in directory: $BinDir" "Link"
& g++ -shared -o $Output $ObjectFiles $Flags $Libs

if ($LASTEXITCODE -eq 0) {
    $Size = [Math]::Round((Get-Item $Output).Length / 1KB, 2)
    Write-Log "Process completed successfully." "Success"
    Write-Host "----------------------------------" -ForegroundColor Gray
    Write-Host "Generated DLL: $Output"
    Write-Host "File Size:       $Size KB"
    Write-Host "----------------------------------" -ForegroundColor Gray
    Write-Host "Exit Code: $LASTEXITCODE" -ForegroundColor Gray
} else {
    Write-Log "Error occurred while generating the DLL." "Error"
}