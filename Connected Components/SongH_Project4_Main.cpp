#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <climits>

using namespace std;

struct Property {
  int label;
  int numPixels;
  int minR;
  int minC;
  int maxR;
  int maxC;
};

class ccLabel {
  private:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int newLabel;
    int trueNumCC;
    int newMin;
    int newMax;
    int nonZeroNeighborAry[5]{};
    int** zeroFramedAry;
    int* EQAry;
    char option;
    Property* CCproperty;

    int getMinValue(vector<int> vector){
      int minValue = INT_MAX;
      for(int val : vector){
        if(val < minValue){
          minValue = val;
        }
      }
      return minValue;
    }

    void connectPass1(bool is4){
      int neighborRowSize = 2;
      vector<vector<int>> neighborhood;
      if(is4){
        neighborhood = {
          {-1, 0},
          {0, -1}
        };
      } else {
        neighborRowSize = 4;
        neighborhood = {
          {-1, -1},
          {-1, 0},
          {-1, 1},
          {0, -1}
        };
      }

      for(int i = 1; i < numRows+1; i++){
        for(int j = 1; j < numCols+1; j++){
          if(zeroFramedAry[i][j] > 0){
            int nonZeroCount = 0;
            vector<int> nonZeroVector;
            for(int k = 0; k < neighborRowSize; k++){
              int newI = i + neighborhood[k][0];
              int newJ = j + neighborhood[k][1];
              if(zeroFramedAry[newI][newJ] != 0){
                nonZeroCount++;
                nonZeroVector.push_back(zeroFramedAry[newI][newJ]);
              }
            }
            // Case 0, when all neighbors are 0
            if(nonZeroCount == 0){
              newLabel++;
              zeroFramedAry[i][j] = newLabel;
            // Case 1, when only none neighbor is non-zero
            } else if(nonZeroCount == 1){
              zeroFramedAry[i][j] = nonZeroVector[nonZeroVector.size()-1];
            // Case 2, when multiple neighbors are non-zero
            } else {
              int minLabelValue = getMinValue(nonZeroVector);
              zeroFramedAry[i][j] = minLabelValue;
              for(int label : nonZeroVector){
                EQAry[label] = minLabelValue;
              }
            }
          }
        }
      }
    }

    void connectPass2(bool is4){
      int neighborRowSize = 3;
      vector<vector<int>> neighborhood;
      if(is4){
        neighborhood = {
          {1, 0},
          {0, 1},
          {0, 0}
        };
      } else {
        neighborRowSize = 5;
        neighborhood = {
          {0, 1},
          {1, 1},
          {1, 0},
          {1, -1},
          {0, 0}
        };
      }

      for(int i = numRows; i > 0; i--){
        for(int j = numCols; j > 0; j--){
          if(zeroFramedAry[i][j] > 0){
            int nonZeroCount = 0;
            vector<int> nonZeroVector;
            for(int k = 0; k < neighborRowSize; k++){
              int newI = i + neighborhood[k][0];
              int newJ = j + neighborhood[k][1];
              if(zeroFramedAry[newI][newJ] != 0){
                nonZeroCount++;
                nonZeroVector.push_back(zeroFramedAry[newI][newJ]);
              }
            }
            if(nonZeroCount != 0){
              int minLabelValue = getMinValue(nonZeroVector);
              if(zeroFramedAry[i][j] != minLabelValue){
                zeroFramedAry[i][j] = minLabelValue;
                for(int label : nonZeroVector){
                  EQAry[label] = minLabelValue;
                }
              }
            }
          }
        }
      }
    }

    int manageEQAry(){
      int readLabel = 0;
      int index = 1;
      while(index <= newLabel){
        if(index != EQAry[index]){
          EQAry[index] = EQAry[EQAry[index]];
        } else {
          readLabel++;
          EQAry[index] = readLabel;
        }
        index++;
      }
      return readLabel;
    }

