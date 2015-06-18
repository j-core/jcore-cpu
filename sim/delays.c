#include "delays.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sim/utlist.h"

static int split_line(char *line, char **parts) {
  int num = 0;
  char *l = line;
  while (*l) {
    if (isspace(*l)) {
      l++;
    } else if (num == 6) {
      return -1;
    } else {
      parts[num++] = l;
      while (*l && !isspace(*l))
        l++;
      if (*l)
        *(l++) = '\0';
      else
        break;
    }
  }
  return num;
}

static int parse_delay(char *s, uint32_t *delay) {
  long int v;
  v = strtol(s, &s, 10);
  if (*s || v <= 0)
    return -1;
  *delay = v;
  return 0;
}

static int parse_cfg_line(struct delay_set *delays, char *line) {
  int num = 0;
  char *parts[6];
  char *l = line;
  char *s;
  long long int v;
  /* chop line at comment markers */
  while (*l) {
    if (*l == ';' || *l == '#') {
      *l = '\0';
      break;
    }
    l++;
  }
  num = split_line(line, parts);
  if (num == 0)
    return 0;
  if (num < 3 || num > 6) {
    fprintf(stderr, "Invalid number of parts on line\n");
    return -1;
  }
  struct delay *d = malloc(sizeof(struct delay));
  if (!d) {
    return -1;
  }
  memset(d, 0, sizeof(*d));

  if (num % 2) {
    /* first part must be a range */
    d->type = DELAY_RANGE;
    v = strtoll(parts[0], &s, 16);
    if (*s != '-' || v < 0 || v >= 0xFFFFFFFF)
      goto out_free;
    d->info.range.start = v;
    v = strtoll(s+1, &s, 16);
    if (*s != 0 || v < 0 || v >= 0xFFFFFFFF)
      goto out_free;
    d->info.range.end = v;
  } else {
    d->type = DELAY_MASK;
    v = strtoll(parts[0], &s, 16);
    if (*s != 0 || v < 0 || v >= 0xFFFFFFFF)
      goto out_free;
    d->info.match.mask = v;
    v = strtoll(parts[1], &s, 16);
    if (*s != 0 || v < 0 || v >= 0xFFFFFFFF)
      goto out_free;
    d->info.match.match = v & d->info.match.mask;
  }

  if (parse_delay(parts[num - 2], &d->wr_delay) ||
      parse_delay(parts[num - 1], &d->wr_drop_delay))
    goto out_free;
  if (num >= 5) {
    if (parse_delay(parts[num - 4], &d->rd_delay) ||
        parse_delay(parts[num - 3], &d->rd_drop_delay))
      goto out_free;
  } else {
    d->rd_delay = d->wr_delay;
    d->rd_drop_delay = d->wr_drop_delay;
  }
  /*switch (d->type) {
  case DELAY_MASK:
    printf("mask %u %u", d->match.mask, d->match.match);
    break;
  case DELAY_RANGE:
    printf("range %u %u", d->range.start, d->range.end);
    break;
  }
  printf(" d %u %u %u %u\n", d->rd_delay, d->rd_drop_delay,
    d->wr_delay, d->wr_drop_delay);*/
  LL_APPEND(delays->delays, d);
  return 0;
 out_free:
  free(d);
  return -1;
}

int delays_init_cfg(struct delay_set *delays, const char *filename) {
  int err = 0;
  char line[1024];
  int line_num = 1;
  memset(delays, 0, sizeof(*delays));
  if (!filename)
    return 0;
  FILE *f = fopen(filename, "r");
  if (!f) {
    perror("fopen");
    fprintf(stderr, "Failed to open %s\n", filename);
    return -1;
  }
  while (fgets(line, sizeof(line), f)) {
    //printf("parse line: %s", line);
    if ((err = parse_cfg_line(delays, line))) {
      fprintf(stderr, "Failed to parse line %d of %s\n", line_num, filename);
      goto out;
    }
    line_num++;
  }
 out:
  fclose(f);
  return err;
}

int delays_free(struct delay_set *delays) {
  struct delay *d, *t;
  LL_FOREACH_SAFE(delays->delays, d, t) {
    free(d);
  }
  delays->delays = 0;
  return 0;
}

static int check_addr(struct delay *delay, uint32_t addr) {
  switch (delay->type) {
  case DELAY_MASK:
    if ((delay->info.match.mask & addr) == delay->info.match.match)
      return 0;
    break;
  case DELAY_RANGE:
    if (delay->info.range.start <= addr && addr <= delay->info.range.end)
      return 0;
    break;
  }
  return -1;
}

int delays_lookup(struct delay_set *delays, uint32_t addr, int is_read,
                  uint32_t *delay, uint32_t *drop_delay) {
  struct delay *d;
  LL_FOREACH(delays->delays, d) {
    if (check_addr(d, addr) == 0) {
      if (is_read) {
        *delay = d->rd_delay;
        *drop_delay = d->rd_drop_delay;
      } else {
        *delay = d->wr_delay;
        *drop_delay = d->wr_drop_delay;
      }
      return 0;
    }
  }
  return -1;
}
