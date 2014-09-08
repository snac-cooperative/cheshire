#include "db.h"
/*
 * Upgrades old Berkeley DB's to version 3.0
 */
int main(int argc, char **argv) {
  int i;
  DB *dbp;
  int ret;
  if (argc == 1) {
    fprintf(stderr, "usage: db_upgrade file1 [file2 ..]\n");
    exit(1);
  }
  for (i=1; i<argc; i++) {
    
    if ((ret = db_create(&dbp, NULL, 0)) != 0) {
      fprintf(stderr, "db_create: %s\n", db_strerror(ret));
      exit (1);
    }
    
    if ((ret =
	 dbp->upgrade(dbp, argv[i], 0)) != 0) {
      dbp->err(dbp, ret, "Cannot upgrade %s", argv[i]);
    }
  }
  return 0;
}