    void connectPass3(ofstream& deBugFile){
      deBugFile << "Entering connectPass3 method" << endl;
      for(int i = 1; i <= trueNumCC; i++){
        CCproperty[i].label = i;
        CCproperty[i].numPixels = 0;
        CCproperty[i].minR = numRows;
        CCproperty[i].maxR = 0;
        CCproperty[i].minC = numCols;
        CCproperty[i].maxC = 0;
      }
      for(int i = 1; i < numRows+1; i++){
        for(int j = 1; j < numCols+1; j++){
          zeroFramedAry[i][j] = EQAry[zeroFramedAry[i][j]];
          int k = zeroFramedAry[i][j];
          CCproperty[k].numPixels++;
          if(i < CCproperty[k].minR){
            CCproperty[k].minR = i;
          }
          if(i > CCproperty[k].maxR){
            CCproperty[k].maxR = i;
          }
          if(j < CCproperty[k].minC){
            CCproperty[k].minC = j;
          }
          if(j > CCproperty[k].maxC){
            CCproperty[k].maxC = j;
          }
        }
      }
      deBugFile << "Leaving connectPass3 method" << endl;
    }

  public:
    ccLabel(int numRows, int numCols, int minVal, int maxVal, char option){
      this->numRows = numRows;
      this->numCols = numCols;
      this->minVal = minVal;
      this->maxVal = maxVal;
      this->option = option;
      this->newLabel = 0;
      this->trueNumCC = 0;
      this->newMin = 0;
      this->newMax = trueNumCC;

      this->zeroFramedAry = new int*[numRows+2];
      for(int i = 0; i < numRows+2; i++){
        zeroFramedAry[i] = new int[numCols+2];
      }
      int eqSize = (numRows * numCols)/4;
      this->EQAry = new int[eqSize];
      for(int i = 0; i < eqSize; i++){
        EQAry[i] = i;
      }
    }

    void zero2D(int** ary, int rowSize, int colSize){
      for(int i = 0; i < rowSize; i++){
        for(int j = 0; j < colSize; j++){
          ary[i][j] = 0;
        }
      }
    }

    void loadImage(ifstream& inFile, int** ary){
      int pixelVal = 0;
      int rowIndex = 0;
      int colIndex = 0;

      while(!inFile.eof()){
        inFile >> pixelVal;
        ary[rowIndex+1][colIndex+1] = pixelVal;
        colIndex++;
        colIndex %= numCols;
        if(colIndex == 0){
          rowIndex++;
          rowIndex %= numRows;
        }
      }
    }

