@echo off
SETLOCAL ENABLEEXTENSIONS
vsperfcmd /Start:Sample /Output:C:\Svn-ProfileData\suite.vsp
call run-tests -s -d basic#1-10
vsperfcmd /Shutdown