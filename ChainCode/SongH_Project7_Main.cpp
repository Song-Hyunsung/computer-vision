#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct CCproperty {
  int label, numPixels, minRow, minCol, maxRow, maxCol;
};

class ChainCode {
  private:
    struct point {
      int row, col;
    };

    int numRows, numCols, minVal, maxVal, numCC, lastQ, nextDir, pChainDir;
    int** imgAry;
    int** boundaryAry;
    int** CCAry;
    CCproperty CC;
    point startP, currentP, nextP;
    point coordOffset[8] = {{0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}};
    int zeroTable[8] = {6,0,0,2,2,4,4,6};

    int** allocate2DAry(int rowSize, int colSize){
      int** newArr = new int*[rowSize];
      for(int i = 0; i < rowSize; i++){
        newArr[i] = new int[colSize];
      }
      for(int i = 0; i < rowSize; i++){
        for(int j = 0; j < colSize; j++){
          newArr[i][j] = 0;
        }
      }
      return newArr;
    }

    bool pointEquals(point a, point b){
      return (a.row == b.row) && (a.col == b.col);
    }

  public:
    ChainCode(int numRows, int numCols, int minVal, int maxVal, int numCC){
      this->numRows = numRows;
      this->numCols = numCols;
      this->minVal = minVal;
      this->maxVal = maxVal;
      this->numCC = numCC;
      this->imgAry = allocate2DAry(numRows+2, numCols+2);
      this->boundaryAry = allocate2DAry(numRows+2, numCols+2);
      this->CCAry = allocate2DAry(numRows+2, numCols+2);
      this->lastQ = 4; // 4 since we are scanning L->R, T->B
    }

    void getChainCode(CCproperty CC, ofstream& chainCodeFile, ofstream& deBugFile){
      deBugFile << "Entering getChainCode method" << endl;
      chainCodeFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
      
      int label = CC.label;

      for(int i = 1; i < numRows+1; i++){
        bool startPFound = false;
        for(int j = 1; j < numCols+1; j++){
          if(CCAry[i][j] == label){
            chainCodeFile << label << " " << i-1 << " " << j-1 << endl;
            startP.row = i;
            startP.col = j;
            currentP.row = i;
            currentP.col = j;
            lastQ = 4;
            startPFound = true;
            break;
          }
        }
        if(startPFound){
          break;
        }
      }

      do {
        int nextQ = (lastQ+1) % 8;
        pChainDir = findNextP(currentP, label, deBugFile);
        chainCodeFile << pChainDir << " ";
        nextP.row = currentP.row + coordOffset[pChainDir].row;
        nextP.col = currentP.col + coordOffset[pChainDir].col;
        currentP = nextP;

        if(pChainDir == 0){
          lastQ = zeroTable[7];
        } else {
          lastQ = zeroTable[pChainDir-1];
        }

        deBugFile << "lastQ=" << lastQ << "; nextQ=" << nextQ << "; currentP.row=" << currentP.row-1 << "; currentP.col=" << currentP.col-1 << "; nextP.row=" << nextP.row-1 << "; nextP.col=" << nextP.col-1 << endl;
      } while(!pointEquals(currentP, startP));

      chainCodeFile << endl;
      deBugFile << "Leaving getChainCode" << endl;
    }

    int findNextP(point currentP, int label, ofstream& deBugFile){
      deBugFile << "Entering findNextP method" << endl;

      int index = lastQ;
      bool found = false;
      int chainDir = 0;

      while(!found){
        int iRow = currentP.row + coordOffset[index].row;
        int jCol = currentP.col + coordOffset[index].col;

        if(imgAry[iRow][jCol] == label){
          chainDir = index;
          found = true;
        } else {
          index = (index+1) % 8;
        }
      }

      deBugFile << "Leaving findNextP method" << endl;
      deBugFile << "chainDir=" << chainDir << endl;

      return chainDir;
    }

    void constructBoundary(ifstream& chainCodeFile){
      string line;
      // skip the first two line, which are header (4 value) and numCC (1 value)
      for(int i = 0; i < 5; i++){
        chainCodeFile >> line;
      }

      for(int i = 0; i < numCC; i++){
        // skip the header for each component since we don't need
        for(int j = 0; j < 4; j++){
          chainCodeFile >> line;
        }
        int label, currentPRow, currentPCol;
        chainCodeFile >> label;
        chainCodeFile >> currentPRow;
        chainCodeFile >> currentPCol;
        // increment coordinate from file because it excludes zeroFrame
        currentPRow++;
        currentPCol++;
        chainCodeFile.get(); // skip over file pointer to the next line, where chain code exists
        getline(chainCodeFile, line);
        
        boundaryAry[currentPRow][currentPCol] = label;
        
        for(char c : line){
          int iRow = currentPRow + coordOffset[c-'0'].row;
          int jCol = currentPCol + coordOffset[c-'0'].col;
          boundaryAry[iRow][jCol] = label;
          currentPRow = iRow;
          currentPCol = jCol;
        }
      }
    }

