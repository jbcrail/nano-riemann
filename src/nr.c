#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "nano-riemann.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "riemann.pb.h"

#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "5555"

struct nr_config {
  char host[256];
  char port[6]; // max length of base-10 16-bit integer + 1
};

// Adapted from http://unixpapa.com/incnote/string.html
size_t strncpy_safe(char *dst, const char *src, size_t size)
{
  size_t len, srclen;
  srclen = strlen(src);
  if (--size <= 0) return srclen;
  len = min(size, srclen);
  memmove(dst, src, len);
  dst[len] = '\0';
  return srclen;
}

void usage()
{
  printf(
"Usage: nr [options] <service> <state> [<tag> ...]\n"
"\n"
"  --help               show this help\n");

  exit(EXIT_SUCCESS);
}

void parse_environment(struct nr_config *config)
{
  char *authority = getenv("RIEMANN_AUTHORITY");
  if (authority) {
    strncpy_safe(config->host, authority, sizeof(config->host));
    char *delimiter = strchr(authority, ':');
    if (delimiter) {
      strncpy_safe(config->host, authority, min(delimiter-authority+1, sizeof(config->host)));
      strncpy_safe(config->port, delimiter+1, sizeof(config->port));
    } else {
      strncpy_safe(config->host, authority, sizeof(config->host));
    }
  }
}

void parse_args(int argc, char **argv, struct nr_config *config)
{
  size_t arglen;
  for (int i = 1; i < argc; i++) {
    //int lastarg = (i == argc-1);
    arglen = strlen(argv[i]);
    if (!strncasecmp(argv[i], "--help", arglen)) {
      usage();
    } else {
      fprintf(stderr, "Unknown option: '%s'\n", argv[i]);
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv)
{
  struct nr_config config;
  strcpy(config.host, DEFAULT_HOST);
  strcpy(config.port, DEFAULT_PORT);

  parse_environment(&config);
  parse_args(argc, argv, &config);

  return 0;
}
