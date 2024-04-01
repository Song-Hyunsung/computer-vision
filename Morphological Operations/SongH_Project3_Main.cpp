#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Morphology {
  private:
    int numImgRows;
    int numImgCols;
    int imgMin;
    int imgMax;
    int numStructRows;
    int numStructCols;
    int structMin;
    int structMax;
    int rowOrigin;
    int colOrigin;
    int rowFrameSize;
    int colFrameSize;
    int extraRows;
    int extraCols;
    int rowSize;
    int colSize;
    int** zeroFramedAry;
    int** morphAry;
    int** tempAry;
    int** structAry;

    int** allocate2DAry(int rowSize, int colSize){
      int** newArr = new int*[rowSize];
      for(int i = 0; i < rowSize; i++){
        newArr[i] = new int[colSize];
      }
      return newArr;
    }

    void printImgHeader(ofstream &outFile, int** ary){
      if(ary == structAry){
        outFile << numStructRows << " " << numStructCols << " " << structMin << " " << structMax << "\n";
      } else if(ary == zeroFramedAry) {
        outFile << numImgRows << " " << numImgCols << " " << imgMin << " " << imgMax << "\n";
      } else if(ary == morphAry) {
        outFile << rowSize << " " << colSize << " " << imgMin << " " << imgMax << "\n";
      }
    }

    void onePixelDilation(int rowIndex, int colIndex, int** outAry){
      int structRowOffset = rowIndex - rowOrigin;
      int structColOffset = colIndex - colOrigin;
      
      for(int rIndex = 0; rIndex < numStructRows; rIndex++){
        for(int cIndex = 0; cIndex < numStructCols; cIndex++){
          if(structAry[rIndex][cIndex] > 0){
            outAry[structRowOffset + rIndex][structColOffset + cIndex] = 1;
          }
        }
      }
    }

    void onePixelErosion(int rowIndex, int colIndex, int** inAry, int** outAry){
      int structRowOffset = rowIndex - rowOrigin;
      int structColOffset = colIndex - colOrigin;
      bool matchFlag = true;

      for(int rIndex = 0; rIndex < numStructRows && matchFlag; rIndex++){
        for(int cIndex = 0; cIndex < numStructCols && matchFlag; cIndex++){
          if(structAry[rIndex][cIndex] > 0 && inAry[structRowOffset + rIndex][structColOffset + cIndex] <= 0){
            matchFlag = false;
          }
        }
      }

      if(matchFlag){
        outAry[rowIndex][colIndex] = 1;
      } else {
        outAry[rowIndex][colIndex] = 0;
      }
    }

  public:
    Morphology(int numImgRows, int numImgCols, int imgMin, int imgMax, int numStructRows, int numStructCols, int structMin, int structMax, int rowOrigin, int colOrigin){
      this->numImgRows = numImgRows;
      this->numImgCols = numImgCols;
      this->imgMin = imgMin;
      this->imgMax = imgMax;
      this->numStructRows = numStructRows;
      this->numStructCols = numStructCols;
      this->structMin = structMin;
      this->structMax = structMax;
      this->rowOrigin = rowOrigin;
      this->colOrigin = colOrigin;
      this->rowFrameSize = numStructRows/2;
      this->colFrameSize = numStructCols/2;
      this->extraRows = rowFrameSize*2;
      this->extraCols = colFrameSize*2;
      this->rowSize = numImgRows+extraRows;
      this->colSize = numImgCols+extraCols;
      this->zeroFramedAry = allocate2DAry(rowSize, colSize);
      this->morphAry = allocate2DAry(rowSize, colSize);
      this->tempAry = allocate2DAry(rowSize, colSize);
      this->structAry = allocate2DAry(numStructRows, numStructCols);
    }

    void zero2DAry(int** ary, int rowSize, int colSize){
      for(int i = 0; i < rowSize; i++){
        for(int j = 0; j < colSize; j++){
          ary[i][j] = 0;
        }
      }
    }

    void loadImg(ifstream &imgFile, int** ary){
      int temp;
      for(int i = rowOrigin; i < numImgRows+rowOrigin; i++){
        for(int j = colOrigin; j < numImgCols+colOrigin; j++){
          imgFile >> temp;
          ary[i][j] = temp;
        }
      }
    }

    void loadStruct(ifstream &imgFile, int** ary){
      int temp;
      for(int i = 0; i < numStructRows; i++){
        for(int j = 0; j < numStructCols; j++){
          imgFile >> temp;
          ary[i][j] = temp;
        }
      }
    }

    void prettyPrint(ofstream &outFile, int** ary, int rowStart, int colStart, int rowEnd, int colEnd, string caption){
      if(caption.size() > 0){
        outFile << caption << "\n";
      }
      printImgHeader(outFile, ary);
      for(int i = rowStart; i < rowEnd; i++){
        for(int j = colStart; j < colEnd; j++){
          if(ary[i][j] == 0){
            outFile << ". ";
          } else {
            outFile << "1 ";
          }
        }
        outFile << endl;
      }
    }

    void aryToFile(ofstream &outFile, int** ary, int rowStart, int colStart, int rowEnd, int colEnd){
      printImgHeader(outFile, ary);
      for(int i = rowStart; i < rowEnd; i++){
        for(int j = colStart; j < colEnd; j++){
          outFile << ary[i][j] << " ";
        }
        if(i+1 < rowEnd){
          outFile << endl;
        }
      }
    }

    void computeDilation(int** inAry, int** outAry){
      for(int i = rowFrameSize; i < rowSize; i++){
        for(int j = colFrameSize; j < colSize; j++){
          if(inAry[i][j] > 0){
            onePixelDilation(i, j, outAry);
          }
        }
      }
    }

    void computeErosion(int** inAry, int** outAry){
      for(int i = rowFrameSize; i < rowSize; i++){
        for(int j = colFrameSize; j < colSize; j++){
          if(inAry[i][j] > 0){
            onePixelErosion(i, j, inAry, outAry);
          }
        }
      }
    }

    void computeOpening(int** inAry, int** outAry, int** tempAry){
      computeErosion(inAry, tempAry);
      computeDilation(tempAry, outAry);
    }

    void computeClosing(int** inAry, int** outAry, int** tempAry){
      computeDilation(inAry, tempAry);
      computeErosion(tempAry, outAry);
    }

    // getters
    int** getZeroFramedAry(){
      return zeroFramedAry;
    }

    int** getStructAry(){
      return structAry;
    }

    int** getMorphAry(){
      return morphAry;
    }

    int** getTempAry(){
      return tempAry;
    }

    int getRowSize(){
      return rowSize;
    }

    int getColSize(){
      return colSize;
    }
};

