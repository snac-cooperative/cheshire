Installing Cheshire
===========================

The following steps will install Cheshire onto a linux machine from this repo.  Currently only 64-bit versions of CentOS 6.5 and Ubuntu 14.04 are supported by these installation steps.

Install Dependencies
------------------------

1. Grab dependency packages from the package manager:

	```
	centos> sudo yum install tcl tk tk-devel tcl-devel flex postgresql gcc kernel-headers libxml2 libxml2-devel postgresql-devel
	```
or

	```
        ubuntu> sudo apt-get install tcl tk tk-dev tcl-dev flex postgresql gcc linux-headers-$(uname -r) libxml2 libxml2-dev libpq-dev libc6-dev libxml2-utils
        ```

2. Download LLgen from [http://cheshire.berkeley.edu/Downloads/LLgen_cheshire_v.tgz](http://cheshire.berkeley.edu/Downloads/LLgen_cheshire_v.tgz).  Unzip using `tar xzf LLgen_cheshire_v.tgz`.  Then edit the Makefile (if using CentOS 6.5), changing

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

Install Cheshire
--------------------

1. Clone this repository using `git clone user@repo`.
2. Copy `Makefile.centos65` or `Makefile.ubuntu1404` to `Makefile`
3. Run `make newbin` to build Cheshire.

Install CheshirePy
--------------------------------

1. Grab dependency packages from the package manager:

        ```
        centos> sudo yum install [to be determined]
        ```
or

        ```
        ubuntu> sudo apt-get install postgresql postgresql-contrib python-setuptools python-dev swig 
        ```

2. In the former repository, change directory to `CheshirePy` using `cd CheshirePy`.  Then run the make commands: `make clean && make`.


Use Cheshire for VIAF
----------------------

1. Download VIAF clusters XML file from [viaf](http://viaf.org/viaf/data/).
2. Unzip the file using `gunzip filename.xml.gz`
3. Build the associator file for cheshire:
	1. Make sure cheshire is in your path, using `PATH=$PATH:~/cheshire/bin`.
	2. Run `buildassoc -q -s /path/to/viaf/filename.xml VIAFCluster`.
		* The `-q` silences output
		* The `-s` skips other XML and non-XML code
		* `VIAFCluster` is the name of the XML element for which we are building an associator
	3. The associator will save a `filename.assoc` file for the XML, in the same directory as the XML file.

Note: For the Aug 28, 2014 VIAF data dump, there were 26,793,357 records.  However, the VIAF format changed from May 2014 to August 2014, therefore the viaf.dtd file must be updated to accommodate the newer format.  I was unsuccessful at getting it correctly updated, so I used the older records, found on shannon at `~rlarson/DATA/VIAF/data4`.

### Space Requirements

Cheshire indexing does not require much space, as it turns out.  The VIAF records use 72GB (for the Aug 2014 set) or 86GB (for the May 2014 set).  The associator file is approximately 200MB, with the indexes and BerkeleyDB database environment totalling under 1MB.  The logfile generated from `index_cheshire` was 36GB in size, and should be removed or relocated.


