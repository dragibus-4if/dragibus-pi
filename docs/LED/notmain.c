int
bar(int param)
{
  int result;
  result = param / 3;
  return result;
}

int
foo()
{
  int local;
  local = bar(42);
  return local;
}

int
notmain ( void )
{
  int code;
  code = foo();
  return code;
}
