#include <iostream>
#include <map>

#include <cstdio>

using namespace std;

const int BUF_LEN=32;

void usage(const char *fn)
{
  printf("%s : filename\n", fn);
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    usage(argv[0]);
    return 0;
  }

  FILE *fs;

  fs=fopen(argv[1], "rb");
  if (!fs)
  {
    printf("open %s fail\n", argv[1]);
    return -1;
  }
  printf("open %s success\n", argv[1]);

  int offset=0x4e30;//+8;
  //int offset=0;//+8;

  fseek(fs, offset, SEEK_SET);
  char buf[BUF_LEN];
  size_t read_count=10;

  read_count=fread(buf, 1, BUF_LEN, fs);

  printf("read_count : %d\n", read_count);
  for (int i=0; i < read_count; ++i)
  {
    if (i%16==0) printf("\n");
    printf("%.2x ", (char *)buf[i]);
  }
  





  return 0;
}
