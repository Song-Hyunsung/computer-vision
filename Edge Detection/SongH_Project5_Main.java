import java.io.*;

class Edge {
  private int numRows;
  private int numCols;
  private int minVal;
  private int maxVal;
  private int[][] MFAry;
  private int[][] edgeAry;
  private int[][] zeroCrossAry;
  private static final int[][] ZERO_CROSSING_MASK = {
    { 0, -2, 0 },
    { -2, 8, -2 },
    { 0, -2, 0 }
  };
  private static final int[][] NEIGHBORHOOD_OFFSET = {
    { -1, -1 }, { -1, 0 }, { -1, 1 },
    { 0, -1 }, { 0, 1 },
    { 1, -1 }, { 1, 0 }, { 1, 1 }
  };

  Edge(int numRows, int numCols, int minVal, int maxVal){
    this.numRows = numRows;
    this.numCols = numCols;
    this.minVal = minVal;
    this.maxVal = maxVal;
    this.MFAry = new int[numRows+2][numCols+2];
    this.edgeAry = new int[numRows+2][numCols+2];
    this.zeroCrossAry = new int[numRows+2][numCols+2];
  }

  public void loadImage(BufferedReader inFile, int[][] ary) throws NumberFormatException, IOException{
    String line;
    int rowIndex = 1;

    while((line = inFile.readLine()) != null){
      String[] pixels = line.split(" ");
      int colIndex = 1;
      for(String pixel : pixels){
        ary[rowIndex][colIndex] = Integer.parseInt(pixel);
        colIndex++;
      }
      rowIndex++;
    }
  }

  public void imgReformatPrettyPrint(int[][] ary, BufferedWriter file) throws IOException {
    for(int i = 1; i < numRows+1; i++){
      for(int j = 1; j < numCols+1; j++){
        if(ary[i][j] == 0){
          file.write(".  ");
        } else {
          if(ary[i][j] > 9){
            file.write(ary[i][j] + " ");
          } else {
            file.write(ary[i][j] + "  ");
          }
        }
      }
      file.write("\n");
    }
  }

  public void imgReformat(int[][] ary, BufferedWriter file) throws IOException {
    printHeader(file);

    String str = Integer.toString(maxVal);
    int width = str.length();
    int i = 2;
    while(i < (numRows+2)){
      int j = 2;
        while(j < (numCols+2)){
          str = Integer.toString(ary[i][j]);
          file.write(str);
          int WW = str.length();
          while(WW <= width){
            file.write(" ");
              WW++;
            }
          j++;
        }
      file.write("\n");
      i++;
    }
  }

  public void zeroCrossingEdgeDetector(int[][] MFAry, int[][] edgeAry, int[][] zeroCrossAry, BufferedWriter zeroCrossFile, BufferedWriter binEdgeFile, BufferedWriter deBugFile) throws IOException{
    computeEdge(MFAry, edgeAry, deBugFile);
    crossZero(edgeAry, zeroCrossAry, deBugFile);
    zeroCrossFile.write(String.format("%d %d 0 1\n", numRows, numCols));
    imgReformatPrettyPrint(zeroCrossAry, zeroCrossFile);
    binEdgeFile.write(String.format("%d %d 0 1\n", numRows, numCols));
    binaryEdgeDetector(zeroCrossAry, binEdgeFile);
  }

  public void computeEdge(int[][] MFAry, int[][] edgeAry, BufferedWriter deBugFile) throws IOException {
    deBugFile.write("Entering computeEdge() method\n");

    for(int i = 1; i < numRows+1; i++){
      for(int j = 1; j < numCols+1; j++){
        edgeAry[i][j] = convolution3x3(MFAry, i, j, ZERO_CROSSING_MASK, deBugFile);
      }
    }

    deBugFile.write("Leaving computeEdge method\n");
  }

