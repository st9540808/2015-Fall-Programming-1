#include <mpi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stddef.h>
#include <cstring>
#include <cstdlib>
#include "bmp.h"

using namespace std;

//定義平滑運算的次數
#define NSmooth 1000

/*********************************************************/
/*變數宣告：                                             */
/*  bmpHeader    ： BMP檔的標頭                          */
/*  bmpInfo      ： BMP檔的資訊                          */
/*  **BMPSaveData： 儲存要被寫入的像素資料               */
/*  **BMPData    ： 暫時儲存要被寫入的像素資料           */
/*********************************************************/
BMPHEADER bmpHeader;
BMPINFO bmpInfo;
RGBTRIPLE **BMPSaveData = NULL;
RGBTRIPLE **BMPData = NULL;
RGBTRIPLE **BMPBuffer =  NULL; // partial data block from BMPData

/*********************************************************/
/*函數宣告：                                             */
/*  readBMP()  ： 讀取圖檔，並把像素資料儲存在BMPSaveData*/
/*  saveBMP()  ： 寫入圖檔，並把像素資料BMPSaveData寫入  */
/*  swap()     ： 交換二個指標                           */
/*  alloc_memory()： 動態分配一個Y * X矩陣               */
/*********************************************************/
int readBMP(const char *fileName); // read file
int saveBMP(const char *fileName); // save file
void swap(RGBTRIPLE *a, RGBTRIPLE *b);
RGBTRIPLE **alloc_memory(int Y, int X); // allocate memory
void free_memory(RGBTRIPLE **data);

/////////////////////////////////////////////////////////////
// my code
/////////////////////////////////////////////////////////////
int height;
int width;

void smooth_calculate_parallel(MPI_Datatype, int, int, int[], int[]);
void smooth_calculate_single_thread(void);

int main(int argc, char *argv[])
{
    const char *infileName = "input.bmp";
    const char *outfileName = "output2.bmp";
    double startwtime, endwtime;

    int world_size, id, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME]; 
    MPI_Init(&argc, &argv);
    startwtime = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Get_processor_name(processor_name, &namelen);

    if (id == 0) {
        if (readBMP(infileName))
            cout << "Read file successfully!!" << endl;
        else
            cout << "Read file fails!!" << endl;
        height = bmpInfo.biHeight;
        width = bmpInfo.biWidth;
    }

    // broadcast bmpInfo.biHeight and bmpInfo.biWidth
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&width,  1, MPI_INT, 0, MPI_COMM_WORLD);

    // make data type
    MPI_Datatype RGBTriple_mpi, RGBTriple_array_mpi;
    int rgb_blocklengths[] = {1, 1, 1};
    MPI_Aint rgb_displacements[] = {
        offsetof(RGBTRIPLE, rgbBlue),
        offsetof(RGBTRIPLE, rgbGreen),
        offsetof(RGBTRIPLE, rgbRed)
    };
    MPI_Datatype rgb_types[] = {MPI_CHAR, MPI_CHAR, MPI_CHAR};
    MPI_Aint lb, extent;
    MPI_Type_create_struct(3, rgb_blocklengths, rgb_displacements,
                           rgb_types, &RGBTriple_mpi);
    MPI_Type_get_extent(RGBTriple_mpi, &lb, &extent);
    MPI_Type_create_resized(RGBTriple_mpi, lb, extent, &RGBTriple_mpi);
    MPI_Type_commit(&RGBTriple_mpi);
    
    // create an array type of RGBTriple
    MPI_Type_contiguous(width, RGBTriple_mpi, &RGBTriple_array_mpi);
    MPI_Type_commit(&RGBTriple_array_mpi);
    
    int sendcounts[32], displs[32];
    for (int i = 0; i < world_size - 1; i++) {
        sendcounts[i] = height / world_size;
        displs[i] = i * (height / world_size);
    }
    sendcounts[world_size - 1] = height - ((world_size - 1) * (height / world_size));
    displs[world_size - 1] = (world_size - 1) * (height / world_size);

    // prepare memory block to hold RGBTriple data block
    BMPBuffer = alloc_memory(sendcounts[id] + 2, width);
    // swap memory for BMPBuffer for every processor
    BMPData = alloc_memory(sendcounts[id] + 2, width);

    // scattering all data
    if (world_size != 1) {
        if (id == 0) {
            // main block
            MPI_Scatterv(BMPSaveData[0], sendcounts, displs, RGBTriple_array_mpi,
                         BMPBuffer[1], sendcounts[0], RGBTriple_array_mpi,
                         0, MPI_COMM_WORLD);
        } else {
            MPI_Scatterv(NULL, sendcounts, displs, RGBTriple_array_mpi,
                         BMPBuffer[1], sendcounts[id], RGBTriple_array_mpi,
                         0, MPI_COMM_WORLD);
        }
    }

   
    // smooth calculate
    if (world_size == 1)
        smooth_calculate_single_thread();
    else
        smooth_calculate_parallel(RGBTriple_array_mpi, id, world_size,
                                  sendcounts, displs);

    // gathering all data
    if (world_size != 1) {
        if (id == 0) {
            MPI_Gatherv(BMPBuffer[1],  sendcounts[0], RGBTriple_array_mpi,
                        BMPSaveData[0], sendcounts, displs, RGBTriple_array_mpi,
                        0, MPI_COMM_WORLD);
        } else {
            MPI_Gatherv(BMPBuffer[1], sendcounts[id], RGBTriple_array_mpi,
                        NULL, sendcounts, displs, RGBTriple_array_mpi,
                        0, MPI_COMM_WORLD);
        }
    }

    MPI_Type_free(&RGBTriple_mpi);
    MPI_Type_free(&RGBTriple_array_mpi);

    if (id == 0) {
        if (saveBMP(outfileName))
            cout << "Save file successfully!!" << endl;
        else
            cout << "Save file fails!!" << endl;
    }
        
    endwtime = MPI_Wtime();
    cout << "The execution time = " << endwtime - startwtime << endl;

    free_memory(BMPSaveData);
    free_memory(BMPData);
    free_memory(BMPBuffer);
    MPI_Finalize();
    
    return 0;
}


