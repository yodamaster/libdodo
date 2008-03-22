
#include <libdodo/regexp.h>
#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{	
	regexp expr;
	dodoStringArray pock;
	
	expr.multiline = true;
	expr.greedy = false;

	std::string sample = "string1234\n";

	std::string pattern = "^(.*)([0-9]+)([^0-9]+)";

	cout << "\nMatch: pattern: " << pattern << "\n\ntest string: " << sample << "\n\npockets:\n";
	
	if (expr.match(pattern,sample,pock))
	{
		cout << "size: " << pock.size() << "\n";
		
		cout << "1:" << pock[0] << "\n";
		cout << "2:" << pock[1] << "\n";
		cout << "3:" << pock[2] << "\n";
	}
	else
		cout << "not matched";
	
	sample = "string12345string";
	cout << "\nReplacement: pattern: " << pattern << "\n\ntest string: " << sample << "\n\nreplace with '!!!', '@@'\nResult:\n";
	
	dodoStringArray a;
	a.push_back("!!!");
	a.push_back("@@");
	cout << expr.replace(pattern, sample, a) << "\n";
	
	return 0;
}