  private int convolution3x3(int[][] inAry, int rowIndex, int colIndex, int[][] mask, BufferedWriter deBugFile) throws IOException {
    deBugFile.write("Entering convolution3x3 method\n");
    int result = 0;

    for(int row = -1; row <= 1; row++){
      for(int col = -1; col <= 1; col++){
        result += mask[row+1][col+1] * inAry[rowIndex+row][colIndex+col];
      }
    }

    deBugFile.write(String.format("Before leaving convolution3x3 method(): i=%d; j=%d; result=%d\n", rowIndex, colIndex, result));
    return result;
  }

  private void binaryEdgeDetector(int[][] zeroCrossAry, BufferedWriter binEdgeFile) throws IOException{
    for(int i = 1; i < numRows+1; i++){
      for(int j = 1; j < numCols+1; j++){
        boolean isBinaryEdge = false;
        if(zeroCrossAry[i][j] >= 1){
          for(int[] neighbor : NEIGHBORHOOD_OFFSET){
            int newRow = i + neighbor[0];
            int newCol = j + neighbor[1];
            if(zeroCrossAry[newRow][newCol] == 0){
              isBinaryEdge = true;
              break;
            }
          }
        }
        if(isBinaryEdge){
          binEdgeFile.write("1 ");
        } else {
          binEdgeFile.write("0 ");
        }
      }
      binEdgeFile.write("\n");
    }
  }

  public void crossZero(int[][] edgeAry, int[][] zeroCrossAry, BufferedWriter deBugFile) throws IOException{
    deBugFile.write("Entering crossZero() method\n");
    for(int i = 1; i < numRows+1; i++){
      for(int j = 1; j < numCols+1; j++){
        if(edgeAry[i][j] <= 0){
          zeroCrossAry[i][j] = 0;
        } else {
          zeroCrossAry[i][j] = 1;
        }
      }
    }
    deBugFile.write("Leaving crossZero method\n");
  }

  public void printHeader(BufferedWriter file) throws IOException{
    file.write(String.format("%d %d %d %d\n", numRows, numCols, minVal, maxVal));
  }

  public int[][] getMFAry(){
    return this.MFAry;
  }

  public int[][] getEdgeAry(){
    return this.edgeAry;
  }

  public int[][] getZeroCrossAry(){
    return this.zeroCrossAry;
  }


}

public class SongH_Project5_Main {
  public static void main(String[] args){
    try {
      // Step 0, initialize all class variable and parse any input arguments
      BufferedReader inFile = new BufferedReader(new FileReader(args[0]));
      BufferedWriter outFile1 = new BufferedWriter(new FileWriter(args[1]));
      BufferedWriter zeroCrossFile = new BufferedWriter(new FileWriter(args[2]));
      BufferedWriter binEdgeFile = new BufferedWriter(new FileWriter(args[3]));
      BufferedWriter deBugFile = new BufferedWriter(new FileWriter(args[4]));

      String line;
      int numRows = 0, numCols = 0, minVal = 0, maxVal = 0;
      if((line = inFile.readLine()) != null){
        String[] words = line.split(" ");
        numRows = Integer.parseInt(words[0]);
        numCols = Integer.parseInt(words[1]);
        minVal = Integer.parseInt(words[2]);
        maxVal = Integer.parseInt(words[3]);
      }

      Edge edge = new Edge(numRows, numCols, minVal, maxVal);
      int[][] MFAry = edge.getMFAry();
      int[][] edgeAry = edge.getEdgeAry();
      int[][] zeroCrossAry = edge.getZeroCrossAry();

      edge.loadImage(inFile, MFAry);
      edge.printHeader(outFile1);
      edge.imgReformatPrettyPrint(MFAry, outFile1);
      edge.zeroCrossingEdgeDetector(MFAry, edgeAry, zeroCrossAry, zeroCrossFile, binEdgeFile, deBugFile);

      inFile.close();
      outFile1.close();
      zeroCrossFile.close();
      binEdgeFile.close();
      deBugFile.close();
    } catch (Exception e){
      System.out.print("Exception during main, see trace below");
      e.printStackTrace();
    }
  }
}