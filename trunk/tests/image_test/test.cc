/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsMisc.h>
#include <libdodo/graphicsImage.h>
#include <libdodo/graphicsTransform.h>
#include <libdodo/graphicsDraw.h>
#include <libdodo/ioFile.h>

#include <math.h>
#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

#ifdef IMAGEMAGICK_EXT

using namespace graphics;

#ifndef GRAPHICS_WO_XEXEC

void
hook(__xexecCollectedData *odata,
	 short int type,
	 void *udata)
{
	__xexecImageCollectedData *imData = (__xexecImageCollectedData *)odata;

	if (imData->operType == IMAGE_OPERATION_WRITE)
	{
		try
		{
			cout << "Rotating" << endl;

			image *img = dynamic_cast<image *>(imData->executor);
			graphics::transform tr(img);
			tr.rotate(TRANSFORM_ROTATEDIRECTIONANGLE_180);
		}
		catch (dodo::exception::basic ex)
		{
			cout << endl << ex.baseErrstr << endl << ex.line << endl << ex.baseErrno << endl;
		}
	}
}

#endif

#endif

int main(int argc, char **argv)
{
	try
	{
#ifdef IMAGEMAGICK_EXT
		image im;

		graphics::transform tr(&im);
		draw dr(&im);

#ifndef GRAPHICS_WO_XEXEC

		int prewrite = im.addPreExec(hook, NULL);

		///another one to revert
		int postwrite = im.addPostExec(hook, NULL);

#endif

		im.readFile("test.png");
		cout << im.getCompression() << " " << im.getEncoder() << " " << im.getQuality() << endl;

		tr.scale(1000, 1000);

		dr.circle(point(300, 300), 100, color::red, color::green, 5);
		dr.circle(point(300, 300), 50, color::blue, color::white, 5);

		im.writeFile("test.jpg");

#ifndef GRAPHICS_WO_XEXEC

		im.delPreExec(prewrite);
		im.delPostExec(postwrite);

#endif

		dodoString img;
		im.setEncoder(IMAGE_ENCODER_PNG);
		im.setCompression(IMAGE_COMPRESSION_ZIP);
		im.setQuality(4);
		im.setType(IMAGE_TYPE_GRAYSCALE);
		im.writeMemory(img);

		file::regular io;
		io.open("my.png", file::REGULAR_OPENMODE_READ_WRITE_TRUNCATE);
		io.outSize = img.size();
		io.write(img);

		cout << img.size() << endl;

		im.create(400, 400);
		dr.circle(point(200, 200), 100, color::red, color::green, 5);
		dr.circle(point(200, 200), 50, color::blue, color::white, 5);
		dr.rectangle(point(200, 200), point(300, 300), color::green, color::red, 15);
		im.writeFile("new.png");

		im.readFile("new.png");
		im.removeAlpha();
		im.writeFile("new-1.png");

		im.readFile("new-1.png");
		im.setBackgroundColor(color::transparent);
		dr.circle(point(200, 200), 100, color::red, color::green, 5);
		im.setType(IMAGE_TYPE_GRAYSCALE);
		dr.circle(point(200, 200), 50, color::blue, color::white, 5);
		im.setOpacity(65535/2);
		__color mygreen = color::green;
		mygreen.opacity = 65535/2;
		dr.circle(point(250, 250), 50, mygreen, color::white, 5);
		im.writeFile("new-2.png");
		
		im.create(400, 400);
		dr.circle(point(200, 200), 100, color::red, color::green, 5);
		dodoArray<point> points;
		for (int i=0;i<10;++i)
			points.push_back(point(i*20, (unsigned long)(390 - pow(i, 2)*5)));
		dr.line(points, color::black, 1);
		for (int i=0;i<10;++i)
			dr.point(point(i*20+100, (unsigned long)(390 - pow(i, 2)*5)), color::blue, 5);
		for (int i=0;i<360;++i)
			dr.point(point((unsigned long)(cos(i)*100 + 150), (unsigned long)(200 - sin(i)*100)), color::black);
		im.writeFile("new-3.png");
		
		im.create(400, 400);
		dr.circle(point(200, 200), 100, color::red, color::green, 5);
		dr.text(point(100, 200), "libdodo", "Arial", 70, color::blue, color::green);
		dr.text(point(100, 100), "libdodo", "Arial", 30);
		dr.text(point(150, 150), "libdodo", "Arial", 50, color::blue, color::green, 2, 180);
		dr.text(point(150, 200), "libdodo", "Arial", 50, color::blue, color::green, 2, 90);
		im.writeFile("new-4.png");

		image wm;
		wm.readFile("new-4.png");

		im.create(600, 600);
		dr.image(point(100, 100), wm, 45);
		im.setOpacity(65535/2);
		im.writeFile("new-5.png");
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << endl << ex.baseErrstr << endl << ex.line << endl << ex.baseErrno << endl << ex.getCallStack() <<  endl;
	}

	return 0;
}

