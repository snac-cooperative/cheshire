/* Swig configuration file for CheshirePy */
%define DOCSTRING
"The CheshirePy module lets you access the Cheshire Information Retrieval
system and get results directly in Python."
%enddef

%module(docstring=DOCSTRING) CheshirePy
%{
#define SWIG_FILE_WITH_INIT
#define SWIG_PYTHON_SAFE_CSTRINGS
#include "CheshirePy.h"
%}

%feature("autodoc", "1");

%inline %{
extern SGML_DTD *main_dtd;
extern int statflag;
extern config_file_info *cf_info_base;
extern Tcl_HashTable *cf_file_names;
extern Tcl_HashTable cf_file_names_data;
extern FILE *LOGFILE;
extern FILE *cheshire_log_file;
extern DB_ENV *gb_dbenv;
extern void free_all_dtds();

%}

%newobject showconfig;
%newobject getconfigpath;
%newobject getconfigindexnames;
%newobject showdbs;
%newobject showdb;
%newobject getdbname;
%newobject showresultname;
%newobject showsyntax;
%newobject showformat;
%newobject getrecord;
/* %newobject Search; nope - can't throw it away */


%include "CheshirePy.h"

int init(char *config_path);
char *showconfig();
char *getconfigpath();
char *getconfigindexnames();

int setdb(char *dbname);
char *showdbs();
char *showdb();
char *getdbname();


int setresultname(char *name);
char *showresultname();

int setnumwanted(int numwanted);
int shownumwanted();

int setsyntax(char *syntax);
char *showsyntax();

int setresultformat(char *format);
char *showformat();

weighted_result *Search(char *search);
int getnumfound(weighted_result *final_set);
char *getrecord(weighted_result *final_set, int recnum);
float getrelevance(weighted_result *final_set, int recnum);
void closeresult(weighted_result *final_set);

void CheshireClose();

