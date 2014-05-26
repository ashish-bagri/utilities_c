#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;
void get_numbers_from_comma(string str);
int get_number_from_str(string s);
bool is_int(string a);
void split_str_delimiter(string s, vector<string> &tokens, string delimiter);

void process_file(string in_file, void (*print_func)(string));
void parse_options_long(int argc, char** argv);
void parse_index(string index);
void print_help_exit();

void byte_print_range(string s);
void byte_print_indexes(string s);
void print_complement_indx(string s);
void field_print_index(string s);
void field_print_range(string s);
void escape_delimiter();

int indxstart = 1;
int indxend = -1;
string dvalue = "\t";
std::vector<size_t> indxvalues;
std::vector<string> input_file_names;
bool print_indx = false;
bool print_range = false;
int complement = 0;
bool fon, bon, don;

//#define DEBUG
int main(int argc, char* argv[])
{

	parse_options_long(argc,argv);
#ifdef DEBUG
	if(print_range)
	{
		cout<<"Start index: "<<indxstart<<endl;
		cout<<"End index: "<<indxend<<endl;
	}
	if(print_indx)
	{
		cout<<"Index values"<<endl;
		for(int x=0;x<indxvalues.size();x++)
		{
			cout<<indxvalues[x]<<" ";
		}
		cout<<endl;
	}
#endif

	if (indxend < indxstart && indxend!=-1)
	{
		cout<<"End smaller than start"<<endl;
		print_help_exit();
	}
	void (*print_func)(string);
	if(bon)
	{
		if(print_range)
			print_func = &byte_print_range;
		else if(print_indx)
			print_func = &byte_print_indexes;
	}else if (fon)
	{
		if(print_range)
			print_func = &field_print_range;
		else if(print_indx)
			print_func = &field_print_index;
	}else
	{
		print_help_exit();
	}

	int i = 0;
	for(i=0;i<input_file_names.size();i++)
	{
		process_file(input_file_names[i], print_func);
	}
	return 0;
}

void print_field_complement_index(vector<string> tokens)
{
	int i=0;
	for (i = 0;i < tokens.size(); i++)
	{
		// index i is not asked to be print, so print here as this is complement
		if(std::find(indxvalues.begin(), indxvalues.end(), i) == indxvalues.end())
		{
			cout<<tokens[i]<<dvalue;
		}
	}
	escape_delimiter();
}


void field_print_index(string s)
{
	vector<string> tokens;
	split_str_delimiter(s, tokens, dvalue);
	int num_tokens = tokens.size();
	if(num_tokens == 0)
	{
		// no delimiter found, just print the line
		cout<<s<<endl;
		return;
	}

	if(complement)
	{
		print_field_complement_index(tokens);
		return;
	}

	// must print indexes in indxvalues
	int i=0;
	for(i=0;i<indxvalues.size();i++){
		if(indxvalues[i] >= num_tokens)
			continue;
		cout<<tokens[indxvalues[i]]<<dvalue;
	}
	escape_delimiter();
}


void escape_delimiter()
{
	int d = 0;
	for(d=0;d<dvalue.length();d++) cout<<"\b";
	for(d=0;d<dvalue.length();d++) cout<<" ";
	cout<<endl;
}


void field_print_range(string s)
{
	vector<string> tokens;
	split_str_delimiter(s, tokens, dvalue);
	int num_tokens = tokens.size();
	if(num_tokens == 0)
	{
		// no delimiter found, just print the line
		cout<<s<<endl;
		return;
	}

	int end_indx;
	int start_indx = indxstart;

	if(indxend == -1 || indxend >= tokens.size())
		end_indx = tokens.size();
	else
		end_indx = indxend + 1; // to print inclusive of the end_indx

	if(!complement){
		int x = start_indx;
		for(;x<end_indx;x++)
		{
			cout<<tokens[x]<<dvalue;
		}
		escape_delimiter();
	}else
	{
		int x = 0;
		for(;x<start_indx;x++)
		{
			cout<<tokens[x]<<dvalue;
		}

		for(x=end_indx;x<tokens.size();x++)
		{
			cout<<tokens[x]<<dvalue;
		}
		escape_delimiter();
	}
}


void print_complement_indx(string s)
{
	int i=0;
	for (i = 0;i < s.length(); i++)
	{
		// index i is not asked to be print, so print here as this is complement
		if(std::find(indxvalues.begin(), indxvalues.end(), i) == indxvalues.end())
		{
			cout<<s[i];
		}
	}
	cout<<endl;
}

void byte_print_indexes(string s)
{
	if(complement)
	{
		print_complement_indx(s);
	}
	else
	{
		int i=0;
		for(i=0;i<indxvalues.size();i++){
			if(indxvalues[i] >= s.length())
				continue;
			cout<<s[indxvalues[i]];
		}
		cout<<endl;
	}
}

void byte_print_range(string s)
{
	if(indxend == indxstart && !complement)
	{
		if(indxstart > s.length())
		{
			cout<<endl;
		}else
		{
			cout<<s[indxend]<<endl;
		}
		return;
	}

	int end_indx;
	int start_indx = indxstart;

	if(indxend == -1 || indxend >= s.length())
		end_indx = s.length();
	else
		end_indx = indxend +1; // inclusive printing of last element

	if(!complement){
		if(start_indx < s.length())
			cout<<s.substr(start_indx, end_indx-start_indx)<<endl;
		else
			cout<<endl;

	}else
	{
		if(start_indx > 0)
		{
			cout<<s.substr(0, start_indx); // do not want to print start_indx
		}
		if(end_indx < s.length())
		{
			cout<<s.substr(end_indx, s.length() - end_indx);
		}
		cout<<endl;
	}
}

