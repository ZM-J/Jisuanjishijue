//
//  main.cpp
//  Canny Edge Detector
//
//  Created by Hasan Akgün on 21/03/14.
//  Copyright (c) 2014 Hasan Akgün. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <vector>
#include "CImg.h"
#include "canny.h"

using namespace std;
using namespace cimg_library;

int main()
{
    string filePath = "lena.bmp"; //Filepath of input image
    canny cny(filePath);
        
    return 0;
}

