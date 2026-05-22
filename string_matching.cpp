#include <iostream>      
#include <fstream>      
#include <string>        
#include <vector>        
#include <map>           
#include <chrono>        
#include <algorithm>     
#include <iomanip>       
using namespace std;

string globalText = "";     // stores the currently loaded or entered text

int   g_comparisons = 0;   // total char comparisons in last search
int   g_matches     = 0;   // total matches found in last search
double g_timeMs     = 0.0; // Execution time in milliseconds of last search
string g_algorithm  = "";  // Which algorithm was used

//for case-insensitive searching
string toUpperCase(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper); 
    return s;                                             // return the modified uppercase string
}

// 5.3 part, Pattern Highlighting
string highlightPattern(const string& text, const string& pattern, const vector<int>& positions) {
    if (positions.empty()) return text;               // If no matches, return original text unchanged

    string result = "";                               // Will hold the final highlighted string
    int i = 0;                                        // Current index in the text
    int pLen = pattern.length();                      // Length of the pattern to highlight

    for (int pos : positions) {                       // Loop through each match position
        result += text.substr(i, pos - i);            // Append text before the match as it is
        result += "[" + text.substr(pos, pLen) + "]"; // So that it can be highlighted with brackets []
        i = pos + pLen;                               // Move index past the matched pattern
    }
    result += text.substr(i);                         // Append any remaining text after last match
    return result;                                    // Return the highlighted string
}

