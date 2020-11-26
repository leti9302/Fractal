#include <Magick++.h>
#include <iostream>
#include<sstream>

using namespace Magick;
using namespace std;

int main(int argc, char** argv) {
    try {
        InitializeMagick(*argv);
        Image img("lena512.bmp");
        Image img2("lena512.bmp");
        for (int i = 1; i < 9; i++)
        {
            if (i == 5)
                img2.flop();
            img2.rotate(90);
        }
        img2.flop();
        img2.write("lena512_2.bmp");
        ll; l;;

        //положение, координату, коэффициент
        //stringstream ss;
        //string str;
        //int hight, wight, count = 0;
        //wight = img.columns() / 4;
        //hight = img.rows() / 4;
        //ss << wight * 4 << "x" << hight * 4 << "!";
        //ss >> str;
        //img.resize(str);
        //stringstream().swap(ss);
        //for (hight = 0; hight < img.rows(); hight += 4)
        //{
        //    for (wight = 0; wight < img.columns(); wight += 4)
        //    {
        //        ss << "4x4+";
        //        ss << wight;
        //        ss << "+";
        //        ss << hight;
        //        ss >> str;
        //        count++;
        //        stringstream().swap(ss);
        //        //img2 = img;
        //        //img2.crop(str);
        //    }
        //}
        //Image img3("file.jpg");
        //img3.type(GrayscaleType);
        //ColorGray gr;
        //gr = img3.pixelColor(0, 0);
        //cout<<gr.shade()*255<<'\n';
        //cout << count << '\n';
        //img2.write("lena512_2.bmp");
        //system("pause");
    }
    catch (Magick::Exception& error) {
        cerr << "Caught Magick++ exception: " << error.what() << endl;
    }
    return 0;
}