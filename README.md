# Pattern-Matching

###Author(s) : 
Karthik Krishna Subramanian

---
###Problem Statement :

First, the algorithm will take as input a sequence of k > 1 textfiles T1, T2, ..., Tk, of
respective lengths n1, n2, ...nk, and it will adapt them in form of a suitable structure capable of 
supporting subsequent queries of the kind specified below.

A user will specify a query string, call it w, and one of the option modes ’search’ or
’stat’, each followed by suboption ’1’ or ’2’.

If the ’search’ mode is specified, the system must report the following. Under suboption
’1’: ’yes’ if w appears in the textfiles, ’no’ otherwise. Under suboption ’2’: if w appears
in the textfiles, the system reports the starting positions of all occurrences of w, it reports
’not found’ otherwise.

If the ’stat’ mode is specified, the system must report the following. Under suboption
’1’: the number of occurrences of w in the textfiles. Under suboption ’2’: the number and
name of the distinct textfiles that contain each at least one occurrence of w

---
###Disclaimer : 

This code was written as part of the course work for CSE 6241 : Pattern Matching at Georgia Institute of Technology. 
Any infringement of this code to use towards academic assignments/projects may lead to a violation of the institute's Honor Code, for which this author is not 
responsible. The code is to be seen merely as a guide to one possible approach on how to solve the above mentioned 
problem statement.

---
###Acknowledgments : 

1. The implementation of the suffix structure and the method to build a suffix
array is by [Suffix Array | Set 2 (nLogn Algorithm)](http://www.geeksforgeeks.org/suffix-array-set-2-a-nlognlogn-algorithm/)
and the code makes use of this tutorial to construct the suffix array itself. There have, however, been certain modifications
that have been made to the code to ensure compatibility to current requirements.

2. The input text files used to verify the code are snippets of publicly available filings regarding the performance of various companies. 
These files in particular were part of the the sample data used to test code outputs as part of the coursework
for the graduate course Machine Learning for Trading at Georgia Insitute of Technology.
The files were provided by Prof. Tucker Balch of Georgia Tech as part of his course.

---
###Compile and Execute : 

Requires Linux GCC.

1. Download all files into a directory, and open a terminal shell within this directory. 

2. Run `c++ match.cpp` on the terminal.

3. Once compiled, run `./a.out trial*.txt`

4. Follow Instructions displayed on the console

---
###Known Bugs :

1. Program gives inaccurate results when size of any individual text file goes beyond a certain point. The reason for 
this is as of yet unclear. The file size beyond which this anomaly happens is also yet to be determined.

2. The input query variable will accept only one word. Therefore the input provided has to be written without 
white spaces. In case of whitespaces being used, the string query takes in only the first word of the phrase.
Changing the call to read the input from the console to `std::getline(std::cin,std::string);` should help.
However, it fails to consistently provide the same result. Until I'm able to find out why,
the code will take in the input via `std::cin` directly. 

		
