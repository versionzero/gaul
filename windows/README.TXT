MS Windows Readme
=================

Introduction
------------

The precompiled static libraries, gaul-devel.a and gaul-util.a, and
the example program, struggle.exe, were created using Dev-C++
version 4.9.8.0.  This is available from
http://www.bloodshed.org/dev/devcpp.html

Compilation
-----------

The abbreviated instructions for compiling your own gaul executables
with Dev-C++ are:

PART I: gaul-util library.

1) Create a "gaul-util" project to compile a static library:
 a) select the menu "File->New->Project",
 b) click the "Static Library" icon,
 c) select the "C Project" radiobutton,
 d) type "gaul-util" in the "Name:" text box,
 c) click the "Okay" button.
2) Add all C source files from the GAUL subdirectory "util".
3) Create a project folder, "gaul", and add all of the C header
files from the subdirctory "util/gaul" into this.
4) Select menu item "Project->Project_Options", then the tab
"Parameters".
5) In the "Compiler:" box add "-I###", replacing ### by the location
of the "util" directory.
6) Click "Okay" button.
7) Select menu option "Execute->Compile" or click the compile button
(four coloured squares).

PART II: gaul-devel library.

1) Create a "gaul-devel" project to compile a static library:
 a) select the menu "File->New->Project",
 b) click the "Static Library" icon,
 c) select the "C Project" radiobutton,
 d) type "gaul-devel" in the "Name:" text box,
 c) click the "Okay" button.
2) Add all C source files from the GAUL subdirectory "src".
3) Create a project folder, "gaul", and add all of the C header
files from the subdirctory "src/gaul" into this.
4) Select menu item "Project->Project_Options", then the tab
"Parameters".
5) In the "Compiler:" box add "-I###" -I***, replacing ### by the
location of the "util" directory and *** by the location of the
"src" directory.
6) Click "Add Library or Object" button.
7) Select the library compiled in part I, above, "gaul-util.a".
8) Click "Okay".
9) Select menu option "Execute->Compile" or click the compile button
(four coloured squares).

PART III: example programs.

Note that each example program requires a seperate project, sorry,
but there is nothing I can do about this.  Complain on the Dev-C++
if you like.  These instructions are for the "struggle" example,
but all examples are compiled in a similar way.

1) Create a "struggle" project to compile a console application:
 a) select the menu "File->New->Project",
 b) click the "Console Application" icon,
 c) select the "C Project" radiobutton,
 d) type "struggle" in the "Name:" text box,
 c) click the "Okay" button.
2) Add the relevent C source filesfrom the GAUL subdirectory
"examples".
3) Select menu item "Project->Project_Options", then the tab
"Parameters".
4) In the "Compiler:" box add "-I###" -I***, replacing ### by the
location of the "util" directory and *** by the location of the
"src" directory.
5) Click "Add Library or Object" button.
7) Select the library compiled in part I, above, "gaul-util.a".
8) Select the library compiled in part II, above, "gaul-devel.a".
8) Click "Okay".
9) Select menu option "Execute->Compile" or click the compile button
(four coloured squares).

Creating and compiling your own programs that use GAUL should
require a similar process to that outlined in PART III.

If you should feel the urge to create a MSVC project for GAUL, and
are successful in the endeavour, then I would be very interested
in receiving a package for distribution at the GAUL website.

Stewart Adcock, 26th August 2003.
