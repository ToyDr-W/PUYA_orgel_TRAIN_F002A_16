@echo off
:: ==========================================
:: ★ ここに 使用したいCOMポート番号ボーレート を指定してください
set COM_PORT=COM12
set BAUD_RATE=230400
:: ==========================================

:: 通信設定
mode %COM_PORT%:%BAUD_RATE%,n,8,1

:: 1. 送信処理
<nul set /p ="ERASE" > \\.\%COM_PORT%

:: 2. 受信処理と表示（PowerShell側にもバッチの変数 %COM_PORT% を渡しています）
powershell -Command "$port = New-Object System.IO.Ports.SerialPort('%COM_PORT%', %BAUD_RATE%, 'None', 8, 'One'); $port.ReadTimeout = 30000; $port.Open(); try { $response = $port.ReadLine(); Write-Output $response } catch [TimeoutException] {  } finally { $port.Close() }"

pause
