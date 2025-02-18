#include <cstdio>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include "MoravianCameraSDK/gxeth.h"
#include "window.h"

using namespace System;
using namespace System::Windows::Forms;

#define MAX_PIXELS 1000
#define MAX_STARS 100

typedef struct {
    double x;
    double y;
    double intensity;
} Pixel;

typedef struct {
    Pixel* pixels;
    Pixel center;
    Pixel prevCenter;
    int quantity;
    double distance;
} Star;

using namespace gxetha;
using namespace Gdiplus;

CARDINAL cameraId = 0;

static GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR gdiplusToken;

void initPNG()
{
    // Initialize GDI+
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

bool GetEncoderClsid(const wchar_t* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    if (size == 0) return false;

    uint8_t* buffer = (uint8_t*)malloc(size);

    ImageCodecInfo* pImageCodecInfo = reinterpret_cast<ImageCodecInfo*>(buffer);
    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT i = 0; i < num; ++i) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(buffer);
            return true;
        }
    }
    free(buffer);
    return false;
}

int SaveToTxt(const wchar_t* filename, uint16_t* pixelData, int width, int height)
{
    FILE* f;
    if (_wfopen_s(&f, filename, L"w+"))
        return 0;
    for (int r = 0; r < height; r++)
    {
        for (int c = 0; c < width - 1; c++)
            fprintf(f, "%d ", pixelData[r * width + c]);
        fprintf(f, "%d\n", pixelData[(r + 1) * width - 1]);
    }
    fclose(f);
    return 1;
}

int SaveToPNG(const wchar_t* filename, uint16_t* pixelData, int width, int height)
{
    // Create a Bitmap object
    Bitmap bitmap(width, height, PixelFormat24bppRGB);

    // Lock the bitmap for direct access to pixels
    BitmapData bitmapData;
    Rect rect(0, 0, width, height);

    if (bitmap.LockBits(&rect, ImageLockModeWrite, PixelFormat24bppRGB, &bitmapData) == Ok) {
        // Copy pixel data to the bitmap
        uint8_t* bgr = (uint8_t*)malloc(width * height * 3);
        uint32_t max = 0;
        for (int i = 0; i < width * height; i++) {
            uint16_t gray = pixelData[i];
            if (gray > max) max = gray;
        }
        printf("max=%hu\n", max);
        for (int i = 0; i < width * height; i++) {
            uint16_t val = (uint16_t)(10 * (pixelData[i] / (double)max * 255));
            if (val > 255) val = 255;
            if (val < 100) val = 0;
            uint8_t gray = (uint8_t)val;
            bgr[3 * i + 0] = gray;
            bgr[3 * i + 1] = gray;
            bgr[3 * i + 2] = gray;
        }
        memcpy(bitmapData.Scan0, bgr, width * height * 3);
        free(bgr);
        bitmap.UnlockBits(&bitmapData);
    }
    else {
        printf("Failed to lock bitmap.\n");
        return 0;
    }

    // Save as PNG
    CLSID clsid;
    if (GetEncoderClsid(L"image/png", &clsid)) {

        Status s = bitmap.Save(filename, &clsid, 0);
        if (s != Ok) {
            printf("Failed to save PNG.\n");
            return 0;
        }
    }
    else {
        printf("Failed to get PNG encoder CLSID..\n");
        return 0;
    }
    return 1;
}

void donePng()
{
    // Shutdown GDI+
    GdiplusShutdown(gdiplusToken);
}

void EnumCamera_CB(CARDINAL EnumId)
{
    cameraId = EnumId;
}

/* OUR PART */
// Function to find the brightest pixels in the image. May be integrated along with the function save.txt?
int FindBrightestPixels(uint16_t* imageData, Pixel* pixels, int* count, int width, int height) {

    //LOCALIZE THE MAXIMUM OF THE INTENSITY, DOING A MAX VALUE TO THE PIXEL DATA
    int minIntensity = 30000;
    int currentCount = 0;
    int pixelValue = 0;

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            pixelValue = imageData[r * width + c];
            if (pixelValue > minIntensity) {
                pixels[currentCount].x = c;
                pixels[currentCount].y = r;
                pixels[currentCount].intensity = pixelValue;
                currentCount++;
            }
        }
    }

    *count = currentCount;

    printf("\nFound %d Pixels with notable intensity\n", currentCount);
    return 1;
}

double AreAdjacent(Pixel a, Pixel b) {
    return fabs(a.x - b.x) <= 1 && fabs(a.y - b.y) <= 1;
}

