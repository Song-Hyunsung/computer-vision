import java.io.*;

class Image {
    private int numRows;
    private int numCols;
    private int minVal;
    private int maxVal;
    private int thrValue;

    Image(int numRows, int numCols, int minVal, int maxVal, int thrValue){
        this.numRows = numRows;
        this.numCols = numCols;
        this.minVal = minVal;
        this.maxVal = maxVal;
        this.thrValue = thrValue;
    }

    public void Processing(BufferedReader inFile, BufferedWriter outFile1, BufferedWriter outFile2, int thrValue) throws IOException {
        int pixelVal;
        int count = 0;
        String line;

        while((line = inFile.readLine()) != null){
            String[] pixels = line.split(" ");
            for(String pixel : pixels){
                pixelVal = Integer.parseInt(pixel);
                if(pixelVal >= thrValue){
                    outFile1.write("1 ");
                    outFile2.write(pixelVal + " ");
                } else {
                    outFile1.write("0 ");
                    outFile2.write("0  ");
                }
                count++;
                if(count >= numCols){
                    outFile1.write("\n");
                    outFile2.write("\n");
                    count = 0;
                }
            }
        }
    }
}

public class SongH_Project0_Main {
    public static void main(String[] args) {
        try {
            BufferedReader inFile = new BufferedReader(new FileReader(args[0]));
            BufferedWriter outFile1 = new BufferedWriter(new FileWriter(args[2]));
            BufferedWriter outFile2 = new BufferedWriter(new FileWriter(args[3]));
            String line;
            int thrValue = Integer.parseInt(args[1]);
            int numRows, numCols, minVal, maxVal;

            if((line = inFile.readLine()) != null){
                String[] words = line.split(" ");
                numRows = Integer.parseInt(words[0]);
                numCols = Integer.parseInt(words[1]);
                minVal = Integer.parseInt(words[2]);
                maxVal = Integer.parseInt(words[3]);

                outFile1.write(numRows + " " + numCols + " 0 " + "1\n");
                outFile2.write(numRows + " " + numCols + " 0 " + maxVal + "\n");

                Image inputImage = new Image(numRows, numCols, minVal, maxVal, thrValue);
                inputImage.Processing(inFile, outFile1, outFile2, thrValue);
            }

            inFile.close();
            outFile1.close();
            outFile2.close();
        } catch(Exception e){
            System.out.println("Error during threshold operation.");
            e.printStackTrace();
        }
    }
}