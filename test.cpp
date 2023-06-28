#include <stdio.h>
#include "draw.h"

void testMatrixIndexRotation();

// test
int main() {
    testMatrixIndexRotation();
}

void printMat(float mat[]){
     for(int i =0; i < 9; i++ ){
        if(i!=0)
            printf(",");
        if(i%3 == 0) {
            printf("\n");
        }
        printf(" %.2f", mat[i]);
    }
    printf("\n");
}

void testMatrixIndexRotation(){
    printf("\n -- testMatrixIndexRotation -- \n\n");
    float testMatrix[9];
    for(int i =0; i < 9; i++ ){
        testMatrix[i] = (float)(i+1.0f);
    }

    printMat(testMatrix);
    int inArr[2] = {0};
    int outArr[2] = {0};
    for(int i = 0; i<2; i++){
        inArr[0] = (i)/3;
        inArr[1] = i%3;

        float tmpV = testMatrix[inArr[0]*3 + inArr[1]];
        for(int j = 0; j < 4; j++){        
            forwardRotation(inArr, outArr);
            float tmp = testMatrix[outArr[0]*3 + outArr[1]];
            testMatrix[outArr[0]*3 + outArr[1]] = tmpV;
            tmpV = tmp;

            inArr[0] = outArr[0];
            inArr[1] = outArr[1];
        }
    }
    printMat(testMatrix);

    
    for(int i = 0; i<2; i++){
        inArr[0] = (i)/3;
        inArr[1] = i%3;

        float tmpV = testMatrix[inArr[0]*3 + inArr[1]];
        for(int j = 0; j < 4; j++){        
            backwardRotation(inArr, outArr);
            float tmp = testMatrix[outArr[0]*3 + outArr[1]];
            testMatrix[outArr[0]*3 + outArr[1]] = tmpV;
            tmpV = tmp;

            inArr[0] = outArr[0];
            inArr[1] = outArr[1];
        }
    }
    printMat(testMatrix);

}