#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

class ThresholdSelection {
  private:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int biGaussThrVal;
    int histHeight;
    int maxHeight;
    int* histAry;
    int* gaussAry;
    int* bestFitGaussAry;
    char** graph;
    const char HISTOGRAM_CHAR = '+';
    const char GAUSSIAN_CURVE_CHAR = '*';
    const char DELTA_CHAR = '#';

    void setZero(int* arr, int size){
      for(int i = 0; i < size; i++){
        arr[i] = 0;
      }
    }

    double fitGauss(int leftIndex, int rightIndex, ofstream &deBugFile){
      // Step 0, output debug print and initialize variables
      deBugFile << "Entering fitGauss method" << endl;
      double mean, var, sum, Gval;
      sum = 0.0;
      // Step 1, compute mean and variance
      mean = computeMean(leftIndex, rightIndex, deBugFile);
      var = computeVar(leftIndex, rightIndex, mean, deBugFile);
      // Step 2, set index iterator variable
      int index = leftIndex;
      while(index <= rightIndex){
        // Step 3, calculate Gauss value
        Gval = modifiedGauss(index, mean, var);
        // Step 4, compute sum
        sum += abs(Gval - (double) histAry[index]);
        // Step 5, update guassAry
        gaussAry[index] = (int) Gval;
        // Step 6, increment index
        index++;
      }
      // Step 8, print debug statement
      deBugFile << "Leaving fitGauss method, sum is: " << sum << endl;
      // Step 9, return sum
      return sum;
    }

    double computeMean(int leftIndex, int rightIndex, ofstream &deBugFile){
      // Step 0, output debug print and initialize variables
      deBugFile << "Entering computeMean method" << endl;
      maxHeight = 0;
      int sum = 0;
      int numPixels = 0;
      // Step 1, initialize index iterater variable
      int index = leftIndex;
      // Step 2-5, loop to compute mean
      while(index < rightIndex){
        sum += (histAry[index] * index);
        numPixels += histAry[index];
        if(histAry[index] > maxHeight){
          maxHeight = histAry[index];
        }
        index++;
      }
      // Step 6, store result
      double result = ((double) sum) / ((double) numPixels);
      // Step 7, output debug statement
      deBugFile << "Leaving computeMean method " << maxHeight << " is an result " << result << endl;
      // Step 8, return
      return result;
    }

    double computeVar(int leftIndex, int rightIndex, double mean, ofstream &deBugFile){
      // Step 0, output to debug file and initialize variable
      deBugFile << "Entering computeVar() method" << endl;
      double sum = 0.0;
      int numPixels = 0;
      // Step 1, initialize index iterator variable
      int index = leftIndex;
      // Step 2-4, computeVar in a loop
      while(index < rightIndex){
        sum += ((double) histAry[index]) * pow((((double) index) - mean), 2);
        numPixels += histAry[index];
        index++;
      }
      // Step 5, compute result
      double result = sum / ((double) numPixels);
      // Step 6, print debug statement
      deBugFile << "Leaving computeVar method returning " << result << endl;
      // Step 7, return result
      return result;
    }

    double modifiedGauss(int x, double mean, double var){
      return ((double) maxHeight) * exp((-1) * (pow((((double) x) - mean), 2) / (2 * var)));
    }

    void copyArys(int* arr1, int* arr2, int size){
      for(int i = 0; i < size; i++){
        arr2[i] = arr1[i];
      }
    }

  public:
    ThresholdSelection(int numRows, int numCols, int minVal, int maxVal){
      this->numRows = numRows;
      this->numCols = numCols;
      this->minVal = minVal;
      this->maxVal = maxVal;
      this->histHeight = 0;
      this->maxHeight = 0;
    }

    int loadHist(ifstream &inFile){
      // initialize arrays
      this->histAry = new int[maxVal+1]();
      this->gaussAry = new int[maxVal+1]();
      this->bestFitGaussAry = new int[maxVal+1]();
      // record histHeight and declare graph using histHeight
      int histIndex, histValue, largestHistValue = 0;
      while(!inFile.eof()){
        inFile >> histIndex;
        inFile >> histValue;
        if(histIndex < maxVal+1){
          histAry[histIndex] = histValue;
          largestHistValue = max(histValue, largestHistValue);
        }
      }
      this->histHeight = largestHistValue;
      this->graph = new char*[maxVal+1];
      for(int i = 0; i < maxVal+1; i++){
        graph[i] = new char[histHeight+1];
      }
      // initialize graph with blanks
      for(int i = 0; i < maxVal+1; i++){
        for(int j = 0; j < histHeight+1; j++){
          graph[i][j] = ' ';
        }
      }

      return this->histHeight;
    }

    void dispHist(ofstream &outFile1){
      // output header values for outFile1
      outFile1 << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
      // output 2D histogram along with caption
      for(int i = minVal; i < maxVal+1; i++){
        outFile1 << i << " (" << histAry[i] << "):" << string(histAry[i], HISTOGRAM_CHAR) << endl;
      }
    }

