rmdir .\tmp /s /q || exit /b
mkdir tmp
call sv.cmd co https://svn.apache.org/repos/asf/subversion/trunk/notes/wc-ng tmp\wc-ng --config-dir tmp\cfg
echo store-plaintext-passwords = nos >> tmp\cfg\servers
echo [auth] >> tmp\cfg\config
echo password-stores = >> tmp\cfg\config
echo >> tmp\wc-ng\design
call sv.cmd ci tmp\wc-ng  --config-dir tmp\cfg -m ""