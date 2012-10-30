/*
 * Smil
 * Copyright (c) 2010 Matthieu Faessel
 *
 * This file is part of Smil.
 *
 * Smil is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Smil is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Smil.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */


#include "DImage.h"
#include "DBase.h"
#include "DLabelMeasures.hpp"
#include "DBench.h"
#include "DTest.h"
#include "DMeasures.hpp"

class Test_MeasureBarycenter : public TestCase
{
  virtual void run()
  {
      Image_UINT8 im(256,256);
      
      UINT8 **lines = im.getLines();
      
      fill(im, UINT8(0));
      for (UINT j=10;j<60;j++)
	for (UINT i=20;i<70;i++)
	  lines[j][i] = 255;

      double xc, yc;
      double xcTruth = 44.5, ycTruth = 34.5;
      
      measBarycenter(im, &xc, &yc);
      
      TEST_ASSERT(xc==xcTruth);
      TEST_ASSERT(yc==ycTruth);
  }
};

class Test_LabelMeasures : public TestCase
{
  virtual void run()
  {
      Image_UINT8 im(256,256);
      
      UINT8 **lines = im.getLines();
      
      fill(im, UINT8(0));
      
      for (UINT j=10;j<60;j++)
	for (UINT i=20;i<70;i++)
	  lines[j][i] = 1;

      for (UINT j=100;j<170;j++)
	for (UINT i=80;i<150;i++)
	  lines[j][i] = 2;

      // Barycenters
	  
      double xc1Truth = 44.5, yc1Truth = 34.5;
      double xc2Truth = 114.5, yc2Truth = 134.5;
      
      map<UINT8, DoublePoint> bMap = measBarycenters(im);
      
      TEST_ASSERT(bMap[1].x==xc1Truth && bMap[1].y==yc1Truth);
      TEST_ASSERT(bMap[2].x==xc2Truth && bMap[2].y==yc2Truth);
      

      // Areas
      
      double area1Thruth = 2500, area2Thruth = 4900;
      map<UINT8, double> areas = measAreas(im);
      
      TEST_ASSERT(areas[1]==area1Thruth);
      TEST_ASSERT(areas[2]==area2Thruth);
  }
};

int main(int argc, char *argv[])
{
      TestSuite ts;

      ADD_TEST(ts, Test_MeasureBarycenter);
      ADD_TEST(ts, Test_LabelMeasures);
      
      Image_UINT8 im(512,512);
      measAreas(im);
      UINT BENCH_NRUNS = 1E3;
//       BENCH(measBarycenter, im, &xc, &yc);
//       BENCH(measBarycenters, im);
      
      return ts.run()==RES_OK;
}
