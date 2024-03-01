#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void printPattern(vector<uint16_t> indices);
void printPattern(vector<uint8_t> pattern);
void writePattern(vector<uint8_t> pattern, ostream &outputFile);
void printDictionary(vector<vector<uint8_t>> dict);

//------------------------------------------------------------------------------
// binary version
//------------------------------------------------------------------------------
bool isEqualPattern(vector<uint8_t> pattern1, vector<uint8_t> pattern2);
int findPatternInDictionary(vector<vector<uint8_t>> &dictionary, vector<uint8_t> patternToFind);
vector<uint8_t> subPattern(vector<uint8_t> pattern, int startIndex, int lengthOfSubPattern);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Function Definition
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void printPattern(vector<uint16_t> indices) {
    cout << "(";
    for (int i = 0; i < indices.size(); i++) {
        cout << hex << setw(4) << setfill('0') << (int)indices[i]; // takes the first byte not character
        if (i < indices.size() - 1) { // do not print space for after the last element
            cout << " ";
        }
    }
    cout << ")";
}

// (00 FF 2E)
// (00 12 3f 2d)
void printPattern(vector<uint8_t> pattern) {
    cout << "(";
    for (int i = 0; i < pattern.size(); i++) {
        cout << hex << setw(2) << setfill('0') << (int)pattern[i]; // takes the first byte not character
        if (i < pattern.size() - 1) { // do not print space for after the last element
            cout << " ";
        }
    }
    cout << ")";
}

void writePattern(vector<uint8_t> pattern, ostream &outputFile) {
    for (int i = 0; i < pattern.size(); i++) {
        outputFile.put(pattern[i]); // takes the first byte not character
    }
}

// dict: [(00 FF 2E) (00) (01) ]
void printDictionary(vector<vector<uint8_t>> dict) {
    cout << "[";
    for (int i = 0; i < dict.size(); i++) {
        printPattern(dict[i]);
        if (i < dict.size() - 1) { // do not print space for after the last element
            cout << " ";
        }
    }
    cout << "]";
}

//------------------------------------------------------------------------------
// binary version
//------------------------------------------------------------------------------
bool isEqualPattern(vector<uint8_t> pattern1, vector<uint8_t> pattern2) {
    if (pattern1.size() != pattern2.size()) {
        return false;
    }
    
    for (int i = 0; i < pattern1.size(); i++) {
        if (pattern1[i] != pattern2[i]) {
            return false;
        }
    }
    
    return true;
}

int findPatternInDictionary(vector<vector<uint8_t>> &dictionary, vector<uint8_t> patternToFind) {
    int indexFound = -1;
    
    for (int i = 0; i < dictionary.size(); i++) {
        if (isEqualPattern(dictionary[i], patternToFind)) { // class string can be compared using ==
            indexFound = i;
            break;
        }
    }
    
    //    cout << "indexFound: " << indexFound << endl;
    return indexFound;
}

vector<uint8_t> subPattern(vector<uint8_t> pattern, int startIndex, int lengthOfSubPattern) {
    vector<uint8_t> newPattern;
    for (int i = startIndex; i < startIndex + lengthOfSubPattern; i++) {
        newPattern.push_back(pattern[i]);
    }
    return newPattern;
}