double CalculateDistance(Pixel a, Pixel b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// DFS function to find the largest connected component, the largest star
void DFS(Pixel* Pixels, int count, int* visited, int index, Pixel* currentStar, int* size) {
    visited[index] = 1;
    currentStar[(*size)++] = Pixels[index];

    for (int i = 0; i < count; i++) {
        if (!visited[i] && AreAdjacent(Pixels[index], Pixels[i])) {
            DFS(Pixels, count, visited, i, currentStar, size);
        }
    }
}

// Function to find the largest connected component, the largest star
Star* FindAllStars(Pixel* Pixels, int count, int* starCount) {
    int visited[MAX_PIXELS] = { 0 };
    Star* stars = (Star*)malloc(sizeof(Star) * MAX_PIXELS); // Allocate space for all stars
    *starCount = 0; // Initialize the star count

    Pixel* currentStar = (Pixel*)malloc(sizeof(Pixel) * count); // Temporary storage for a single star
    int currentSize = 0;

    for (int i = 0; i < count; i++) {
        if (!visited[i]) {
            currentSize = 0;
            DFS(Pixels, count, visited, i, currentStar, &currentSize);

            // Allocate and store the current star
            stars[*starCount].pixels = (Pixel*)malloc(sizeof(Pixel) * currentSize);
            stars[*starCount].quantity = currentSize;
            memcpy(stars[*starCount].pixels, currentStar, sizeof(Pixel) * currentSize);

            (*starCount)++; // Increment the star count
        }
    }
    free(currentStar);
    return stars;      // Return the list of stars
}

void CalculateStarCenters(Star* stars, int starCount) {
    for (int i = 0; i < starCount; i++) {
        Pixel center = { 0, 0 };
        double totalWeight = 0;

        for (int j = 0; j < stars[i].quantity; j++) {
            Pixel Pixel = stars[i].pixels[j];
            center.x += Pixel.x * Pixel.intensity;
            center.y += Pixel.y * Pixel.intensity;
            totalWeight += Pixel.intensity;
        }

        if (totalWeight > 0) {
            center.x /= totalWeight; // Weighted average for x
            center.y /= totalWeight; // Weighted average for y
            center.intensity = totalWeight / stars[i].quantity;
        }
        stars[i].center = center; // Store the calculated center
    }
}

void ProcessStars(Star* stars, int starCount, Star* previousStars, int previousStarCount, bool firstIteration) {
    for (int j = 0; j < starCount; j++) {
        Star star = stars[j];
        printf("    Star %d:\n", j + 1);
        printf("        Size: %d Number of pixels\n", star.quantity);
        printf("        Center: (%.10f, %.10f)\n", star.center.x, star.center.y);
        printf("        Intensity: %.10f\n", star.center.intensity);

        if (!firstIteration) {
            double minDistance = INFINITY;
            int matchedIndex = -1;

            for (int k = 0; k < previousStarCount; k++) {
                double distance = CalculateDistance(star.center, previousStars[k].center);
                if (distance < minDistance) {
                    minDistance = distance;
                    matchedIndex = k;
                }
            }

            if (matchedIndex != -1) {
                stars[j].distance = minDistance;
                //printf("    Distance with the previous image of Star %d is: %.10f\n", matchedIndex + 1, minDistance);
            }
            else {
                printf("No matching star found in the previous image\n");
            }
        }
    }
}

void sendBrightestStars(Star* stars, int starCount, bool firstIteration) {
    Star BrightestStar = stars[0];

    for (int i = 1; i < starCount; i++) {
        if (stars[i].quantity >= 5) {
            if (stars[i].center.intensity > BrightestStar.center.intensity) {
                BrightestStar = stars[i];
            }
        }
    }
    printf("Brightest Star:\n");
    printf("    Size: %d Number of pixels\n", BrightestStar.quantity);
    printf("    Center: (%.10f, %.10f)\n", BrightestStar.center.x, BrightestStar.center.y);
    printf("    Intensity: %.10f\n", BrightestStar.center.intensity);
    if (!firstIteration) {
        printf("    Relative shift: %.10f\n", BrightestStar.distance);
    }
}

/* ADDED CODE IN MAIN*/
void AddedCode(uint16_t* imageData, int width, int height) {
    Pixel pixels[MAX_PIXELS];
    int count;
    char filename[50];
    int starCount;

    bool firstIteration = true;
    Star* stars = NULL; // To store stars from the current iteration
    Star* previousStars = NULL; // To store stars from the previous iteration

    int previousStarCount = 0;

    if (!FindBrightestPixels(imageData, pixels, &count, width, height))
    {
        return;
    }
    stars = FindAllStars(pixels, count, &starCount);
    CalculateStarCenters(stars, starCount);

    printf("Found %d stars in file %s:\n", starCount, filename);

    ProcessStars(stars, starCount, previousStars, previousStarCount, firstIteration);

    sendBrightestStars(stars, starCount, firstIteration);

    free(previousStars);
    previousStars = stars; // Assign current stars to previous stars
    previousStarCount = starCount; // Assign current star count to previous star count
    firstIteration = false; //Helps jumping the first iteration

    if (previousStars != NULL) {
        free(previousStars);
    }
}

//PARAMETERS
double expTime = 0.0;
int width = 0;
int height = 0;
const char* ETH_ADAPTER_IP = "192.168.1.100";


void run()
{

    initPNG();

    Enumerate(EnumCamera_CB);

    if (cameraId == 0)
    {
        printf("No cameras detected");
        return;
    }
    printf("Using camera id: %u\n", cameraId);

    CCamera* cameraHandle = Initialize(cameraId);
    if (!cameraHandle)
    {
        printf("Couldn't connect to the camera!\n");
        return;
    }

    char desc[1000];
    GetStringParameter(cameraHandle, gspCameraDescription, 999, desc);
    printf("Camera description: %s\n", desc);

    CARDINAL major, minor, build;
    REAL valf;

    GetIntegerParameter(cameraHandle, gipFirmwareMajor, &major);
    GetIntegerParameter(cameraHandle, gipFirmwareMinor, &minor);
    GetIntegerParameter(cameraHandle, gipFirmwareBuild, &build);
    printf("Camera FW version: %u.%u.%u\n", major, minor, build);

    GetValue(cameraHandle, gvChipTemperature, &valf);
    printf("Camera chip temp: %.3f °C\n", valf);

    GetValue(cameraHandle, gvSupplyVoltage, &valf);
    printf("Camera supply voltage: %.3f V", valf);

    CARDINAL Index = 0;
    byte rv;
    do
    {
        rv = EnumerateReadModes(cameraHandle, Index, 999, desc);
        if (rv != 0)
        {
            printf("Read mode %u: %s\n", Index, desc);
            Index++;
        }
    } while (rv != 0);

    // set last read mode
    SetReadMode(cameraHandle, Index - 1);

    CARDINAL chipW, chipD;



    // Start exposure
    GetIntegerParameter(cameraHandle, gipChipW, &chipW);
    GetIntegerParameter(cameraHandle, gipChipD, &chipD);
    printf("Starting exposure of light frame with params:\nExp. time: %f sec\nWidth: %u px\nHeight: %u px\n", expTime, chipW, chipD);


    for (int img_num = 0; img_num < 10; img_num++)
    {
        StartExposure(cameraHandle, expTime, 1, 0, 0, chipW, chipD);

        // sleep during exposure
        Sleep((DWORD)expTime * 1000);

        BOOLEAN ready = 0;
        while (ready == 0)
        {
            ImageReady(cameraHandle, &ready);
        }

        // read whole frame
        CARDINAL imageLen = (chipW * chipD * 2);
        uint16_t* imageData = (uint16_t*)malloc(imageLen);

        rv = ReadImage(cameraHandle, imageLen, imageData);




        AddedCode(imageData, width, height);

        if (rv == 0)
        {
            printf("Error reading image!\n");
        }
        else
        {
            wchar_t filename[100];
            printf("Downloaded image size: %u bytes\n", imageLen);

            swprintf_s(filename, L"output_%3d.txt", img_num);
            SaveToTxt(filename, imageData, chipW, chipD);
            swprintf_s(filename, L"output_%3d.png", img_num);
            SaveToPNG(filename, imageData, chipW, chipD);
        }
    }
    // disconnect
    Release(cameraHandle);
    donePng();
}

int main(array<System::String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    pruebas2::window^ form = gcnew pruebas2::window();
    Application::Run(form);

    // After the form is closed, retrieve the values
    expTime = form->GetExpTime();
    width = form->GetWidth();
    height = form->GetHeight();
    ETH_ADAPTER_IP = form->GetETH();

    MessageBox::Show("Exposure Time: " + expTime.ToString() +
        "\nWidth: " + width.ToString() +
        "\nHeight: " + height.ToString() +
        "\nETH Adapter IP: " + gcnew System::String(ETH_ADAPTER_IP));

    return 0;
}