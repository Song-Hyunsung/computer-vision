#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define SongH_Project0_Main main

class Image {
  private:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int thrValue;
  public:
    Image(int numRows, int numCols, int minVal, int maxVal, int thrValue){
      this->numRows = numRows;
      this->numCols = numCols;
      this->minVal = minVal;
      this->maxVal = maxVal;
      this->thrValue = thrValue;
    }
    void Processing(ifstream &inFile, ofstream &outFile1, ofstream &outFile2, int thrValue){
      int pixelVal;
      int count = 0;
      string line;

      while(!inFile.eof()){
        inFile >> pixelVal;

        if(pixelVal >= thrValue){
          outFile1 << "1 ";
          outFile2 << pixelVal << " ";
        } else {
          outFile1 << "0 ";
          outFile2 << "0  ";
        }

        count++;

        if(count >= numCols){
          outFile1 << endl;
          outFile2 << endl;
          count = 0;
        }
      }
    }
};

int SongH_Project0_Main(int argc, const char* argv[]){
  ifstream inFile;
  ofstream outFile1;
  ofstream outFile2;

  int numRows, numCols, minVal, maxVal;

  inFile.open(argv[1]);
  int thrValue = stoi(argv[2]);
  outFile1.open(argv[3]);
  outFile2.open(argv[4]);

  if(!inFile.is_open() || !outFile1.is_open() || !outFile2.is_open()){
    cout << "Unable to open provided input files" << endl;
    exit(1);
  }

  inFile >> numRows;
  inFile >> numCols;
  inFile >> minVal;
  inFile >> maxVal;

  outFile1 << numRows << " " << numCols << " " << "0 " << "1" << endl;
  outFile2 << numRows << " " << numCols << " " << "0 " << maxVal << endl;

  Image inputImage = Image(numRows, numCols, minVal, maxVal, thrValue);
  inputImage.Processing(inFile, outFile1, outFile2, thrValue);

  return 0;
}