// for multi-processor
void smooth_calculate_parallel(MPI_Datatype RGBTriple_array_mpi, int id,
                               int world_size, int sendcounts[], int displs[]) {
    // calculate boundary displacement
    int counts_all_1[32];
    int upper_boundary[32];
    int lower_reference[32], upper_reference[32];
    for (int i = 0; i < world_size; i++) {
        counts_all_1[i] = 1;
        upper_boundary[i] = i == world_size - 1 ? height - 1 : displs[i + 1] - 1;
        lower_reference[i] = i == 0 ? height - 1 : displs[i] - 1;
        upper_reference[i] = i == world_size - 1 ? 0 : displs[i + 1];
    }

    for (int count = 0; count < NSmooth; count++) {
        // update boundary
        if (id == 0) {
            // gather lower bounary
            MPI_Gatherv(BMPBuffer[1], 1, RGBTriple_array_mpi,
                        BMPSaveData[0], counts_all_1, displs, RGBTriple_array_mpi,
                        0, MPI_COMM_WORLD);
            // gather upper boundary
            MPI_Gatherv(BMPBuffer[sendcounts[0]], 1, RGBTriple_array_mpi,
                        BMPSaveData[0], counts_all_1, upper_boundary, RGBTriple_array_mpi,
                        0, MPI_COMM_WORLD);
        } else {
            MPI_Gatherv(BMPBuffer[1], 1, RGBTriple_array_mpi,
                        NULL, counts_all_1, displs, RGBTriple_array_mpi,
                        0, MPI_COMM_WORLD);
            MPI_Gatherv(BMPBuffer[sendcounts[id]], 1, RGBTriple_array_mpi,
                        NULL, counts_all_1, upper_boundary, RGBTriple_array_mpi,
                        0, MPI_COMM_WORLD);
        }

        if (id == 0) {
            // scatter lower reference
            MPI_Scatterv(BMPSaveData[0], counts_all_1, lower_reference, 
                         RGBTriple_array_mpi, BMPBuffer[0], 1, RGBTriple_array_mpi,
                         0, MPI_COMM_WORLD);
            // scatter upper reference
            MPI_Scatterv(BMPSaveData[0], counts_all_1, upper_reference, 
                         RGBTriple_array_mpi, BMPBuffer[sendcounts[0] + 1],
                         1, RGBTriple_array_mpi, 0, MPI_COMM_WORLD);
        } else {
            MPI_Scatterv(NULL, counts_all_1, lower_reference, 
                         RGBTriple_array_mpi, BMPBuffer[0], 1, RGBTriple_array_mpi,
                         0, MPI_COMM_WORLD);
            MPI_Scatterv(NULL, counts_all_1, upper_reference, 
                         RGBTriple_array_mpi, BMPBuffer[sendcounts[id] + 1],
                         1, RGBTriple_array_mpi, 0, MPI_COMM_WORLD);
        }

        std::cout << "bounary updated in " << count << std::endl;

        swap(BMPBuffer, BMPData);
        for (int i = 1; i < sendcounts[id] + 1; i++) {
            for (int j = 0; j < width; j++) {
                int Top   = i - 1;
                int Down  = i + 1;
                int Left  = j == 0 ? width - 1 : j - 1;
                int Right = j == width - 1 ? 0 : j + 1;

                BMPBuffer[i][j].rgbBlue =
                    (double)(BMPData[i][j].rgbBlue +
                             BMPData[Top][j].rgbBlue +
                             BMPData[Down][j].rgbBlue +
                             BMPData[i][Left].rgbBlue +
                             BMPData[i][Right].rgbBlue) / 5 + 0.5;
                BMPBuffer[i][j].rgbGreen =
                    (double)(BMPData[i][j].rgbGreen +
                             BMPData[Top][j].rgbGreen +
                             BMPData[Down][j].rgbGreen +
                             BMPData[i][Left].rgbGreen +
                             BMPData[i][Right].rgbGreen) / 5 + 0.5;
                BMPBuffer[i][j].rgbRed =
                    (double)(BMPData[i][j].rgbRed +
                             BMPData[Top][j].rgbRed +
                             BMPData[Down][j].rgbRed +
                             BMPData[i][Left].rgbRed +
                             BMPData[i][Right].rgbRed) / 5 + 0.5;
            }
        }
    }
}

