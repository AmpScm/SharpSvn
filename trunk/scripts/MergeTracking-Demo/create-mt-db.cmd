@echo off
setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
set SVN=svn-win32
set SVNADMIN=svnadmin-win32

if EXIST \tmp RMDIR \tmp /s /q
if EXIST \tmp (
	echo Can't delete \tmp
	goto :EOF
)
mkdir \tmp

REM ======================= Revision 1 ==============================
%SVNADMIN% create /tmp/repos
%SVN% co file:///tmp/repos \tmp\wc
%SVN% mkdir \tmp\wc
%SVN% mkdir \tmp\wc\branches
%SVN% mkdir \tmp\wc\tags
%SVN% mkdir \tmp\wc\trunk
%SVN% mkdir \tmp\wc\trunk\about
%SVN% mkdir \tmp\wc\trunk\jobs
%SVN% mkdir \tmp\wc\trunk\news
%SVN% mkdir \tmp\wc\trunk\products
%SVN% mkdir \tmp\wc\trunk\support
pushd files\1
for %%I in (*.html) DO (
   set f=%%I
   set to=\tmp\wc\trunk\!f:-=\!
   copy !f! !to! || echo Failed to copy !f! to !to!
   %SVN% add !to!
)
popd
for /r \tmp\wc %%i in (*.html) do %SVN% ps svn:eol-style native %%i
%SVN% ci --username user \tmp\wc -m "Create initial product structure"


REM ======================= Revision 2 ==============================
pushd files\2
%SVN% co file:///tmp/repos/trunk \tmp\trunk
for %%I in (*.html) DO (
   set f=%%I
   set to=\tmp\trunk\!f:-=\!
   copy !f! !to! || echo Failed to copy !f! to !to!
)
popd
%SVN% ci --username user \tmp\trunk -m "Flesh out page content before launch"   


REM ======================= Revision 3 ==============================
%SVN% --username copier copy file:///tmp/repos/trunk file:///tmp/repos/branches/a -m "Create branch a"

REM ======================= Revision 4 ==============================
%SVN% co file:///tmp/repos/branches/a \tmp\branch-a
pushd files\4
for %%I in (*.html) DO (
   set f=%%I
   set to=\tmp\branch-a\!f:-=\!
   copy !f! !to! || echo Failed to copy !f! to !to!
   %SVN% add !to!
   %SVN% ps svn:eol-style native !to!
)
popd
%SVN% ci --username auser \tmp\branch-a -m "Create page for medium product"

REM ======================= Revision 5 ==============================
%SVN% --username copier copy file:///tmp/repos/trunk file:///tmp/repos/branches/c -m "Create branch c"

REM ======================= Revision 6 ==============================
pushd \tmp\trunk
%SVN% up .
%SVN% merge file:///tmp/repos/branches/a
%SVN% ci --username merger -m "Merge branch a.  Added medium product"
popd

REM ======================= Revision 7 ==============================
%SVN% up /tmp/branch-a
%SVN% mkdir /tmp/branch-a/blocked
pushd files\6
for %%I in (*.html) DO (
   set f=%%I
   set to=\tmp\branch-a\!f:-=\!
   copy !f! !to! || echo Failed to copy !f! to !to!
   %SVN% add !to!
   %SVN% ps svn:eol-style native !to!
)
popd
%SVN% ci --username auser /tmp/branch-a -m "Create blocked folder.  This should only ever exist on branch a"

REM ======================= Revision 8 ==============================
pushd \tmp\trunk
%SVN% up .
%SVN% merge --record-only -r 6:7 file:///tmp/repos/branches/a
%SVN% ci --username blocker -m "Block r7 from branch a"
popd

REM ======================= Revision 9 ==============================
pushd files\9
for %%I in (*.html) DO (
   set f=%%I
   set to=\tmp\trunk\!f:-=\!
   copy !f! !to! || echo Failed to copy !f! to !to!
   %SVN% add !to!
   %SVN% ps svn:eol-style native !to!
)
popd
%SVN% ci --username user /tmp/trunk -m "Add medium product to products list"

REM ======================= Revision 10 =============================
%SVN% --username copier copy file:///tmp/repos/trunk file:///tmp/repos/branches/b -m "Create branch b"


REM ======================= Revision 11 =============================
%SVN% up /tmp/branch-a
pushd files\11
for %%I in (*.html) DO (
   set f=%%I
   set to=\tmp\branch-a\!f:-=\!
   copy !f! !to! || echo Failed to copy !f! to !to!
   %SVN% add !to!
   %SVN% ps svn:eol-style native !to!
)
popd
%SVN% ci --username auser /tmp/branch-a -m "Add product roadmap"

REM ======================= Revision 12 =============================
%SVN% co file:///tmp/repos/branches/b \tmp\branch-b
pushd \tmp\branch-b
%SVN% merge file:///tmp/repos/branches/a
%SVN% ci --username merger -m "Merge branch a - product roadmap"
popd

REM ======================= Revision 13 =============================
%SVN% up /tmp/branch-b
pushd files\13
for %%I in (*.html) DO (
   set f=%%I
   set to=\tmp\branch-b\!f:-=\!
   copy !f! !to! || echo Failed to copy !f! to !to!
   %SVN% add !to!
   %SVN% ps svn:eol-style native !to!
)
popd
%SVN% ci --username buser /tmp/branch-b -m "Update info about our company"

REM ======================= Revision 14 =============================
%SVN% up /tmp/trunk
pushd \tmp\trunk
%SVN% merge file:///tmp/repos/branches/b
popd
%SVN% ci --username merger /tmp/trunk -m "Merge branch b - product roadmap and update about page"

REM ======================= Revision 15 =============================
%SVN% co file:///tmp/repos/branches/c \tmp\branch-c
pushd \tmp\branch-c
%SVN% merge file:///tmp/repos/trunk
%SVN% ci --username merger -m "Synch branch back up with trunk"
popd

REM ======================= Revision 16 =============================
%SVN% up /tmp/branch-c
pushd files\16
for %%I in (*.html) DO (
   set f=%%I
   set to=\tmp\branch-c\!f:-=\!
   copy !f! !to! || echo Failed to copy !f! to !to!
   %SVN% add !to!
   %SVN% ps svn:eol-style native !to!
)
popd
%SVN% ci --username cuser /tmp/branch-c -m "Added new job to jobs page"

REM ======================= Revision 17 =============================
%SVN% up /tmp/trunk
pushd \tmp\trunk
%SVN% merge file:///tmp/repos/branches/c
popd
%SVN% ci --username merger /tmp/trunk -m "Merged branch c back to trunk"


%SVNADMIN% dump \tmp\repos > own.repos.dump