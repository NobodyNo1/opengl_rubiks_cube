#define H_PI       3.14159265358979323846   // pi

float mod(float a, float b){
    float c = a;
    while(c>b){
        c -= b;
    }
    return c;
}

float min(float a, float b)
{
    if (a < b)
        return a;
    return b;
}

float max(float a, float b)
{
    if (a > b)
        return a;
    return b;
}

// 3x3 matrix rotation (clockwise)
void forwardRotation(const int arr[2], int outArr[2])
{
    int i, j;
    i = arr[0];
    j = arr[1];

    int p = 3 * i + j;
    int nextPos = (3 * p + 2) % 10;
    int nI = nextPos / 3;
    int nJ = nextPos % 3;

    outArr[0] = nI;
    outArr[1] = nJ;
}


// 3x3 matrix rotation (anti-clockwise)
void backwardRotation(const int arr[2], int outArr[2])
{
    forwardRotation(arr, outArr);
    int tmpArr[2];
    forwardRotation(outArr, tmpArr);
    forwardRotation(tmpArr, outArr);
}

template<typename T>
void bulkMatrixRotation(int is_forward, T modelIdxes[])
{
    int inArr[2] = {0};
    int outArr[2] = {0};
    for (int i = 0; i < 2; i++)
    {
        inArr[0] = (i) / 3;
        inArr[1] = i % 3;

        //FOR C: void* tmpV = ((void**)modelIdxes)[inArr[0] * 3 + inArr[1]];
        T tmpV = modelIdxes[inArr[0] * 3 + inArr[1]];
        for (int j = 0; j < 4; j++)
        {
            if (is_forward)
                forwardRotation(inArr, outArr);
            else
                backwardRotation(inArr, outArr);
            /*
            FOR C: 
                void* tmp = ((void**)modelIdxes)[outArr[0] * 3 + outArr[1]];
                ((void**)modelIdxes)[outArr[0] * 3 + outArr[1]] = tmpV;
            */
            T tmp = modelIdxes[outArr[0] * 3 + outArr[1]];
            modelIdxes[outArr[0] * 3 + outArr[1]] = tmpV;
            tmpV = tmp;

            inArr[0] = outArr[0];
            inArr[1] = outArr[1];
        }
    }
}

double convert_to_rad(double angle_in_deg)
{
    double angle_in_rad;
    angle_in_rad = (angle_in_deg * H_PI) / 180;
    return angle_in_rad;
}