    void conversion(int** ary){
      for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
          if(ary[i+1][j+1] == 0){
            ary[i+1][j+1] = 1;
          } else {
            ary[i+1][j+1] = 0;
          }
        }
      }
    }

    void connected(bool is4, ofstream& RFprettyPrintFile, ofstream& deBugFile){
      string methodName = "connected4";
      if(!is4){ methodName = "connected8"; }
      deBugFile << "Entering " << methodName << " method" << endl;
      connectPass1(is4);
      deBugFile << "After " << methodName << " pass1, newLabel=" << newLabel << endl;
      imgReformat(RFprettyPrintFile, zeroFramedAry, 1, 1, numRows+1, numCols+1, "Result of: Pass 1");
      printEQAry(RFprettyPrintFile, "Equivalency Table after: Pass 1 (indexing starts from 1)");
      connectPass2(is4);
      deBugFile << "After " << methodName << " pass2, newLabel=" << newLabel << endl;
      imgReformat(RFprettyPrintFile, zeroFramedAry, 1, 1, numRows+1, numCols+1, "Result of: Pass 2");
      printEQAry(RFprettyPrintFile, "Equivalency Table after: Pass 2 (indexing starts from 1)");
      trueNumCC = manageEQAry();
      printEQAry(RFprettyPrintFile, "Equivalency Table after: EQ Table Management (indexing starts from 1)");
      newMin = 0;
      newMax = trueNumCC;
      CCproperty = new Property[trueNumCC+1];
      deBugFile << "In " << methodName << ", after manage EQAry, trueNumCC=" << trueNumCC << endl;
      connectPass3(deBugFile);
      imgReformat(RFprettyPrintFile, zeroFramedAry, 1, 1, numRows+1, numCols+1, "Result of: Pass 3");
      printEQAry(RFprettyPrintFile, "Equivalency Table after: Pass 3 (indexing starts from 1)");
      deBugFile << "Leaving " << methodName << "  method" << endl;
    }

    void imgReformat(ofstream &RFprettyPrintFile, int** ary, int rowStart, int colStart, int rowEnd, int colEnd, string caption){
      if(caption.size() > 0){
        RFprettyPrintFile << caption << "\n";
      }
      for(int i = rowStart; i < rowEnd; i++){
        for(int j = colStart; j < colEnd; j++){
          if(ary[i][j] == 0){
            RFprettyPrintFile << ".  ";
          } else {
            if(ary[i][j] > 9){
              RFprettyPrintFile << ary[i][j] << " ";
            } else {
              RFprettyPrintFile << ary[i][j] << "  ";
            }
          }
        }
        RFprettyPrintFile << endl;
      }
    }

    void printEQAry(ofstream &RFprettyPrintFile, string caption){
      if(caption.size() > 0){
        RFprettyPrintFile << caption << "\n";
      }
      for(int i = 1; i <= newLabel; i++){
        RFprettyPrintFile << i << " ";
      }
      RFprettyPrintFile << endl;
      for(int i = 1; i <= newLabel; i++){
        RFprettyPrintFile << EQAry[i] << " ";
      }
      RFprettyPrintFile << endl;
    }

    void printImg(ofstream& labelFile){
      labelFile << numRows << " " << numCols << " " << newMin << " " << newMax << endl;
      for(int i = 1; i < numRows+1; i++){
        for(int j = 1; j < numCols+1; j++){
          labelFile << zeroFramedAry[i][j] << "  ";
        }
        labelFile << endl;
      }
    }

    void printCCproperty(ofstream& propertyFile){
      propertyFile << numRows << " " << numCols << " " << newMin << " " << newMax << endl;
      propertyFile << trueNumCC << endl;

      for(int i = 1; i <= trueNumCC; i++){
        propertyFile << CCproperty[i].label << endl;
        propertyFile << CCproperty[i].numPixels << endl;
        propertyFile << CCproperty[i].minR-1 << " " << CCproperty[i].minC-1 << endl;
        propertyFile << CCproperty[i].maxR-1 << " " << CCproperty[i].maxC-1 << endl;
      }
    }

    void drawBoxes(ofstream& deBugFile){
      deBugFile << "Entering drawBoxes method" << endl;
      for(int i = 1; i <= trueNumCC; i++){
        int minR = CCproperty[i].minR;
        int minC = CCproperty[i].minC;
        int maxR = CCproperty[i].maxR;
        int maxC = CCproperty[i].maxC;
        int label = CCproperty[i].label;

        for(int i = minC; i <= maxC; i++){
          zeroFramedAry[minR][i] = label;
          zeroFramedAry[maxR][i] = label;
        }

        for(int i = minR; i<= maxR; i++){
          zeroFramedAry[i][minC] = label;
          zeroFramedAry[i][maxC] = label;
        }
      }
      deBugFile << "Leaving drawBoxes method" << endl;
    }

    int** getZeroFramedAry(){
      return zeroFramedAry;
    }

    int getTrueNumCC(){
      return trueNumCC;
    }
};

int main(int argc, const char* argv[]){
  ifstream inFile;
  ofstream RFprettyPrintFile, labelFile, propertyFile, deBugFile;
  int numRows, numCols, minVal, maxVal;
  int connectness = 4;
  char option = 'y';
  // Step 0, open necessary files and initialize
  inFile.open(argv[1]);
  connectness = stoi(argv[2]);
  option = argv[3][0];
  RFprettyPrintFile.open(argv[4]);
  labelFile.open(argv[5]);
  propertyFile.open(argv[6]);
  deBugFile.open(argv[7]);

  if(connectness != 4 && connectness != 8 && option != 'y' && option != 'n' && option != 'Y' && option != 'N'){
    cout << "Provided connectness or option is not expected value, program is terminated." << endl;
    exit(1);
  }

  inFile >> numRows;
  inFile >> numCols;
  inFile >> minVal;
  inFile >> maxVal;
  ccLabel cc = ccLabel(numRows, numCols, minVal, maxVal, option);
  int** zeroFramedAry = cc.getZeroFramedAry();
  cc.zero2D(zeroFramedAry, numRows+2, numCols+2);
  cc.loadImage(inFile, zeroFramedAry);
  if(option == 'y' || option == 'Y'){
    cc.conversion(zeroFramedAry);
  }
  if(connectness == 4){
    cc.connected(true, RFprettyPrintFile, deBugFile);
  }
  if(connectness == 8){
    cc.connected(false, RFprettyPrintFile, deBugFile);
  }

  cc.printImg(labelFile);
  cc.printCCproperty(propertyFile);
  cc.drawBoxes(deBugFile);
  cc.imgReformat(RFprettyPrintFile, zeroFramedAry, 1, 1, numRows+1, numCols+1, "Result of: DrawBoxes");
  RFprettyPrintFile << "Total number of connected component: " << cc.getTrueNumCC() << endl;
  

  return 0;
}