void process_file(string in_file, void (*print_func)(string))
{
	string STRING;
	std::ifstream input( in_file.c_str());

	for( string line; getline( input, line ); )
	{
		print_func(line);
	}
	input.close();
}

bool is_int(string a)
{
	int len = a.length();
	if (len == 0)
		return false;
	int i = 0;
	for(i=0;i<len;i++)
	{
		if(!isdigit(a[i]))
			return false;
	}
	return true;
}

void print_help_exit()
{
	cout<<"cut OPTIONS [FILE]"<<endl;
	cout<<"OPTIONS: "<<endl;
	cout<<"-b [start-end] indx,indx,...(specifies the bytes to select. Can be a \"-\" sepearated range or \",\" index-values and starts from 1)"<<endl;
	cout<<"-f [start-end] indx,indx,...(specifies the fields to select separated by delimiter specified -d. Can be a \"-\" sepearated range or \",\" index-values and starts from 1)"<<endl;
	cout<<"-d [delimiter] (delimiter to use to split the lines to select the fields. If not specified, defaults to tab(\"\\t\"))"<<endl;
	cout<<"--complement (if specified, selcted the complement of index specified by -b or -f)"<<endl;
	exit(1);
}

void parse_options_long(int argc, char** argv)
{
   fon = don = bon = false;
   int c;
   static struct option long_options[] =
	 {
	   {"complement", no_argument,       &complement, 1},
	   {"byte",     no_argument,       0, 'b'},
	   {"field",  no_argument,       0, 'f'},
	   {0, 0, 0, 0}
	 };
   int long_index = 0;

   while ((c = getopt_long(argc, argv,"b:f:d:",
           long_options, &long_index )) != -1) {
	   switch (c)
	  {
	   	   case 0:
	   		   break;
	  	  case 'b':
	  		parse_index(optarg);
	  		  bon = true;
	  		  break;
	  	  case 'f':
	  		parse_index(optarg);
	  		fon = true;
	  		break;
	  	  case 'd':
	  		  dvalue = optarg;
	  		  if(dvalue.length() > 1)
	  		  {
	  			  cout<<"The delimiter must be a single character"<<endl;
	  			  print_help_exit();
	  		  }
	  		  don = true;
	  		  break;
	  	  default:
	  		  print_help_exit();
	  		  break;
	  }
   }
   if(!bon && !fon)
   {
	   cout<<"No index ranges specified"<<endl;
	   print_help_exit();
   }

   if(fon && bon)
   {
	   cout<<"Field value and byte value cannot be specified together."<<endl;
	   print_help_exit();
   }
   if(bon && don)
   {
	   cout<<"delimiter value can only be specified on fields"<<endl;
	   print_help_exit();
   }
   int index;
   for (index = optind; index < argc; index++)
   {
	   input_file_names.push_back(argv[index]);
   }
   if(input_file_names.size() == 0)
   {
	   cout<<"No input file specified"<<endl;
	   print_help_exit();
   }
}


void parse_index(string index)
{
	size_t found;
	size_t found_dash;

    if(is_int(index))
	{
		indxstart = get_number_from_str(index) - 1;
		indxend = indxstart;
		print_range = true;
	}
	else
	{
		// can have infinte comma's !!
		found = index.find_first_of(",");
		found_dash = index.find_first_of("-");

		if(found != std::string::npos && found_dash != std::string::npos)
		{
			cout<<"Index can only be specified as \"-\" or \",\""<<endl;
			print_help_exit();
		}

		if (found != std::string::npos)
		{
			get_numbers_from_comma(index);
			print_indx = true;
		}
		else if(found_dash != std::string::npos)
		{
			if(found_dash == 0)
			{
				indxstart = 0;
				string indxendstr = index.substr(1);
				if(!is_int(indxendstr)) {
					cout<<"Index values should be integers"<<endl;
					print_help_exit();
				}
				indxend = get_number_from_str(indxendstr) - 1;
				print_range = true;
			}
			else if(found_dash == index.length()-1)
			{
				string indxstartstr = index.substr(0,index.length()-1);
				if(!is_int(indxstartstr)) {
					cout<<"Index values should be integers"<<endl;
					print_help_exit();
				}
				indxstart = get_number_from_str(indxstartstr) - 1;
				indxend = -1;
				print_range = true;
			}else
			{
				string indxstartstr = index.substr(0,found_dash);
				string indxendstr = index.substr(found_dash+1, index.length());
				if(!is_int(indxstartstr) || !is_int(indxendstr)) {
					cout<<"Index values should be integers"<<endl;
					print_help_exit();
				}
				indxstart = get_number_from_str(indxstartstr) - 1;
				indxend = get_number_from_str(indxendstr) - 1;
				print_range = true;
			}
		}
		else
		{
			cout<<"Wrong indexing"<<endl;
			print_help_exit();
		}
	}
}

int get_number_from_str(string s)
{
	int ret;
	stringstream(s) >> ret;
	return ret;
}

void get_numbers_from_comma(string str)
{
	vector<string> tokens;
	split_str_delimiter(str, tokens, ",");
	for(int i=0;i<tokens.size();i++)
	{
		if(is_int(tokens[i]))
		{
			indxvalues.push_back(get_number_from_str(tokens[i]) -1);
		}else
		{
			cout<<"Index values must be integers"<<endl;
			print_help_exit();
		}
	}
	std::sort (indxvalues.begin(), indxvalues.end());
}

void split_str_delimiter(string s, vector<string> &tokens, string delimiter)
{
	size_t pos = 0;
	string token;
	if(s.find(delimiter) == std::string::npos)
	{
		return;
	}

	while ((pos = s.find(delimiter)) != std::string::npos) {
	    token = s.substr(0, pos);
	    tokens.push_back(token);
	    s.erase(0, pos + delimiter.length());
	}
	tokens.push_back(s);
}
