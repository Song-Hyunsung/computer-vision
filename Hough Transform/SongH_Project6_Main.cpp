#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

class HoughTransform {
  private:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int houghDist;
    int houghAngle;
    int** imgAry;
    int** cartesianAry;
    int** polarAry;
    int angleInDegree;
    double angleInRadians;
    int offset;

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

    void computeSinusoid(int x, int y, ofstream& deBugFile){
      deBugFile << "Entering computeSinusoid" << endl;

      for(int angleInDegree = 0; angleInDegree < 180; angleInDegree++){
        angleInRadians = (double) angleInDegree * (M_PI / 180.0);
        double dist = cartesianDist(x, y, angleInRadians);
        int distInt = (int) dist;
        deBugFile << "Incrementing cartesianAry: " << distInt << " " << angleInDegree << endl;
        cartesianAry[distInt][angleInDegree]++;
        dist = polarDist(x, y, angleInRadians);
        distInt = (int) dist;
        deBugFile << "Incrementing polarAry: " << distInt << " " << angleInDegree << endl;
        polarAry[distInt][angleInDegree]++;
      }

      deBugFile << "Leaving computeSinusoid" << endl;
    }

    double cartesianDist(int x, int y, double a){
      double t = a - atan(y/x) - (M_PI/2);
      return sqrt(pow(x,2) + pow(y,2)) * cos(t) + offset;
    }

    double polarDist(int x, int y, double a){
      return (x * cos(a)) + (y * sin(a)) + offset;
    }

  public:
    HoughTransform(int numRows, int numCols, int minVal, int maxVal){
      this->numRows = numRows;
      this->numCols = numCols;
      this->minVal = minVal;
      this->maxVal = maxVal;
      this->houghAngle = 180;
      this->offset = sqrt(pow(numRows,2) + pow(numCols,2));
      this->houghDist = 2 * offset;
      this->angleInDegree = 0;
      this->angleInRadians = 0.0;
      this->imgAry = allocate2DAry(numRows, numCols);
      this->cartesianAry = allocate2DAry(houghDist, houghAngle);
      this->polarAry = allocate2DAry(houghDist, houghAngle);
    }

    void buildHoughSpace(ofstream &outFile1, ofstream& deBugFile){
      deBugFile << "Entering buildHoughSpace" << endl;

      for(int x = 0; x < numRows; x++){
        for(int y = 0; y < numCols; y++){
          if(imgAry[x][y] > 0){
            computeSinusoid(x, y, deBugFile);
          }
        }
      }

      deBugFile << "Leaving buildHoughSpace" << endl;
    }

    void loadImg(ifstream &imgFile){
      int temp;
      for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
          imgFile >> temp;
          imgAry[i][j] = temp;
        }
      }
    }

    void reformatPrettyPrint(ofstream &prettyPrintFile, int** ary, int rowStart, int colStart, int rowEnd, int colEnd, string caption){
      if(caption.size() > 0){
        prettyPrintFile << caption << endl;
      }

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

    int** getImgAry(){
      return this->imgAry;
    }

    int** getCartesianAry(){
      return this->cartesianAry;
    }

    int** getPolarAry(){
      return this->polarAry;
    }

    int getHoughDist(){
      return this->houghDist;
    }

    int getHoughAngle(){
      return this->houghAngle;
    }
};

int main(int argc, const char* argv[]){
  ifstream inFile;
  ofstream outFile1, deBugFile;
  int numRows, numCols, minVal, maxVal;
  // Step 0, open necessary files and initialize
  inFile.open(argv[1]);
  outFile1.open(argv[2]);
  deBugFile.open(argv[3]);
  inFile >> numRows;
  inFile >> numCols;
  inFile >> minVal;
  inFile >> maxVal;
  HoughTransform houghTransform = HoughTransform(numRows, numCols, minVal, maxVal);

  // Step 1, load image and prettyPrint to outFile1
  int** imgAry = houghTransform.getImgAry();
  int** cartesianAry = houghTransform.getCartesianAry();
  int** polarAry = houghTransform.getPolarAry();
  int houghDist = houghTransform.getHoughDist();
  int houghAngle = houghTransform.getHoughAngle();

  houghTransform.loadImg(inFile);
  houghTransform.reformatPrettyPrint(outFile1, imgAry, 0, 0, numRows, numCols, "");

  // Step 2, build Hough Space
  houghTransform.buildHoughSpace(outFile1, deBugFile);

  // Step 3, print cartesian and polar to outFile
  houghTransform.reformatPrettyPrint(outFile1, cartesianAry, 0, 0, houghDist, houghAngle, "Cartesian Hough Space");
  houghTransform.reformatPrettyPrint(outFile1, polarAry, 0, 0, houghDist, houghAngle, "Polar Hough Space");

  // Step 4, destructor takes care of file closing
  return 0;
}