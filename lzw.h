#ifndef LZW_H
#define LZW_H

// encode binary file
void encode(ifstream &input, fstream &output);
void decode(ifstream &inputEncoded, ofstream &outputDecoded, int compressedDataSize = 0);

#endif