// for a single processor
void smooth_calculate_single_thread() {
    for (int count = 0; count < NSmooth; count++) {
        swap(BMPSaveData, BMPData);
        std::cout << "bounary updated in " << count << std::endl;
        
        for (int i = 0; i < bmpInfo.biHeight; i++) {
            for (int j = 0; j < bmpInfo.biWidth; j++) {
                int Top   = i == 0 ? bmpInfo.biHeight - 1 : i - 1;
                int Down  = i == bmpInfo.biHeight - 1 ? 0 : i + 1;
                int Left  = j == 0 ? bmpInfo.biWidth - 1  : j - 1;
                int Right = j == bmpInfo.biWidth - 1 ? 0  : j + 1;

                BMPSaveData[i][j].rgbBlue =
                    (double)(BMPData[i][j].rgbBlue +
                             BMPData[Top][j].rgbBlue +
                             BMPData[Down][j].rgbBlue +
                             BMPData[i][Left].rgbBlue +
                             BMPData[i][Right].rgbBlue) / 5 + 0.5;
                BMPSaveData[i][j].rgbGreen =
                    (double)(BMPData[i][j].rgbGreen +
                             BMPData[Top][j].rgbGreen +
                             BMPData[Down][j].rgbGreen +
                             BMPData[i][Left].rgbGreen +
                             BMPData[i][Right].rgbGreen) / 5 + 0.5;
                BMPSaveData[i][j].rgbRed =
                    (double)(BMPData[i][j].rgbRed +
                             BMPData[Top][j].rgbRed +
                             BMPData[Down][j].rgbRed +
                             BMPData[i][Left].rgbRed +
                             BMPData[i][Right].rgbRed) / 5 + 0.5;
            }
        }
    }
}


