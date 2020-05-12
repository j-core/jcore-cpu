void bzero(void *p, int n)
{
  char *c = p;

  while (n) c[n--] = 0;
}
