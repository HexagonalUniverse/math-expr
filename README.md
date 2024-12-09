C Project Template
==================

This is a template for a generic library-like C project with a target executable,
based on separate compilation steps.
Uses necessarily Makefile.

<strong>Notes</strong>:
* For instance, the template is intended to personal use.
* Focus primarily on Windows, besides some pretty straightforward adaptions 
making it as well as usable at linux.
* The test support is sketchy and highly experimental.


Default Structure
-----------------

The library's source code is at \<src/\>.
By default, the headers files are placed alongside the implementation source code at the directory.
The \<include/\> directory will holds header files intended for this purpose.

All library's source files are compiled into object separately.
The object's target is their respective source file, as well as any header files.
The library's object (static .lib or dynamic .dll) target is those objects.

The driver source code are located at \<drv/\>.
The respective executables depends upon the source code, as well as the library itself.

\<data/\> is destinated for the application's state and permanent data.
\<temp/\> differs for not being relevant for the project's state.


### Commands

Build with 
> make build

Cleaning holds the meaning of eliminating byproduct binaries from the building.

Clean the project with
> make clean

Eliminate all binaries and then build with
> make rebuild

Fully eliminate all binaries and temporary files with
> make full-clean

Run the default executable with
> make run

Execute the tests with 
> make test


Tests
-----

\<tests\> functions as a support testing library.
As such, there one can define tests and run them as aforementioned.
(Go to \<tests/\> subdir README.md for more details.)


Post-building Processing
------------------------

If one whishes to execute an action after building the project, it can be done setting up the makefile.
For example, if there's a base data overwhich some processing needs to be perfomed for the main application, there's where it enters.


