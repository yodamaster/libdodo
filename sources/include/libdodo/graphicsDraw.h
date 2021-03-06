/***************************************************************************
 *            graphicsDraw.h
 *
 *  Thu Nov 23 2007
 *  Copyright  2007  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _GRAPHICSDRAW_H_
#define _GRAPHICSDRAW_H_ 1

#include <libdodo/directives.h>

#ifdef IMAGEMAGICK_EXT
#include <libdodo/types.h>
#include <libdodo/graphicsColor.h>

namespace dodo {
    namespace graphics {
        class image;

        /**
         * class point
         * @brief defines x and y position of the point
         */
        class point {
          public:

            /**
             * constructor
             * @param x defines x position of the point
             * @param y defines y position of the point
             */
            point(unsigned long x,
                  unsigned long y);

            unsigned long x;    ///< x position of the point
            unsigned long y;    ///< y position of the point
        };

        /**
         * @class draw
         * @brief provides simple image drawing
         * @note for (x;y) == (0;0) it's left upper corner
         */
        class draw {
          public:

            /**
             * draw circle
             * @param image defines image on which perform drawing
             * @param center defines x and y position of circle center
             * @param radius defines circle radius
             * @param fillColor defines circle fill color
             * @param borderColor defines circle border color
             * @param borderWidth defines the width of the border of the circle
             */
            static void circle(graphics::image       &image,
                               const graphics::point &center,
                               unsigned long         radius,
                               const __color__       &fillColor,
                               const __color__       &borderColor,
                               unsigned short        borderWidth = 1);

            /**
             * draw rectangle
             * @param image defines image on which perform drawing
             * @param tl defines x and y position of rectangle top left corner
             * @param br defines x and y position of rectangle bottom right corner
             * @param fillColor defines rectangle fill color
             * @param borderColor defines rectangle border color
             * @param borderWidth defines the width of the border of the rectangle
             */
            static void rectangle(graphics::image       &image,
                                  const graphics::point &tl,
                                  const graphics::point &br,
                                  const __color__       &fillColor,
                                  const __color__       &borderColor,
                                  unsigned short        borderWidth = 1);

            /**
             * draw text
             * @param image defines image on which perform drawing
             * @param position defines x and y position of text top left corner
             * @param text defines text to render
             * @param font defines font of the text
             * @param fontWidth defines font width
             * @param fillColor defines text fill color
             * @param borderColor defines text border color
             * @param borderWidth defines the width of the border of the text
             * @param angle defines the deflection angle in degrees of the text relatively to horison
             */
            static void text(graphics::image       &image,
                             const graphics::point &position,
                             const dodo::string      &text,
                             const dodo::string      &font,
                             unsigned short        fontWidth,
                             const __color__       &fillColor = color::black,
                             const __color__       &borderColor = color::black,
                             unsigned short        borderWidth = 1,
                             double                angle = 0);

            /**
             * put image
             * @param image defines image on which perform drawing
             * @param position defines x and y position of text top left corner
             * @param im defines image to render
             * @param angle defines the deflection angle in degrees of the image relatively to horison
             */
            static void image(graphics::image       &image,
                              const graphics::point &position,
                              const graphics::image &im,
                              double                angle = 0);

            /**
             * draw line
             * @param image defines image on which perform drawing
             * @param points defines vector of points of the line
             * @param lineColor defines line color
             * @param lineWidth defines the width of the line
             */
            static void line(graphics::image                  &image,
                             const dodoArray<graphics::point> &points,
                             const __color__                  &lineColor,
                             unsigned short                   lineWidth = 1);

            /**
             * draw dot
             * @param image defines image on which perform drawing
             * @param position defines coordinate of the point
             * @param pointColor defines poiny color
             * @param pointWidth defines the width of the point
             * @note for point width > 1 point is emulated as a circle, it's possible to get some side-effects
             */
            static void point(graphics::image       &image,
                              const graphics::point &position,
                              const __color__       &pointColor,
                              unsigned short        pointWidth = 1);

          protected:

            /**
             * draw primitive by description
             * @param image defines image on which perform drawing
             * @param description defines primitive description
             * @param fillColor defines primitive fill color
             * @param borderColor defines primitive border color
             * @param borderWidth defines the width of the border of the primitive
             */
            static void primitive(graphics::image &image,
                                  char            *description,
                                  const __color__ &fillColor,
                                  const __color__ &borderColor,
                                  unsigned short  borderWidth);
        };
    };
};
#endif
#endif
