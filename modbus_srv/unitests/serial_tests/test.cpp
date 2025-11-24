#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


#include <iostream>
#include <list>
#include <numeric>

using namespace std;

void print(const list<double> &lst)
{
list<double>::const_iterator p;
for(p=lst.begin(); p!=lst.end();++p) 
	cout<<*p<<'\t';
cout<<endl;
}

int main ()
{
list<double> z;

for (int i=1;i<5;i++)
	z.push_front(i*i);
print(z);
cout<<"accumulate "<<accumulate(z.begin(), z.end(), 0.0)<<endl;
count<<z[1]<<endl;
}