    void zeroFrameImgAry(){
      for(int i = 0; i < numRows+2; i++){
        imgAry[i][0] = 0;
        imgAry[i][numCols+1] = 0;
      }

      for(int j = 0; j < numCols+2; j++){
        imgAry[0][j] = 0;
        imgAry[numRows+1][j] = 0;
      }
    }

    void clearCCAry(){
      for(int i = 0; i < numRows+2; i++){
        for(int j = 0; j < numCols+2; j++){
          CCAry[i][j] = 0;
        }
      }
    }

    void loadCCAry(int label){
      for(int i = 1; i < numRows+1; i++){
        for(int j = 1; j < numCols+1; j++){
          if(imgAry[i][j] == label){
            CCAry[i][j] = label;
          }
        }
      }
    }

    void loadImg(ifstream &imgFile){
      int temp;
      for(int i = 1; i < numRows+1; i++){
        for(int j = 1; j < numCols+1; j++){
          imgFile >> temp;
          imgAry[i][j] = temp;
        }
      }
    }

    void reformatPrettyPrint(ofstream &prettyPrintFile, int** ary, int rowStart, int colStart, int rowEnd, int colEnd, string caption){
      if(caption.size() > 0){
        prettyPrintFile << caption << endl;
      }

      prettyPrintFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;

      for(int i = rowStart; i < rowEnd; i++){
        for(int j = colStart; j < colEnd; j++){
          if(ary[i][j] == 0){
            prettyPrintFile << ".  ";
          } else {
            if(ary[i][j] > 9){
              prettyPrintFile << ary[i][j] << " ";
            } else {
              prettyPrintFile << ary[i][j] << "  ";
            }
          }
        }
        prettyPrintFile << endl;
      }
    }

    void imgReformat(ofstream &imgOutputFile, int** ary, int rowStart, int colStart, int rowEnd, int colEnd){
      imgOutputFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;

      for(int i = rowStart; i < rowEnd; i++){
        for(int j = colStart; j < colEnd; j++){
          if(ary[i][j] == 0){
            imgOutputFile << "0 ";
          } else {
            imgOutputFile << ary[i][j] << " ";
          }
        }
        imgOutputFile << endl;
      }
    }

    int** getImgAry(){
      return this->imgAry;
    }

    int** getCCAry(){
      return this->CCAry;
    }

    int** getBoundaryAry(){
      return this->boundaryAry;
    }
};

int main(int argc, const char* argv[]){
  ifstream labelFile, propFile;
  ofstream outFile1, deBugFile, chainCodeFile, boundaryFile;
  int numRows, numCols, minVal, maxVal, numCC;
  // Step 0, open necessary files and initialize
  labelFile.open(argv[1]);
  propFile.open(argv[2]);
  outFile1.open(argv[3]);
  deBugFile.open(argv[4]);
  chainCodeFile.open(argv[5]);
  boundaryFile.open(argv[6]);

  // advance file pointers for label and prop file
  labelFile >> numRows;
  labelFile >> numCols;
  labelFile >> minVal;
  labelFile >> maxVal;

  propFile >> numRows;
  propFile >> numCols;
  propFile >> minVal;
  propFile >> maxVal;
  propFile >> numCC;

  ChainCode chainCode = ChainCode(numRows, numCols, minVal, maxVal, numCC);
  int** imgAry = chainCode.getImgAry();
  int** CCAry = chainCode.getCCAry();
  int** boundaryAry = chainCode.getBoundaryAry();
  chainCode.zeroFrameImgAry(); // not required since we initialize array with 0, but leaving here since spec. mentions it
  chainCode.loadImg(labelFile);
  chainCode.reformatPrettyPrint(outFile1, imgAry, 1, 1, numRows+1, numCols+1, "Original Input Image");

  // Step 1, set up chain code file
  chainCodeFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
  chainCodeFile << numCC << endl;

  // Step 2-5, process chain code for each connected components
  for(int i = 0; i < numCC; i++){
    CCproperty cc;
    propFile >> cc.label;
    propFile >> cc.numPixels;
    propFile >> cc.minRow;
    propFile >> cc.minCol;
    propFile >> cc.maxRow;
    propFile >> cc.maxCol;

    chainCode.clearCCAry();
    chainCode.loadCCAry(cc.label);
    chainCode.reformatPrettyPrint(deBugFile, CCAry, 1, 1, numRows+1, numCols+1, "CCAry with component label: " + to_string(cc.label));
    chainCode.getChainCode(cc, chainCodeFile, deBugFile);
  }

  // Step 7-8, close and reopen chainCodeFile
  chainCodeFile.close();
  ifstream chainCodeReopenFile;
  chainCodeReopenFile.open(argv[5]);

  // Step 9, construct and print boundary
  chainCode.constructBoundary(chainCodeReopenFile);
  chainCode.reformatPrettyPrint(outFile1, boundaryAry, 1, 1, numRows+1, numCols+1, "Boundary Image");
  chainCode.imgReformat(boundaryFile, boundaryAry, 1, 1, numRows+1, numCols+1);

  // Step 10, destructor takes care of file closing
  return 0;
}