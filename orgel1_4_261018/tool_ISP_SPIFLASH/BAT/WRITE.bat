@echo off
:: ==========================================
:: ★ ここに 使用したいCOMポート番号とボーレート、HEXファイル名を指定してください
set COM_PORT=COM12
set BAUD_RATE=230400
set HEX_FILE=W25Q.hex
:: ==========================================

:: 通信設定
mode %COM_PORT%:%BAUD_RATE%,n,8,1

:: 1. 送信処理
<nul set /p ="WRITE" > \\.\%COM_PORT%

:: 2. 受信処理と表示（PowerShell側にもバッチの変数 %COM_PORT% を渡しています）
powershell -Command "$port = New-Object System.IO.Ports.SerialPort('%COM_PORT%', %BAUD_RATE%, 'None', 8, 'One'); $port.ReadTimeout = 1000; $port.Open(); try { $response = $port.ReadLine(); Write-Output $response } catch [TimeoutException] {  } finally { $port.Close() }"

echo "WRITE START"

..\hexsend\Release\hexsend.exe %COM_PORT% %BAUD_RATE% %HEX_FILE%

echo "WRITE END"

pause
