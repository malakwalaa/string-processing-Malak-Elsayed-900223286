#include <iostream>      
#include <fstream>      
#include <string>        
#include <vector>        
#include <map>           
#include <chrono>        
#include <algorithm>     
#include <iomanip>       
using namespace std;


string globalPattern = ""; // The pattern entered by the user to search for
vector<int> globalMatches; // Positions where the pattern was found
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

// For better visualization
void printDivider() {
    cout << "=============================================" << endl; 
}
//5.6 Statistics Output
void printStatistics() {
    printDivider();
    cout << "  Search Statistics" << endl;                         // Header label
    printDivider();
    cout << "  Algorithm   : " << g_algorithm   << endl;          // Which algorithm ran
    cout << "  Comparisons : " << g_comparisons << endl;          // Total comparisons made
    cout << "  Matches     : " << g_matches     << endl;          // Total matches found
    cout << "  Time (ms)   : " << g_timeMs << " ms" << endl;      // Execution time
    printDivider();
}

void loadFile() {
    printDivider();
    cout << "Load Text From File " << endl;           
    printDivider();
    cout << "  Enter file name: ";                     
    string fileName;                                   
    cin >> fileName;                                   // no spaces expected
    cin.ignore();                                      

    ifstream file(fileName);                           
    if (!file.is_open()) {                             
        cout << "\n  ERROR Could not open: " << fileName << endl; 
        return;                                        
    }

    globalText = "";                                   // Clear any previously stored text
    string line;                                       
    while (getline(file, line)) {                      // Read the file line by line until EOF
        if (!globalText.empty()) globalText += " ";   // Add space between lines when joining
        globalText += line;                            // Append this line to the global text
    }
    file.close();                                      

    cout << "\n  File loaded! (" << globalText.length() << " characters)" << endl; 
    cout << "  Preview: " << globalText.substr(0, min((int)globalText.length(), 80)) << "..." << endl;
}

void manualInput() {
    printDivider();
    cout << "Manual Text Input" << endl;             
    printDivider();
    cout << "  Enter text: ";                          
    cin.ignore();                                     
    getline(cin, globalText);                          // Read the entire line including spaces
    cout << "\n Text stored (" << globalText.length() << " characters)" << endl; 
}

