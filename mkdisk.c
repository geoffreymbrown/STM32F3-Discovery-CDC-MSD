#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
  FILE *fd;
  unsigned char buf[512];
  int i;
  int start = 0;
  unsigned int  size = 0;
  
  if (argc < 2) {
    fprintf(stderr, "usage %s rawfile\n", argv[0]);
    exit(1);
  }

  if ((fd = fopen(argv[1], "r")) < 0) {
    fprintf(stderr, "can't open %s\n", argv[1]);
    exit(1);
  }

  printf("// Disk Image\n");
  printf("#include <stdint.h>\n");
  fseek(fd,0,2);
  unsigned int flen = ftell(fd);
  
  printf("const uint32_t DiskImageLen = %u;\n",flen);
  rewind(fd);
  printf("unsigned char DiskImage[%u] = {\n",flen);

  int len;
  while (0<(len = fread(buf,sizeof(buf),1,fd))) {
    size += len;
    int i;
    if (start)
      printf(",\n");
    start = 1;
    for (i = 0; i < sizeof(buf); i++) {
      printf("%s0x%02x",i?",":"",buf[i]);
    }
  }
  printf("};\n");
  fclose(fd);
  fprintf(stderr,"size = %d\n", size * 512);
}