/////////////////////////////////////////////////////////////
// end of my code
/////////////////////////////////////////////////////////////


/*********************************************************/
/* 讀取圖檔                                              */
/*********************************************************/
int readBMP(const char *fileName) {
    //建立輸入檔案物件
    ifstream bmpFile(fileName, ios::in | ios::binary);

    //檔案無法開啟
    if (!bmpFile) {
        cout << "It can't open file!!" << endl;
        return 0;
    }

    //讀取BMP圖檔的標頭資料
    bmpFile.read((char *)&bmpHeader, sizeof(BMPHEADER));

    //判決是否為BMP圖檔
    if (bmpHeader.bfType != 0x4d42) {
        cout << "This file is not .BMP!!" << endl;
        return 0;
    }

    //讀取BMP的資訊
    bmpFile.read((char *)&bmpInfo, sizeof(BMPINFO));

    //判斷位元深度是否為24 bits
    if (bmpInfo.biBitCount != 24) {
        cout << "The file is not 24 bits!!" << endl;
        return 0;
    }

    //修正圖片的寬度為4的倍數
    while (bmpInfo.biWidth % 4 != 0)
        bmpInfo.biWidth++;

    //動態分配記憶體
    BMPSaveData = alloc_memory(bmpInfo.biHeight, bmpInfo.biWidth);

    //讀取像素資料
    // for(int i = 0; i < bmpInfo.biHeight; i++)
    //  bmpFile.read( (char* )BMPSaveData[i], bmpInfo.biWidth*sizeof(RGBTRIPLE));
    bmpFile.read((char *)BMPSaveData[0],
                 bmpInfo.biWidth * sizeof(RGBTRIPLE) * bmpInfo.biHeight);

    //關閉檔案
    bmpFile.close();

    return 1;
}
/*********************************************************/
/* 儲存圖檔                                              */
/*********************************************************/
int saveBMP(const char *fileName) {
    //判決是否為BMP圖檔
    if (bmpHeader.bfType != 0x4d42) {
        cout << "This file is not .BMP!!" << endl;
        return 0;
    }

    //建立輸出檔案物件
    ofstream newFile(fileName, ios::out | ios::binary);

    //檔案無法建立
    if (!newFile) {
        cout << "The File can't create!!" << endl;
        return 0;
    }

    //寫入BMP圖檔的標頭資料
    newFile.write((char *)&bmpHeader, sizeof(BMPHEADER));

    //寫入BMP的資訊
    newFile.write((char *)&bmpInfo, sizeof(BMPINFO));

    //寫入像素資料
    // for( int i = 0; i < bmpInfo.biHeight; i++ )
    //        newFile.write( ( char* )BMPSaveData[i],
    //        bmpInfo.biWidth*sizeof(RGBTRIPLE) );
    newFile.write((char *)BMPSaveData[0],
                  bmpInfo.biWidth * sizeof(RGBTRIPLE) * bmpInfo.biHeight);

    //寫入檔案
    newFile.close();

    return 1;
}

/*********************************************************/
/* 分配記憶體：回傳為Y*X的矩陣                           */
/*********************************************************/
RGBTRIPLE **alloc_memory(int Y, int X) {
    //建立長度為Y的指標陣列
    RGBTRIPLE **temp = new RGBTRIPLE *[Y];
    RGBTRIPLE *temp2 = new RGBTRIPLE [Y * X];
    memset(temp2, 0, sizeof(RGBTRIPLE) * Y * X );
    if (!temp || !temp2) {
        std::cerr << "err in alloc_memory()\n";
        exit(1);
    }

    //對每個指標陣列裡的指標宣告一個長度為X的陣列
    for (int i = 0; i < Y; i++) {
        temp[i] = &temp2[i * X];
    }

    return temp;
}

void free_memory(RGBTRIPLE **data) {
    if (data != NULL) {
        delete [] *data;
        delete [] data;
    }
}
/*********************************************************/
/* 交換二個指標                                          */
/*********************************************************/
void swap(RGBTRIPLE *a, RGBTRIPLE *b) {
    RGBTRIPLE *temp;
    temp = a;
    a = b;
    b = temp;
}
