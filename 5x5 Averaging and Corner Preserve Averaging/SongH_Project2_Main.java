import java.io.*;

class Enhancement {
    private int numRows;
    private int numCols;
    private int minVal;
    private int maxVal;
    private int[][] mirrorFramedAry;
    private int[][] avgAry;
    private int[][] CPavgAry;
    private final int[][][] mask = new int[][][]
        {
                {
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 0, 1, 0, 0},
                        {0, 1, 1, 1, 0},
                        {1, 1, 1, 1, 1}
                },
                {
                        {1, 0, 0, 0, 0},
                        {1, 1, 0, 0, 0},
                        {1, 1, 1, 0, 0},
                        {1, 1, 0, 0, 0},
                        {1, 0, 0, 0, 0}
                },
                {
                        {1, 1, 1, 1, 1},
                        {0, 1, 1, 1, 0},
                        {0, 0, 1, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0}
                },
                {
                        {0, 0, 0, 0, 1},
                        {0, 0, 0, 1, 1},
                        {0, 0, 1, 1, 1},
                        {0, 0, 0, 1, 1},
                        {0, 0, 0, 0, 1}
                },
                {
                        {1, 1, 1, 0, 0},
                        {1, 1, 1, 0, 0},
                        {1, 1, 1, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0}
                },
                {
                        {0, 0, 1, 1, 1},
                        {0, 0, 1, 1, 1},
                        {0, 0, 1, 1, 1},
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0}
                },
                {
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 0, 1, 1, 1},
                        {0, 0, 1, 1, 1},
                        {0, 0, 1, 1, 1}
                },
                {
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0},
                        {1, 1, 1, 0, 0},
                        {1, 1, 1, 0, 0},
                        {1, 1, 1, 0, 0}
                }
        };
    private int[] Avg_histAry;
    private int[] CPavg_histAry;

    Enhancement(int numRows, int numCols, int minVal, int maxVal){
        this.numRows = numRows;
        this.numCols = numCols;
        this.minVal = minVal;
        this.maxVal = maxVal;
        this.mirrorFramedAry = new int[numRows+4][numCols+4];
        this.avgAry = new int[numRows+4][numCols+4];
        this.CPavgAry = new int[numRows+4][numCols+4];
        this.Avg_histAry = new int[maxVal+1];
        this.CPavg_histAry = new int[maxVal+1];
    }

    public void loadImage(BufferedReader inFile) throws IOException {
        String line;
        int rowIndex = 2;

        while((line = inFile.readLine()) != null){
            String[] pixels = line.split(" ");
            int colIndex = 2;
            for(String pixel : pixels){
                mirrorFramedAry[rowIndex][colIndex] = Integer.parseInt(pixel);
                colIndex++;
            }
            rowIndex++;
        }
    }

    public void mirrorFraming(){
        copyRowToRow(2, 0);
        copyRowToRow(2, 1);
        copyRowToRow(numRows+1, numRows+2);
        copyRowToRow(numRows+1, numRows+3);
        copyColToCol(2, 0);
        copyColToCol(2, 1);
        copyColToCol(numCols+1, numCols+2);
        copyColToCol(numCols+1, numCols+3);
    }

    private void copyRowToRow(int sourceIndex, int targetIndex){
        int mirrorFrameAryNumCols = mirrorFramedAry[0].length;
        for(int i = 2; i < mirrorFrameAryNumCols-2; i++){
            mirrorFramedAry[targetIndex][i] = mirrorFramedAry[sourceIndex][i];
        }
    }

    private void copyColToCol(int sourceIndex, int targetIndex){
        for(int i = 0; i < mirrorFramedAry.length; i++){
            mirrorFramedAry[i][targetIndex] = mirrorFramedAry[i][sourceIndex];
        }
    }

    public int[][] getMirrorFramedAry(){
        return mirrorFramedAry;
    }

    public void imgReformat(int[][] inAry, BufferedWriter fileOut) throws IOException {
        fileOut.write(String.format("%d %d %d %d\n", numRows, numCols, minVal, maxVal));
        String str = Integer.toString(maxVal);
        int width = str.length();
        int i = 2;
        while(i < (numRows+2)){
            int j = 2;
            while(j < (numCols+2)){
                str = Integer.toString(inAry[i][j]);
                fileOut.write(str);
                int WW = str.length();
                while(WW <= width){
                    fileOut.write(" ");
                    WW++;
                }
                j++;
            }
            fileOut.write("\n");
            i++;
        }
    }

    public void computeAvg5x5(BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering computeAvg5x5 method\n");
        int newMax = Integer.MIN_VALUE;
        int newMin = Integer.MAX_VALUE;

        for(int i = 2; i < numRows+2; i++){
            for(int j = 2; j < numCols+2; j++){
                avgAry[i][j] = avg5x5(i, j, mirrorFramedAry, deBugFile);
                if(newMax < avgAry[i][j]){
                    newMax = avgAry[i][j];
                }
                if(newMin > avgAry[i][j]){
                    newMin = avgAry[i][j];
                }
            }
        }
        maxVal = newMax;
        minVal = newMin;
        deBugFile.write(String.format("In computeAvg5x5 newMax=%d, newMin=%d\n", newMax, newMin));
        deBugFile.write("Leaving computeAvg5x5 method\n");
    }

    public void cornerPreserveAvg(BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering cornerPreserveAvg() method\n");
        int newMax = Integer.MIN_VALUE;
        int newMin = Integer.MAX_VALUE;

        for(int i = 2; i < numRows+2; i++){
            for(int j = 2; j < numCols+2; j++){
                int minAvg = Integer.MAX_VALUE;
                int minDiff = Integer.MAX_VALUE;
                for(int maskIndex = 0; maskIndex < 8; maskIndex++){
                    int result = convolution5x5(i, j, mask[maskIndex], mirrorFramedAry, deBugFile) / 9;
                    int diff = Math.abs(result - mirrorFramedAry[i][j]);
                    if(diff < minDiff) {
                        minDiff = diff;
                        minAvg = result;
                    }
                }
                CPavgAry[i][j] = minAvg;
                if(newMax < minAvg){
                    newMax = minAvg;
                }
                if(newMin > minAvg){
                    newMin = minAvg;
                }
            }
        }

        maxVal = newMax;
        minVal = newMin;
        deBugFile.write(String.format("In cornerPreserveAvg(): newMax=%d, newMin=%d\n", newMax, newMin));
        deBugFile.write("Leaving cornerPreserveAvg() method\n");
    }

    private int avg5x5(int rowIndex, int colIndex, int[][] inAry, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering avg5x5 method\n");
        int totalSum = 0;
        for(int i = rowIndex-2; i < rowIndex+3; i++){
            for(int j = colIndex-2; j < colIndex+3; j++){
                totalSum += inAry[i][j];
            }
        }
        deBugFile.write(String.format("Before leaving avg5x5 method(): i=%d; j=%d; result=%d\n", rowIndex, colIndex, totalSum / 25));
        return totalSum / 25;
    }

    private int convolution5x5(int rowIndex, int colIndex, int[][] mask, int[][] inAry, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering convolution5x5 method\n");
        int result = 0;

        for(int row = -2; row <= 2; row++){
            for(int col = -2; col <= 2; col++){
                result += mask[row+2][col+2] * inAry[rowIndex+row][colIndex+col];
            }
        }

        deBugFile.write(String.format("Before leaving convolution5x5 method(): i=%d; j=%d; result=%d\n", rowIndex, colIndex, result));
        return result;
    }

    public int[][] getAvgAry(){
        return avgAry;
    }

    public int[][] getCPavgAry(){
        return CPavgAry;
    }

    public int[] getCPavg_histAry(){
        return CPavg_histAry;
    }

    public int getMinVal(){
        return minVal;
    }

    public int getMaxVal(){
        return maxVal;
    }

    public int[] getAvg_histAry(){
        return Avg_histAry;
    }

    public void computeHist(int[][] inAry, int[] histAry, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering computeHist method\n");
        for(int i = 2; i < numRows+2; i++){
            for(int j = 2; j < numCols+2; j++){
                histAry[inAry[i][j]]++;
            }
        }
        deBugFile.write("Leaving computeHist method\n");
    }

    public void dispHist(int[] inAry, BufferedWriter outFile1) throws IOException {
        outFile1.write(String.format("%d %d %d %d\n", numRows, numCols, minVal, maxVal));
        for(int i = 0; i < maxVal+1; i++){
            outFile1.write(String.format("%d (%d): %s\n", i, inAry[i], "+".repeat(inAry[i])));
        }
    }

    public void printHist(int[] histAry, BufferedWriter histFile, BufferedWriter deBugFile) throws IOException {
        deBugFile.write("Entering printHist method\n");
        histFile.write(String.format("%d %d %d %d\n", numRows, numCols, minVal, maxVal));
        for(int i = 0; i <= maxVal; i++){
            histFile.write(String.format("%d %d",i, histAry[i]));
            if(i+1 <= maxVal){
                histFile.write("\n");
            }
        }
        deBugFile.write("Leaving printHist method\n");
    }
}