int main(int argc, const char* argv[]){
  ifstream imgFile, structFile;
  ofstream dilateOutFile, erodeOutFile, openingOutFile, closingOutFile, prettyPrintFile;
  int numImgRows, numImgCols, imgMin, imgMax, numStructRows, numStructCols, structMin, structMax, rowOrigin, colOrigin;
  // Step 0, open necessary files
  imgFile.open(argv[1]);
  structFile.open(argv[2]);
  dilateOutFile.open(argv[3]);
  erodeOutFile.open(argv[4]);
  closingOutFile.open(argv[5]);
  openingOutFile.open(argv[6]);
  prettyPrintFile.open(argv[7]);
  // Step 1, read from input file headers
  imgFile >> numImgRows;
  imgFile >> numImgCols;
  imgFile >> imgMin;
  imgFile >> imgMax;
  structFile >> numStructRows;
  structFile >> numStructCols;
  structFile >> structMin;
  structFile >> structMax;
  structFile >> rowOrigin;
  structFile >> colOrigin;
  // Step 2, initialize and allocate Morphology class member fields
  Morphology morphology = Morphology(numImgRows, numImgCols, imgMin, imgMax, numStructRows, numStructCols, structMin, structMax, rowOrigin, colOrigin);
  // Step 3, set zeroFramedAry to 0
  int** zeroFramedAry = morphology.getZeroFramedAry();
  int** structAry = morphology.getStructAry();
  int** morphAry = morphology.getMorphAry();
  int** tempAry = morphology.getTempAry();
  int rowSize = morphology.getRowSize();
  int colSize = morphology.getColSize();
  morphology.zero2DAry(zeroFramedAry, rowSize, colSize);
  // Step 4, load imgFile and print to prettyPrintFile
  morphology.loadImg(imgFile, zeroFramedAry);
  morphology.prettyPrint(prettyPrintFile, zeroFramedAry, rowOrigin, colOrigin, numImgRows+rowOrigin, numImgCols+colOrigin, "Original Image");
  // Step 5, initialize, load, and print structuring element
  morphology.zero2DAry(structAry, numStructRows, numStructCols);
  morphology.loadStruct(structFile, structAry);
  morphology.prettyPrint(prettyPrintFile, structAry, 0, 0, numStructRows, numStructCols, "Structuring Image");
  // Step 6, perform dilation
  morphology.zero2DAry(morphAry, rowSize, colSize);
  morphology.computeDilation(zeroFramedAry, morphAry);
  morphology.aryToFile(dilateOutFile, morphAry, 0, 0, rowSize, colSize);
  morphology.prettyPrint(prettyPrintFile, morphAry, 0, 0, rowSize, colSize, "Dilation");
  // Step 7, perform erosion
  morphology.zero2DAry(morphAry, rowSize, colSize);
  morphology.computeErosion(zeroFramedAry, morphAry);
  morphology.aryToFile(erodeOutFile, morphAry, 0, 0, rowSize, colSize);
  morphology.prettyPrint(prettyPrintFile, morphAry, 0, 0, rowSize, colSize, "Erosion");
  // Step 8, perform opening
  morphology.zero2DAry(morphAry, rowSize, colSize);
  morphology.computeOpening(zeroFramedAry, morphAry, tempAry);
  morphology.aryToFile(openingOutFile, morphAry, 0, 0, rowSize, colSize);
  morphology.prettyPrint(prettyPrintFile, morphAry, 0, 0, rowSize, colSize, "Opening");
  // Step 9, perform closing
  morphology.zero2DAry(morphAry, rowSize, colSize);
  morphology.zero2DAry(tempAry, rowSize, colSize);
  morphology.computeClosing(zeroFramedAry, morphAry, tempAry);
  morphology.aryToFile(closingOutFile, morphAry, 0, 0, rowSize, colSize);
  morphology.prettyPrint(prettyPrintFile, morphAry, 0, 0, rowSize, colSize, "Closing");
  // Step 10, close all files
  // destructor in ifstream and ofstream takes care of closing files
  // there is no need to manually close them in this application
  return 0;
}