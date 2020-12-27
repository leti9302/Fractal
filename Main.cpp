#include"Coding.h"
#include"Decoding.h"

void main(int argc, char** argv) {
	try {
		InitializeMagick(*argv);
		fstream compress_result;
		Image image("lena512.bmp");
		image.resize("32x32!"); // в тестовых целях
		image.write("original.jpg");
		compress_result.open("Compress Image.txt", fstream::out);
		Coding(image, compress_result);
		compress_result.close();
		compress_result.open("Compress Image.txt", fstream::in);
		Decoding(compress_result);
		compress_result.close();
	}
	catch (Magick::Exception& error) {
		cerr << "Caught Magick++ exception: " << error.what() << endl;
	}
}