void boyerMooreSearch() {
    
    g_algorithm   = "Boyer-Moore";   
    g_comparisons = 0;               // Reset comparison counter to zero
    g_matches     = 0;               // Reset match counter to zero
    g_timeMs      = 0.0;             // Reset timer to zero
    globalMatches.clear();           // Clear any previous match positions

    string text    = toUpperCase(globalText);    // Convert text to uppercase for case-insensitive search
    string pattern = toUpperCase(globalPattern); // Convert pattern to uppercase too
    int n = text.length();                       // Length of the text being searched
    int m = pattern.length();                    // Length of the pattern being searched for

    if (m == 0 || n == 0 || m > n) {            // handle empty or impossible cases
        cout << "  [ERROR] Invalid text or pattern." << endl;
        return;                                  
    }

    
    // Maps each character to its last (rightmost) position in the pattern
    map<char, int> badChar;                      // Bad character shift table
    for (int i = 0; i < m; i++) {               // Loop through every character in the pattern
        badChar[pattern[i]] = i;                 // Store the last index where this character appears
    }

    
    auto startTime = chrono::high_resolution_clock::now(); // Record the time before searching begins

   
    int s = 0;                                   
    while (s <= n - m) {                         // Keep searching while pattern can still fit
        int j = m - 1;                           // Start comparing from the rightmost character of pattern

        // Compare pattern to text from right to left
        while (j >= 0 && pattern[j] == text[s + j]) { // While characters match
            g_comparisons++;                     // Count each successful comparison
            j--;                                 // Move one step left in the pattern
        }

        if (j >= 0) g_comparisons++;             // Count the final mismatching comparison

        if (j < 0) {                             // j went below 0 = full pattern matched
            globalMatches.push_back(s);          // Save this match position
            g_matches++;                         // Increment match counter
            s += 1;                              // Shift by 1 to tackle overlapping matches
        } else {
            // use bad character rule to decide how far to shift
            char mismatch = text[s + j];         // The text character that did not match
            int shift = badChar.count(mismatch)  // Is this character somewhere in the pattern?
                        ? j - badChar[mismatch]  // if Yes: shift so its last occurrence aligns with j
                        : j + 1;                 // else No: shift pattern completely past this character
            s += max(1, shift);                  // Always shift at least 1 to avoid infinite loop
        }
    }


    auto endTime = chrono::high_resolution_clock::now();                         // Record end time
    g_timeMs = chrono::duration<double, milli>(endTime - startTime).count();     

    
    cout << "\n  Pattern: \"" << globalPattern << "\"" << endl; // Show the searched pattern
    if (globalMatches.empty()) {                 // No matches found
        cout << "  Result: No match found." << endl;
    } else {                                     // One or more matches found
        for (int pos : globalMatches) {          // Loop through every match position
            cout << "  Match at index: " << pos << endl; 
        }
        cout << "\n  Highlighted Text:" << endl; 
        cout << "  " << highlightPattern(globalText, globalMatches, m) << endl;
    }
}
void rabinKarpSearch() {
    
    g_algorithm   = "Rabin-Karp";    
    g_comparisons = 0;               // Reset comparison counter
    g_matches     = 0;               // Reset match counter
    g_timeMs      = 0.0;             // Reset timer
    globalMatches.clear();           // Clear previous match positions

    string text    = toUpperCase(globalText);    // Uppercase text for case insensitive search
    string pattern = toUpperCase(globalPattern); // Uppercase pattern too
    int n = text.length();                       // Length of the text
    int m = pattern.length();                    // Length of the pattern

    if (m == 0 || n == 0 || m > n) {            //invalid or impossible input
        cout << "  [ERROR] Invalid text or pattern." << endl;
        return;                                 
    }

   
    const int BASE = 256;            // number of possible ASCII characters
    const int MOD  = 101;            // a prime number to keep hash values small

    int patternHash = 0;             // Hash value computed from the pattern
    int textHash    = 0;             
    int h           = 1;             

    
    auto startTime = chrono::high_resolution_clock::now(); // Record start time

    // Compute h = BASE^(m-1) % MOD
    for (int i = 0; i < m - 1; i++) { // Loop m-1 times
        h = (h * BASE) % MOD;          // Multiply by BASE each time and keep within MOD
    }

    // Compute initial hash for pattern and first window of text
    for (int i = 0; i < m; i++) {                              // Loop through first m characters
        patternHash = (BASE * patternHash + pattern[i]) % MOD; // Build pattern hash
        textHash    = (BASE * textHash    + text[i])    % MOD; // Build first window hash
    }

   
    for (int i = 0; i <= n - m; i++) {    // Slide window one character at a time across text
        g_comparisons++;                   // Count this hash comparison

        if (patternHash == textHash) {     
            bool match = true;             // Assume it's a real match until proven otherwise
            for (int k = 0; k < m; k++) { // Verify character by character
                g_comparisons++;           // Count each character comparison
                if (text[i + k] != pattern[k]) { // Characters differ: this was a false positive
                    match = false;         // Mark as not a real match
                    break;                 
                }
            }
            if (match) {                   // All characters matched: confirmed real match
                globalMatches.push_back(i);// Save this match position
                g_matches++;               // Increment match counter
            }
        }

        
        if (i < n - m) {                                                    // Don't compute past end
            textHash = (BASE * (textHash - text[i] * h) + text[i + m]) % MOD; // Remove left, add right
            if (textHash < 0) textHash += MOD;  // Handle negative values 
        }
    }

  
    auto endTime = chrono::high_resolution_clock::now();                     // Record end time
    g_timeMs = chrono::duration<double, milli>(endTime - startTime).count(); 

   
    cout << "\n  Pattern: \"" << globalPattern << "\"" << endl; // Show searched pattern
    if (globalMatches.empty()) {             // No matches found
        cout << "  Result: No match found." << endl;
    } else {                                 // One or more matches found
        for (int pos : globalMatches) {      // Loop through all match positions
            cout << "  Match at index: " << pos << endl; 
        }
        cout << "\n  Highlighted Text:" << endl; 
        cout << "  " << highlightPattern(globalText, globalMatches, m) << endl;
    }
}