    void plotHist(){
      for(int i = minVal; i < maxVal+1; i++){
        for(int j = 0; j < histAry[i]; j++){
          graph[i][j] = HISTOGRAM_CHAR;
        }
      }
    }

    char** getGraph(){
      return graph;
    }

    int biGaussian(ofstream &deBugFile){
      // Step 0, output debug print and initialize variables
      deBugFile << "Entering biGaussian method" << endl;
      double sum1, sum2, total, minSumDiff;
      int offSet = (int) (maxVal - minVal) / 10;
      int dividePt = offSet;
      int bestThr = dividePt;
      minSumDiff = 99999.0;
      while(dividePt < (maxVal - offSet)){
        // Step 1, setZero to GaussAry
        setZero(gaussAry, maxVal+1);
        // Step 2-3 fit first and second Gaussian curve
        sum1 = fitGauss(minVal, dividePt, deBugFile);
        sum2 = fitGauss(dividePt, maxVal, deBugFile);
        // Step 4, compute total value
        total = sum1 + sum2;
        // Step 5, update bestThr and copyArys to bestFitGaussAry
        if(total < minSumDiff){
          minSumDiff = total;
          bestThr = dividePt;
          copyArys(gaussAry, bestFitGaussAry, maxVal+1);
        }
        // Step 6, print debug statement
        deBugFile << "In biGaussian (): dividePt=" << dividePt << ", sum1=" << sum1 << ", sum2=" << sum2 << ", total=" << total << ", minSumDiff=" << minSumDiff << " and bestThr=" << bestThr << endl;
        dividePt++;
      }
      deBugFile << "Leaving biGaussian method, minSumDiff= " << minSumDiff << ", bestThr is " << bestThr << endl;
      return bestThr;
    }

    void plotGaussCurves(ofstream &deBugFile){
      // Step 0, print debug statement
      deBugFile << "Entering plotGaussCurves() method" << endl;
      // Step 1, initialize iterator variable
      int index = 0;
      // Step 2-7 perform plotting logic
      while(index <= maxVal){
        int i, end1, end2;
        if(bestFitGaussAry[index] <= histAry[index]){
          end1 = bestFitGaussAry[index];
          end2 = histAry[index];
        } else {
          end1 = histAry[index];
          end2 = bestFitGaussAry[index];
        }
        i = end1;
        while(i <= end2){
          graph[index][i] = DELTA_CHAR;
          i++;
        }
        graph[index][bestFitGaussAry[index]] = GAUSSIAN_CURVE_CHAR;
        index++;
      }
      // Step 9, print out debug statement
      deBugFile << "Leaving plotGaussCurves()" << endl;
    }
};

int main(int argc, const char* argv[]){
  ifstream inFile;
  ofstream outFile1;
  ofstream outFile2;
  ofstream deBugFile;
  int numRows, numCols, minVal, maxVal, histHeight;

  // Step 0, open files
  inFile.open(argv[1]);
  outFile1.open(argv[2]);
  outFile2.open(argv[3]);
  deBugFile.open(argv[4]);
  if(!inFile.is_open() || !outFile1.is_open() || !outFile2.is_open() || !deBugFile.is_open()){
    cout << "Unable to open provided files" << endl;
    exit(1);
  }

  // Step 1, read from inFile1
  inFile >> numRows;
  inFile >> numCols;
  inFile >> minVal;
  inFile >> maxVal;

  // Step 2, call dispHist to output to outFile1
  ThresholdSelection thresholdSelection = ThresholdSelection(numRows, numCols, minVal, maxVal);
  histHeight = thresholdSelection.loadHist(inFile);
  thresholdSelection.dispHist(outFile1);
  thresholdSelection.plotHist();

  // Step 3, print graph (currently histogram without Gaussian curve) to deBugFile
  char** graph = thresholdSelection.getGraph();
  for(int i = minVal; i < maxVal+1; i++){
    for(int j = 0; j < histHeight+1; j++){
      deBugFile << graph[i][j];
    }
    deBugFile << endl;
  }

  // Step 4, perform biGaussian operation
  int biGaussThrVal = thresholdSelection.biGaussian(deBugFile);

  // Step 5, output biGaussThrVal to outFile2
  outFile2 << "The BiGaussThrVal is " << biGaussThrVal << endl;

  // Step 6, plotGaussCurve
  thresholdSelection.plotGaussCurves(deBugFile);

  // Step 7, output graph to outputFile2
  for(int i = minVal; i < maxVal+1; i++){
    for(int j = 0; j < histHeight+1; j++){
      outFile2 << graph[i][j];
    }
    outFile2 << endl;
  }

  // Step 8, ofstream and ifstream destructor will automatically close the file
  // hence no reason to manually close them in this application.
  
  return 0;
}