// encode binary file
void encode(ifstream &input, fstream &output) {
    cout << "encode(): started" << endl;
    
    //--------------------------------------------------------------------------
    // initialize dict
    //--------------------------------------------------------------------------
    vector<vector<uint8_t>> initialDictionary;
    //--------------------------------------------------------------------------
    // the type of elements of dictionary = vector<uint8_t> = one or more bytes = the type of the pattern
    //--------------------------------------------------------------------------
    // 00
    // 01
    // 02
    // ...
    // FF = 255
    // -------------------------------------- initial dictionary
    // 00 FF 02
    
    for (int i = 0; i < 256; i++) { // create initial dictionary
        vector<uint8_t> curPattern;
        curPattern.push_back(i);
        initialDictionary.push_back(curPattern);
    }
    
    //--------------------------------------------------------------------------
    // LZW algorithm
    //--------------------------------------------------------------------------
    vector<vector<uint8_t>> dictionary = initialDictionary;
    
    //--------------------------------------------------------------------------
    // Encoding
    //--------------------------------------------------------------------------
    // 1,0,4,3,5...
    // uint8_t index = 0-255; <- we can't use this one because the amount of patterns in the dictionary will be more than 256
    //              Example: the dictionary alphabet is 0-255, when encoding, more patterns are added to the dictionary, increasing the amount of indexes to over 256, if encodedOutput wants to store them it has to use more than one byte
    // uint16_t index = 0 - 65535;
    
    vector<uint16_t> encodedOutput; // array of indices of patterns in dict
    
    vector<uint8_t> totalInput;
    vector<uint8_t> current;
    
    bool lastRound = false;
    
    while (true) {
        //cout << "----------------------------------------" << endl;
        
        if (lastRound == true) {
            break;
        }
        
        uint8_t curByte = input.get(); // add the current byte in input to ch
        if (input.eof()) {
            //            break;
            lastRound = true;
        }
        
        // now ch has valid input(a character).
        //cout << ch[0];
        totalInput.push_back(curByte);
        //cout << "encode(): Input: ";
        //printPattern(totalInput);
        //cout << endl;
        
        current.push_back(curByte);
        //cout << "encode(): Current String: ";
        //printPattern(current);
        //cout << endl;
        
        // seen this(current) before?
        int indexFound = -1;
        indexFound = findPatternInDictionary(dictionary, current);
        if (indexFound != -1) { // seen before
            //            cout << "indexFound = " << indexFound << endl;
            
            //cout << "encode(): Seen this Before? yes" << endl;
            
            //cout << "encode(): Encoded Output: ";
            //printPattern(encodedOutput);
            //cout << endl;
            //cout << "encode(): New Dictionary Entry/Index: none" << endl;
        }
        else { // NOT FOUND: add something to encodedOutput
            //cout << "encode(): Seen this Before? no" << endl;
            
            //            indexFound = findStringInDictionary(dictionary, current.substr(0, current.size()-1));
            indexFound = findPatternInDictionary(dictionary, subPattern(current, 0, current.size()-1));
            
            // append the previous index found to encodedOutput
            //            char buf[80]; // c-string
            //            sprintf(buf, "%d,", indexFound); // convert int to c-string
            //            encodedOutput += buf;
            //            output << buf;
            encodedOutput.push_back(indexFound);
            output.write((char *)&indexFound, 2);
            
            dictionary.push_back(current);
            
            //cout << "encode(): Encoded Output: ";
            //printPattern(encodedOutput);
            //cout << endl;
            //cout << "encode(): New Dictionary Entry/Index: ";
            //printPattern(current);
            //icout << " / " << dictionary.size() << endl;
            
            // remove prev pattern from the current string
            //            current = current.substr(current.size()-1);
            current = subPattern(current, current.size()-1, 1);
        }
    }
    cout << "encode(): end" << endl;
}

void decode(ifstream &inputEncoded, ofstream &outputDecoded, int compressedDataSize) {
    int curBytesRead = 0;
    
    cout << "decode(): started" << endl;
    //--------------------------------------------------------------------------
    // initialize dict
    //--------------------------------------------------------------------------
    vector<vector<uint8_t>> initialDictionary;
    //--------------------------------------------------------------------------
    // the type of elements of dictionary = vector<uint8_t> = one or more bytes = the type of the pattern
    //--------------------------------------------------------------------------
    // 00
    // 01
    // 02
    // ...
    // FF = 255
    // -------------------------------------- initial dictionary
    // 00 FF 02
    
    for (int i = 0; i < 256; i++) { // create initial dictionary
        vector<uint8_t> curPattern;
        curPattern.push_back(i);
        initialDictionary.push_back(curPattern);
    }
    
    vector<vector<uint8_t>> dictionary = initialDictionary;
    
    vector<vector<uint8_t>> decodedOutput;
    
    vector<uint8_t> entry;
    
    uint8_t lastByte;
    
    uint16_t prevCode = 65535;
    uint16_t currCode;
    
    while (true) {
        if (curBytesRead >= compressedDataSize) {
            break;
        }
        
        inputEncoded.read((char *)&currCode, 2); // read 2 bytes from file, because our index is short(uint16_t).
        if (inputEncoded.eof()) {
            break;
        }
        
        curBytesRead += 2;
        
        if (dictionary.size() > currCode) {
            entry = dictionary[currCode]; // translation of currcode from dictionary
        }
        else {
            // Source: https://www2.cs.duke.edu/csed/curious/compression/lzw.html
            // There is an exception where the algorithm fails, and that is when the code calls
            // for an index which has not yet been entered (eg. calling for an index 31 when
            // index 31 is currently being processed and therefore not in the dictionary yet).
            // An example from Sayood will help illustrate this point.
            // Suppose you had the string abababab..... and an initial dictionary of just
            // a & b with indexes 0 & 1, respectively. The encoding process begins:
            
            // As you can see, the decoder comes across an index of 4 while the entry
            // that belongs there is currently being processed. To understand why this happens,
            // take a look at the encoding table. Immediately after "aba" (with an index of 4)
            // is entered into the dictionary, the next substring that is encoded is an "aba"
            // (ie. the very next code written to the encoded output file is a 4).
            // Thus, the only case in which this special case can occur is if the substring
            // begins and ends with the same character ("aba" is of the form <char><string><char>).
            // So, to deal with this exception, you simply take the substring you have so far,
            // "ab", and concatenate its first character to itself, "ab"+"a" = "aba", instead
            // of following the procedure as normal. Therefore the pseudocode provided above
            // must be altered a bit in order to handle all cases.
            entry.push_back(entry[0]);
        }
        
        // output entry
        decodedOutput.push_back(entry);
        //cout << "decode(): entry = ";
        //printPattern(entry);
        //cout << endl;
        writePattern(entry, outputDecoded);
        
        lastByte = entry[0];
        
        if (prevCode != 65535) { // skip this in the first round
            vector<uint8_t> curPattern = dictionary[prevCode];
            curPattern.push_back(entry[0]);
            dictionary.push_back(curPattern);
        }
        
        prevCode = currCode;
    }
    cout << "decode(): end" << endl;
}