public class SongH_Project2_Main {
    public static void main(String[] args) {
        try {
            // Step 0, initialize all class variable and parse any input arguments
            BufferedReader inFile = new BufferedReader(new FileReader(args[0]));
            BufferedWriter outFile1 = new BufferedWriter(new FileWriter("outFile1.txt"));
            BufferedWriter outFile2 = new BufferedWriter(new FileWriter("outFile2.txt"));
            BufferedWriter outFile3 = new BufferedWriter(new FileWriter("outFile3.txt"));
            BufferedWriter deBugFile = new BufferedWriter(new FileWriter("deBugFile.txt"));
            int choice = Integer.parseInt(args[1]);
            if(choice != 1 && choice != 2){
                throw new Exception("User choice must be 1 or 2");
            }
            String line;
            int numRows = 0, numCols = 0, minVal = 0, maxVal = 0;
            if((line = inFile.readLine()) != null){
                String[] words = line.split(" ");
                numRows = Integer.parseInt(words[0]);
                numCols = Integer.parseInt(words[1]);
                minVal = Integer.parseInt(words[2]);
                maxVal = Integer.parseInt(words[3]);
            }
            Enhancement enhancement = new Enhancement(numRows, numCols, minVal, maxVal);
            enhancement.loadImage(inFile);
            enhancement.mirrorFraming();
            enhancement.imgReformat(enhancement.getMirrorFramedAry(), outFile1);

            // Step 4, 5x5 averaging
            if(choice == 1){
                int[][] avgAry = enhancement.getAvgAry();
                int[] Avg_histAry = enhancement.getAvg_histAry();
                enhancement.computeAvg5x5(deBugFile);
                outFile1.write("Below is the reformatted image of the result of 5x5 averaging on the input image\n");
                enhancement.imgReformat(avgAry, outFile1);
                outFile2.write(String.format("%d %d %d %d\n", numRows, numCols, enhancement.getMinVal(), enhancement.getMaxVal()));
                for(int i = 2; i < numRows+2; i++){
                    for(int j = 2; j < numCols+2; j++){
                        outFile2.write(Integer.toString(avgAry[i][j]));
                        if(j+1 < numCols+2){
                            outFile2.write(" ");
                        }
                    }
                    if(i+1 < numRows+2){
                        outFile2.write("\n");
                    }
                }
                enhancement.computeHist(avgAry, Avg_histAry, deBugFile);
                enhancement.dispHist(Avg_histAry, outFile1);
                enhancement.printHist(Avg_histAry, outFile3, deBugFile);
            }
            // Step 5, corner preserve averaging
            if(choice == 2){
                int[][] CPavgAry = enhancement.getCPavgAry();
                int[] CPavg_histAry = enhancement.getCPavg_histAry();
                enhancement.cornerPreserveAvg(deBugFile);
                outFile1.write("Below is the reformatted image of the result of corner-preserve averaging on the input image\n");
                enhancement.imgReformat(CPavgAry, outFile1);
                outFile2.write(String.format("%d %d %d %d\n", numRows, numCols, enhancement.getMinVal(), enhancement.getMaxVal()));
                for(int i = 2; i < numRows+2; i++){
                    for(int j = 2; j < numCols+2; j++){
                        outFile2.write(Integer.toString(CPavgAry[i][j]));
                        if(j+1 < numCols+2){
                            outFile2.write(" ");
                        }
                    }
                    if(i+1 < numRows+2){
                        outFile2.write("\n");
                    }
                }
                enhancement.computeHist(CPavgAry, CPavg_histAry, deBugFile);
                enhancement.dispHist(CPavg_histAry, outFile1);
                enhancement.printHist(CPavg_histAry, outFile3, deBugFile);
            }

            // close all files after processing is complete
            inFile.close();
            outFile1.close();
            outFile2.close();
            outFile3.close();
            deBugFile.close();
        } catch(Exception e){
            System.out.print("Exception during main, see trace below");
            e.printStackTrace();
        }
    }
}