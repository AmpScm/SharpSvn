/* $Id$ */

This directory contains a minimalistic rerouting of the gettext() api to a c 
override, allowing us to override the error texts from the SharpSvn API. When
enabled this allows replacing the default exception texts with localizable 
texts stored in .Net resources.


Subversion requires a intl3_svn.lib when enabling an nls build, so we provide
one with the pointer definition ;-)