//int main(int argc, const char * argv[]) {
//    //--------------------------------------------------------------------------
//    // initialize dict
//    //--------------------------------------------------------------------------
//    vector<vector<uint8_t>> initialDictionary;
//    //--------------------------------------------------------------------------
//    // the type of elements of dictionary = vector<uint8_t> = one or more bytes = the type of the pattern
//    //--------------------------------------------------------------------------
//    // 00
//    // 01
//    // 02
//    // ...
//    // FF = 255
//    // -------------------------------------- initial dictionary
//    // 00 FF 02
//    
//    for (int i = 0; i < 256; i++) { // create initial dictionary
//        vector<uint8_t> curPattern;
//        curPattern.push_back(i);
//        initialDictionary.push_back(curPattern);
//    }
//    
//    //    printDictionary(initialDictionary);
//    
//    //--------------------------------------------------------------------------
//    // LZW encoding
//    //--------------------------------------------------------------------------
//    // open input file for encoding
//    ifstream inputToEncode;
//    inputToEncode.open ("input.bin");
//    
//    if (inputToEncode.fail()) {
//        cout << "main()::encoding: error - failed to open : input.txt" << endl;
//        exit(1);
//    }
//    
//    // create output file for encoding
//    string encodedOutputFilename = "output.lzw";
//    ofstream outputEncoded;
//    outputEncoded.open(encodedOutputFilename.c_str());
//    if (outputEncoded.fail()) {
//        cout << "main()::encoding: error - failed to open : " << encodedOutputFilename << endl;
//        exit(1);
//    }
//    
//    encode(initialDictionary, inputToEncode, outputEncoded);
//    inputToEncode.close();
//    outputEncoded.close();
//    
//    //--------------------------------------------------------------------------
//    // Decoding
//    //--------------------------------------------------------------------------
//    // open (encoded) input file for decoding
//    ifstream inputToDecode;
//    inputToDecode.open(encodedOutputFilename.c_str());
//    //string inputToDecode = "1,0,3,6,0,4,5,3,2,8";
//    if (inputToDecode.fail()) {
//        cout << "main()::decoding: error - failed to open : " << encodedOutputFilename << endl;
//        exit(1);
//    }
//    
//    // create output file for decoding
//    string decodedOutputFilename = "output_decoded.bin";
//    ofstream outputDecoded;
//    outputDecoded.open(decodedOutputFilename.c_str());
//    if (outputDecoded.fail()) {
//        cout << "main()::decoding: error - failed to open : " << decodedOutputFilename << endl;
//        exit(1);
//    }
//    
//    decode(initialDictionary, inputToDecode, outputDecoded);
//    inputToDecode.close();
//    outputDecoded.close();
//    
//    return 0;
//}
