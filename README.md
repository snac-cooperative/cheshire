Installing Cheshire
===========================

The following steps will install Cheshire onto a linux machine from this repo.  Currently only CentOS 6.5 is supported by these installation steps.

Install Dependencies
------------------------

1. Grab dependency packages from the package manager:
	```
	yum install tcl tk tk-devel tcl-devel flex postgresql gcc kernel-headers libxml2 libxml2-devel postgresql-devel
	```
2. Download LLgen from [http://cheshire.berkeley.edu/Downloads/LLgen_cheshire_v.tgz](http://cheshire.berkeley.edu/Downloads/LLgen_cheshire_v.tgz).  Unzip using `tar xzf LLgen_cheshire_v.tgz`.  Then edit the Makefile, changing
	```
	MANDIR=/usr/local/man
	```
to
	```
	MANDIR=/usr/local/share/man
	```
then compile using `make firstinstall`.
3. Download the latest version of flex from [Sourceforge](http://sourceforge.net/projects/flex/).  Untar the file, then modify `flexdef.h` replacing
	```
	#define MAXIMUM_NMS 31999
	```
with
	```
	#define MAXIMUM_NMS 319999
	```
then compile using the standard
	```
	./configure
	make
	sudo make install
	```
Note, this requires sudo privileges for the current user.
4. Clone this repository using `git clone user@repo`.
5. Run `make newbin` to build Cheshire. 
