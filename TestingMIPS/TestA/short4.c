
void printint(int);
void printchar(char);

int sideffect()
{
   printint(666);  // Will NOT see this if this gets evaulated
   printchar(10); // newline
   return 1;
}


void main()
{
  int x, y;

  x = 17; 
  y = 17;

  if (x==y || sideffect()==1) {
       printint(777);  // should see this
       printchar(10); // newline
  };
}

