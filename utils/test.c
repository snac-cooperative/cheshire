#include <ctype.h>
int main (int argc, char **argv)
{
  char a;
  int i;
  i = 32;
  a = ' ';
  
      printf("testing blanks???\n");
      printf ("test i space = %d\n", isspace(i));
      printf ("test a space = %d\n", isspace(a));
      printf ("test lit space = %d\n", isspace(' '));
      printf ("test lit tab = %d\n", isspace('\t'));
      printf ("test lit nl = %d\n", isspace('\n'));
      printf ("test lit a = %d\n", isspace('a'));
      printf ("test lit > = %d\n", isspace('>'));

}
