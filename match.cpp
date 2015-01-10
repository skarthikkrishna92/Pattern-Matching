//External Libraries
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <numeric>
/*
Libraries Required for String Manipulation. "cstring" and "iterator" become necessary 
because of the parameter type restrictions on various library function calls.
*/
#include <string>
#include <cstring>
#include <iterator>
/*
Library Required for File Manipulation
*/
#include <fstream>
/*
Library required to build suffix array.
*/
#include <algorithm>

using namespace std;

//List of File Names
vector<string> file_names;

//Vector where each string in the list contains all the data of the given text filedata
vector<string> file_data;

//Vector that stores the suffix array for each file
vector<int *>  suffix_array;

/*
Structure that contains information about a given suffix of the
string. The index holds the original position of the suffix in the
text, and the rank array is used in a repeated fashion to 
sort through the data.
*/
struct suffix
{
	int index;
	int rank[2];
};

/*
The comparison function fed to the C++ sort function. The function
takes in two suffix structure instances as input, and compares them based
on the value of their rank. Comparison based on rank is based on the
lexicographic comparison of the corresponding elements in the string in question. The 
comparison function is used as the defining criterion by the sort algorithm while
ordering the input data.
*/
int compare(suffix value_1, suffix value_2)
{
	if(value_1.rank[0] == value_2.rank[0])
	{
		if(value_1.rank[1] < value_2.rank[1])
		{
			return 1;
		}
		
		else
		{
			return 0;
		}
	}

	else
	{
		if(value_1.rank[0]<value_2.rank[0])
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

/*
The function that takes in the input string, and builds the suffix array for said string. If 
observed, when the function is called, the size of the string is denoted by string.size()-1. 
This is because the data read from the file seems to include the final null character as a 
part of the text as well. So in order to avoid \0 causing any issues in the lexicographic 
ordering of suffixes, we disregard it.
*/
int *buildSuffixArray(string input_string, int str_len)
{
    // A structure to store suffixes and their indexes
    struct suffix suffixes[str_len];
 
 	/*
	We sort the suffixes- First according to the first character, then 
	according to the next two, then 4, and so on.
	*/

 	//Populate the variable values for each struct suffix element
    for (int i = 0; i < str_len; i++)
    {
        //Store the Index of each suffix of the string in the corresponding structure instance
        suffixes[i].index = i;

        /*
        Since preprocessing involved removing punctuations, whitespaces, and converting to 
        lower case, the ranking is based on the difference of the element with 'a'. This 
        relative quantification of the character allows for a direct ranking system in the first pass.
        */ 
        suffixes[i].rank[0] = input_string[i] - 'a';
        if((i+1)<str_len)
        {
        	suffixes[i].rank[1] = input_string[i+1] - 'a';
        }
        else
        {
        	suffixes[i].rank[1] = -1;
        }

    }
 
    // Sort the suffixes using the comparison function defined above.
    sort(suffixes, suffixes+str_len, compare);
 
    /*
    At this point, all suffixes are sorted according to first
    2 characters. We continue the process henceforth by powers of two. The 
    ordering of the suffixes will be based on their rank. The ranking process 
    changes below.
    */

    //Required to get the index in suffixes[]. We can use that to find the next and previous suffixes
    int indices[str_len];  

    //Run throughout the string in powers of 2
    for (int k = 4; k < 2*str_len; k = k*2)
    {
        // Assigning initial values to variables based on the first values of struct suffix variable suffixes
        int rank = 0;
        int previous_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        indices[suffixes[0].index] = 0;
 
        // Assigning Rank[0] to suffixes
        for (int i = 1; i < str_len; i++)
        {
            /*
			If the rank pair values for the previous and current suffix are the same,
			assign the same rank[0] value to the current suffix as the previous one
            */
            if ((suffixes[i].rank[0] == previous_rank) && (suffixes[i].rank[1] == suffixes[i-1].rank[1]))
            {
                previous_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            }

            // Otherwise increment rank and assign to rank[0]
            else 
            {
                previous_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }

            //Keep track of the indices
            indices[suffixes[i].index] = i;
        }
 
        // Assign next rank to every suffix traversing towards the end
        for (int i = 0; i < str_len; i++)
        {
            //Takes next_index forward by the current power of 2
            int next_index = suffixes[i].index + k/2;
            
            if(next_index < str_len)
            {
            	suffixes[i].rank[1] = suffixes[indices[next_index]].rank[0];
            }
            else
            {
            	suffixes[i].rank[1] = -1;
            }
        }
 
        // Sort the suffixes according to first k characters
        sort(suffixes, suffixes+str_len, compare);
    }
 
    // Store indexes of all sorted suffixes in the suffix array
    int *suffix_array = new int[str_len];
    for (int i = 0; i < str_len; i++)
    	{
    		//Store the sorted indices of the str_len suffixes in the suffix array
    		suffix_array[i] = suffixes[i].index;
    	/*
		Code Commented Out to Verify the Constructed Suffix Array to ensure that ordering is maintained
    	*/
    	//cout<<"Suffix Array at Index "<<i<<" points to string starting from index : "<<suffix_array[i]<<endl;
    	//cout<<"Value: "<<file_data[0].substr(suffix_array[i],file_data[0].size())<<endl;
    	}
 
    // Return the suffix array
    return  suffix_array;
}

/*
Function that takes care of most of the work apart from the construction of thee suffix 
array. We iterate through each of the suffix arrays present for the given query string.
*/
void solve(int option, int suboption, string query)
{
	//Count The Number of Occurrences of the query per file.
	vector<int> count_occurences;

	//Monitor whether there is an occurence in each of the files
	vector<bool> found;

	//Final Result - Is the string present or not
	bool final_result = false;

	//Count the number of files in which at least one occurrence happens
	int num_of_files = 0;

	//Iterate through all elemenets in suffix array
	for(int i = 0 ; i < suffix_array.size() ; i++)
	{
		//Initialize parameters for binary search
		int len = file_data[i].size()-1;
		int mid = len/2;
		int high = len-1;
		int low = 0;

		//Maintin count when matching the query against the string in a supposedly appropriate suffix array index
		int local_count = 0;

		//Option 01-SubOption01 requires to terminate after finding one single occurrence
		bool run_for_the_rest = true;

		//Used to Populate vector<bool> found using the local variable
		bool found_current = false;
		
		/*
		Debugging Code to verify Step-Wise Results 
		*/
		//cout<<"In File: "<<file_names[i]<<endl;
		//cout<<"Entering While Loop"<<endl;
		while(low<high)
		{
			//cout<<"Value of Low: "<<low<<endl;
			//cout<<"Value of Mid: "<<mid<<endl;
			//cout<<"Value of High: "<<high<<endl;
			//cout<<"file_data[i][suffix_array[i][mid] + local_count]: "<<file_data[i][suffix_array[i][mid] + local_count]<<endl;
			//cout<<"query[local_count]: "<<query[local_count]<<endl;
			
			//In case of a match
			if(file_data[i][suffix_array[i][mid] + local_count] == query[local_count])
			{	
				if(local_count == query.size()-1)
				{
					found_current = true;
					if(option == 1)
					{
						if(suboption ==1)
						{
							//Found one occurrence. Print to Console 
							cout<<"Yes"<<endl;	

							//Exit after printing to console.
							run_for_the_rest = false;	
							//cout<<"Comparison Yeilded: "<<strncmp((char*)file_data[i].substr(suffix_array[i][mid],suffix_array[i][mid] + query.size()-1).c_str(), (char *)query.c_str(), query.size()-1)<<endl;
						}
						
						else
						{
							//List out indices where the value is present in the current file. 
							cout<<endl<<"String: "<< query<< " located in File "<<file_names[i]<<" at indices: "<<endl;
							int gohigh = mid;
							int golow = mid-1;
							
							//If there are matching patterns which can be found in the suffix array after the location of mid
							while(strncmp((char*)file_data[i].substr(suffix_array[i][gohigh],suffix_array[i][gohigh] + query.size()).c_str(), (char *)query.c_str(), query.size()) == 0)
							{
								//cout<<"Parent String Value: "<<file_data[i].substr(suffix_array[i][gohigh],suffix_array[i][gohigh] + query.size())<<endl;
								cout<<"Index: "<<suffix_array[i][gohigh]<<endl;
								gohigh++;
							}

							//If there are matching patterns which can be found in the suffix array before the location of mid
							while(strncmp((char*)file_data[i].substr(suffix_array[i][golow],suffix_array[i][low] + query.size()).c_str(), (char *)query.c_str(), query.size()) == 0)
							{
								//cout<<"Parent String Value: "<<file_data[i].substr(suffix_array[i][golow],suffix_array[i][golow] + query.size())<<endl;
								cout<<"Index: "<<suffix_array[i][golow]<<endl;
								golow--;
							}
						}
				
					 	break;
					}

					else
					{
						if(suboption == 1)
						{
							//Maintain a count of total number of occurences in the given file
							int count = 0;
							int gohigh = mid;
							int golow = mid-1;
							while(strncmp((char*)file_data[i].substr(suffix_array[i][gohigh],suffix_array[i][gohigh] + query.size()).c_str(), (char *)query.c_str(), query.size()) == 0)
							{
								count++;
								gohigh++;
							}
							while(strncmp((char*)file_data[i].substr(suffix_array[i][golow],suffix_array[i][low] + query.size()).c_str(), (char *)query.c_str(), query.size()) == 0)
							{
								count++;
								golow--;
							}

							//Add total count of occurences in current file to vector.
							count_occurences.push_back(count);

						}
						else 
						{
							//Required to print total number of files in which pattern occurs.
							num_of_files++;

							//Maintain count to display individual stats
							int count = 0;

							cout<<endl<<"The string \""<< query<< "\" has occured: ";
							int gohigh = mid;
							int golow = mid-1;
							while(strncmp((char*)file_data[i].substr(suffix_array[i][gohigh],suffix_array[i][gohigh] + query.size()).c_str(), (char *)query.c_str(), query.size()) == 0)
							{
								//cout<<"Parent String Value: "<<file_data[i].substr(suffix_array[i][gohigh],suffix_array[i][gohigh] + query.size())<<endl;
								count++;
								gohigh++;
							}
							while(strncmp((char*)file_data[i].substr(suffix_array[i][golow],suffix_array[i][low] + query.size()).c_str(), (char *)query.c_str(), query.size()) == 0)
							{
								//cout<<"Parent String Value: "<<file_data[i].substr(suffix_array[i][golow],suffix_array[i][golow] + query.size())<<endl;
								count++;
								golow--;
							}
							cout<<count<<" times in File "<<file_names[i]<<endl<<endl;
							
							count_occurences.push_back(count);

							if(i < suffix_array.size())
							{
								//cout<<"i value: "<<i<<endl;
								//getchar();
							}
						}

						break;
					}
				}
				
				else
				{
					//Same if condition also used to map query to the prefix of the suffix until we hit the length of the query word.
					local_count++;	
				}
				
			}
			else if(file_data[i][suffix_array[i][mid]+local_count] > query[local_count])
			{
				//cout<<"text[sf[i]] > query[i]. Look at previous sf values."<<endl;
				//cout<<"Local Count Value: "<<local_count<<endl;
				//cout<<"Inside Binary Search Loop"<<endl;
				high = mid-1;
				mid = (high + low)/2;	
				local_count = 0;			
			}
			else if(file_data[i][suffix_array[i][mid]+local_count] < query[local_count])
			{
				//cout<<"text[sf[i]] < query[i]. Look at next sf values."<<endl;
				//cout<<"Local Count Value: "<<local_count<<endl;
				//cout<<"Inside Binary Search Loop"<<endl;
				low = mid+1;
				mid = (high+low)/2;
				local_count = 0;
			}
			//Control to be able to pause through iterations while debugging
			//getchar();
			
		}

		//Populate the found to keep count of occurences
		found.push_back(found_current);

		//If we have to run the test for the other files as well (i.e., for all other cases other than Opt1-SubOpt1)
		if((i!=suffix_array.size()-1) &&(run_for_the_rest == true))
		{
			continue;
		}
		else
		{
			break;
		}

		getchar();
	}

	//After having iterated through all files, calculate and print number of occurences to console
	if(option == 2 && suboption ==1)
	{
		int sum_of_elems =std::accumulate(count_occurences.begin(),count_occurences.end(),0);
		cout<<"The Query: "<<query<<"has occurred "<<sum_of_elems<<" times in the text files."<<endl;
	}

	//After having iterated through all files, calculate and print the number of files in which the query has occurred.
	else if(option ==2 && suboption ==2)
	{
		cout<<"The query word has appeared in "<<num_of_files<<" files."<<endl;
	}

	//Check if the string was not at all present in any of the files
	for(int i = 0 ; i< found.size(); i++)
	{
		if(found[i] == true)
		{
			final_result = true;
			break;
		}
	}
	if(!final_result)
	{
		cout<<"No Such Word Found in the given TextFiles"<<endl;
	}
}

//Main() of the program
int main(int argc, char* argv[])
{
	//Number of Files
	int k = 0;

	//File Stream Object to Read each of the files.
	ifstream input_file ; 

	/*
	Make sure that you have more than 2 arguments. Note that
	this doesn't ensure that the arguments are handled appropriately. 
	The number of documents provided should match the quantity mentioned 
	as the first command line argument 
	*/
	if(argc > 2)
	{
		//Take in Number of Files
		k = atoi(argv[1]);
	}
	for(int i = 0 ; i < k; i++)
	{
		//Take in File Names of k number of files
		file_names.push_back(argv[i+2]);
	}
	for(int j = 0 ; j < k ; j++)
	{
		//Format File Name to Match Function Signature of STL and open File
		char *current_file= (char *)file_names[j].c_str();
		input_file.open(current_file);

		//Copy The Entire Data of the file on to a string variable, and save it in the file_data vector
    	istreambuf_iterator<char> beg(input_file), end;
    	string text = string(beg, end);

    	//Format Text Data to remove whitespaces and punctuations
    	text.erase(remove_if(text.begin(), text.end(), ::isspace), text.end());
    	text.erase(remove_if(text.begin(), text.end(), ::ispunct), text.end());
    	transform(text.begin(), text.end(), text.begin(), ::tolower);

    	//Store the data
    	file_data.push_back(text);

    	//Use the text data to build a suffix array, and store the resultant array
    	suffix_array.push_back(buildSuffixArray(file_data[j],file_data[j].size()-1));

    	//File Handling - Clean-up loose ends
    	input_file.close();
	}

	//Main Task
	do
	{
		int option = 0;
		int suboption = 0;
		char choice = 'Y';
		string query ;
	
		//Prompt run every time a task is done. Any incorrect key entry at any stage will force an exit
		cout<<"Options- \n"<<endl;
		cout<<"Search: "<<endl;
		cout<<"\t1. Print \"Yes\" if query is found ; \"No\" otherwise."<<endl;
		cout<<"\t2. Print all starting occurences of query word. Print \"Not Found\" if does not exist."<<endl;
		cout<<"\nStat: "<<endl;
		cout<<"\t1. Number of occurences of the query in the text files."<<endl;
		cout<<"\t2. Number and name of distinct text files that contain at least one occurence of the query."<<endl;
		cout<<"\nEnter the option number [1,2]. Enter any other character to exit."<<endl;
		cin>> option;
		if(option!=1 && option!=2)
		{
			break;
		}
		cout<<"\nEnter suboption number [1,2]. Enter any other character to exit."<<endl;
		cin>>suboption;
		if(suboption!=1 && suboption!=2)
		{
			break;
		}	
		cout<<"Enter Query String: ";
		cin>>query;

		//Transform the data as we had the text
		transform(query.begin(), query.end(), query.begin(), ::tolower);
		query.erase(remove_if(query.begin(), query.end(), ::isspace), query.end());
		query.erase(remove_if(query.begin(), query.end(), ::ispunct), query.end());
		
		//Given the appropriate parameters, solve the question and print the output to console
		solve(option,suboption,query);

		//Repeat until user quits
		cout<<"Continue Again (Press 'Y' to continue, any other character to exit)?: ";
		cin>>choice;
		if(choice != 'Y')
		{
			break;
		}	
	}while(true);

	//Exit the program
	return 0;
}
