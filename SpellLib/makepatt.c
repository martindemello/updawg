#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
  char mapsto[256];
  char mapping[128];
  char line[128];
  char *s, *mapp;
  int nextmapch;
  int i;

  for (;;) {
    for (i = 0; i < 256; i++) mapsto[i] = 0;
    nextmapch = 'A'; mapp = mapping;
    s = gets(line);
    if (s == NULL) break;
    for (;;) {
      if (mapsto[*s] != 0) {
        *mapp++ = mapsto[*s];
      } else {
        *mapp++ = nextmapch;
        mapsto[*s] = nextmapch++;
      }
      s += 1;
      if (*s == '\0') break;
    }
    *mapp = '\0';
    fprintf(stdout, "%s=%s\n", mapping, line);
  }
}
