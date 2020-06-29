/*
 * Copyright (c) 2011-2016, Matthieu FAESSEL and ARMINES
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _D_MEASURES_HPP
#define _D_MEASURES_HPP

#include "Core/include/private/DImage.hpp"
#include "DBaseMeasureOperations.hpp"
#include "DImageArith.hpp"
#include "Base/include/DImageDraw.h"

#include <map>
#include <set>
#include <iostream>
#include <iterator> // std::back_inserter

namespace smil
{
  /**
   * @ingroup Base
   * @defgroup Measures Base measures
   * @{
   */

  template <class T>
  struct measAreaFunc : public MeasureFunctionBase<T, double> {
    typedef typename Image<T>::lineType lineType;

    virtual void processSequence(lineType /*lineIn*/, size_t size)
    {
      this->retVal += size;
    }
  };

  /**
   * Area of an image
   *
   * Returns the number of non-zero pixels
   * @param[in] imIn Input image.
   */
  template <class T> size_t area(const Image<T> &imIn)
  {
    measAreaFunc<T> func;
    return static_cast<size_t>(func(imIn, true));
  }

  template <class T>
  struct measVolFunc : public MeasureFunctionBase<T, double> {
    typedef typename Image<T>::lineType lineType;

    virtual void processSequence(lineType lineIn, size_t size)
    {
      for (size_t i = 0; i < size; i++)
        this->retVal += double(lineIn[i]);
    }
  };

  /**
   * Volume of an image
   *
   * Returns the sum of the pixel values.
   * @param[in] imIn Input image.
   */
  template <class T> double vol(const Image<T> &imIn)
  {
    measVolFunc<T> func;
    return func(imIn, false);
  }

  template <class T>
  struct measMeanValFunc : public MeasureFunctionBase<T, Vector_double> {
    typedef typename Image<T>::lineType lineType;
    double sum1, sum2;
    double pixNbr;

    virtual void initialize(const Image<T> & /*imIn*/)
    {
      this->retVal.clear();
      sum1 = sum2 = pixNbr = 0.;
    }
    virtual void processSequence(lineType lineIn, size_t size)
    {
      double curV;
      for (size_t i = 0; i < size; i++) {
        pixNbr += 1;
        curV = lineIn[i];
        sum1 += curV;
        sum2 += curV * curV;
      }
    }
    virtual void finalize(const Image<T> & /*imIn*/)
    {
      double mean_val = pixNbr == 0 ? 0 : sum1 / pixNbr;
      double std_dev_val =
          pixNbr == 0 ? 0 : sqrt(sum2 / pixNbr - mean_val * mean_val);

      this->retVal.push_back(mean_val);
      this->retVal.push_back(std_dev_val);
    }
  };

  /**
   * Mean value and standard deviation
   *
   * Returns mean and standard deviation of the pixel values.
   * @param[in] imIn Input image.
   * @param[in] onlyNonZero If true, only non-zero pixels are considered.
   */
  template <class T>
  Vector_double meanVal(const Image<T> &imIn, bool onlyNonZero = false)
  {
    measMeanValFunc<T> func;
    return func(imIn, onlyNonZero);
  }

  template <class T> struct measMinValFunc : public MeasureFunctionBase<T, T> {
    typedef typename Image<T>::lineType lineType;
    virtual void initialize(const Image<T> & /*imIn*/)
    {
      this->retVal = numeric_limits<T>::max();
    }
    virtual void processSequence(lineType lineIn, size_t size)
    {
      for (size_t i = 0; i < size; i++)
        if (lineIn[i] < this->retVal)
          this->retVal = lineIn[i];
    }
  };

  template <class T>
  struct measMinValPosFunc : public MeasureFunctionWithPos<T, T> {
    typedef typename Image<T>::lineType lineType;
    Point<UINT> pt;
    virtual void initialize(const Image<T> & /*imIn*/)
    {
      this->retVal = numeric_limits<T>::max();
    }
    virtual void processSequence(lineType lineIn, size_t size, size_t x,
                                 size_t y, size_t z)
    {
      for (size_t i = 0; i < size; i++, x++)
        if (lineIn[i] < this->retVal) {
          this->retVal = lineIn[i];
          pt.x         = x;
          pt.y         = y;
          pt.z         = z;
        }
    }
  };

  /**
   * Min value of an image
   *
   * Returns the min of the pixel values.
   * @param[in] imIn Input image.
   * @param[in] onlyNonZero If true, only non-zero pixels are considered.
   */
  template <class T> T minVal(const Image<T> &imIn, bool onlyNonZero = false)
  {
    measMinValFunc<T> func;
    return func(imIn, onlyNonZero);
  }

  template <class T>
  T minVal(const Image<T> &imIn, Point<UINT> &pt, bool onlyNonZero = false)
  {
    measMinValPosFunc<T> func;
    func(imIn, onlyNonZero);
    pt = func.pt;
    return func.retVal;
  }

  template <class T> struct measMaxValFunc : public MeasureFunctionBase<T, T> {
    typedef typename Image<T>::lineType lineType;
    virtual void initialize(const Image<T> & /*imIn*/)
    {
      this->retVal = numeric_limits<T>::min();
    }
    virtual void processSequence(lineType lineIn, size_t size)
    {
      for (size_t i = 0; i < size; i++)
        if (lineIn[i] > this->retVal)
          this->retVal = lineIn[i];
    }
  };

  template <class T>
  struct measMaxValPosFunc : public MeasureFunctionWithPos<T, T> {
    typedef typename Image<T>::lineType lineType;
    Point<UINT> pt;
    virtual void initialize(const Image<T> & /*imIn*/)
    {
      this->retVal = numeric_limits<T>::min();
    }
    virtual void processSequence(lineType lineIn, size_t size, size_t x,
                                 size_t y, size_t z)
    {
      for (size_t i = 0; i < size; i++, x++)
        if (lineIn[i] > this->retVal) {
          this->retVal = lineIn[i];
          pt.x         = x;
          pt.y         = y;
          pt.z         = z;
        }
    }
  };

  /**
   * Max value of an image
   *
   * Returns the max of the pixel values.
   * @param[in] imIn Input image.
   * @param[in] onlyNonZero If true, only non-zero pixels are considered.
   */
  template <class T> T maxVal(const Image<T> &imIn, bool onlyNonZero = false)
  {
    measMaxValFunc<T> func;
    return func(imIn, onlyNonZero);
  }

  template <class T>
  T maxVal(const Image<T> &imIn, Point<UINT> &pt, bool onlyNonZero = false)
  {
    measMaxValPosFunc<T> func;
    func(imIn, onlyNonZero);
    pt = func.pt;
    return func.retVal;
  }

  template <class T>
  struct measMinMaxValFunc : public MeasureFunctionBase<T, vector<T>> {
    typedef typename Image<T>::lineType lineType;
    T minVal, maxVal;
    virtual void initialize(const Image<T> & /*imIn*/)
    {
      this->retVal.clear();
      maxVal = numeric_limits<T>::min();
      minVal = numeric_limits<T>::max();
    }
    virtual void processSequence(lineType lineIn, size_t size)
    {
      for (size_t i = 0; i < size; i++) {
        T val = lineIn[i];
        if (val > maxVal)
          maxVal = val;
        if (val < minVal)
          minVal = val;
      }
    }
    virtual void finalize(const Image<T> & /*imIn*/)
    {
      this->retVal.push_back(minVal);
      this->retVal.push_back(maxVal);
    }
  };

  /**
   * Min and Max values of an image
   *
   * Returns the min and the max of the pixel values.
   * @param[in] imIn Input image.
   * @param[in] onlyNonZero If true, only non-zero pixels are considered.
   */
  template <class T>
  vector<T> rangeVal(const Image<T> &imIn, bool onlyNonZero = false)
  {
    measMinMaxValFunc<T> func;
    return func(imIn, onlyNonZero);
  }

  template <class T>
  struct valueListFunc : public MeasureFunctionBase<T, vector<T>> {
    typedef typename Image<T>::lineType lineType;
    set<T> valList;

    virtual void initialize(const Image<T> & /*imIn*/)
    {
      this->retVal.clear();
      valList.clear();
    }

    virtual void processSequence(lineType lineIn, size_t size)
    {
      for (size_t i = 0; i < size; i++)
        valList.insert(lineIn[i]);
    }
    virtual void finalize(const Image<T> & /*imIn*/)
    {
      // Copy the content of the set into the ret vector
      std::copy(valList.begin(), valList.end(),
                std::back_inserter(this->retVal));
    }
  };

  /**
   * Get the list of the pixel values present in the image
   *
   * @see histogram
   */
  template <class T>
  vector<T> valueList(const Image<T> &imIn, bool onlyNonZero = true)
  {
    valueListFunc<T> func;
    return func(imIn, onlyNonZero);
  }
  template <class T> struct measModeValFunc : public MeasureFunctionBase<T, T> {
    typedef typename Image<T>::lineType lineType;

    map<int, int> nbList;
    int maxNb;
    T mode;
    virtual void initialize(const Image<T> & /*imIn*/)
    {
      // BMI            this->retVal.clear();
      nbList.clear();
      maxNb = 0;
      mode  = 0;
    }

    virtual void processSequence(lineType lineIn, size_t size)
    {
      for (size_t i = 0; i < size; i++) {
        T val = lineIn[i];
        if (val > 0) {
          if (nbList.find(val) == nbList.end()) {
            nbList.insert(std::pair<int, int>(val, 1));
          } else
            nbList[val]++;
          if (nbList[val] > maxNb) {
            mode  = val;
            maxNb = nbList[val];
          }
        } // if (val>0)
      }   // for i= 0; i < size
      this->retVal = mode;

    } // virtual
  };  // END measModeValFunc

  /**
   * Get the mode of the histogram present in the image, i.e. the
   * value that appears most often.
   */
  template <class T>
  T modeVal(const Image<T> &imIn, bool onlyNonZero = true)
  {
    measModeValFunc<T> func;
    return func(imIn, onlyNonZero);
  }


  template <class T>
  struct measMedianValFunc : public MeasureFunctionBase<T, T> {
    typedef typename Image<T>::lineType lineType;

    map<int, int> nbList;
    size_t acc_elem, total_elems;
    T medianval;
    virtual void initialize(const Image<T> & /*imIn*/)
    {
      // BMI            this->retVal.clear();
      nbList.clear();
      medianval   = 0;
      acc_elem    = 0;
      total_elems = 0;
    }

    virtual void processSequence(lineType lineIn, size_t size)
    {
      for (size_t i = 0; i < size; i++) {
        T val = lineIn[i];
        if (val > 0) {
          total_elems++;
          if (nbList.find(val) == nbList.end()) {
            nbList.insert(std::pair<int, int>(val, 1));
          } else
            nbList[val]++;

        } // if (val>0)
      }   // for i= 0; i < size
          //            this->retVal = medianval;
    }     // virtual processSequence
    
    virtual void finalize(const Image<T> & /*imIn*/)
    {
      typedef std::map<int, int>::iterator it_type;

      for (it_type my_iterator = nbList.begin(); my_iterator != nbList.end();
           my_iterator++) {
        acc_elem = acc_elem + my_iterator->second; //  nbList;
        std::cout << "iterator_values" << my_iterator->first
                  << my_iterator->second << "\n";
        if (acc_elem > total_elems / 2.0) {
          medianval = my_iterator->first;
          std::cout << "acc_elem=" << acc_elem << "medianval="
                    << "\n";
          break;
        }
        // iterator->first = key
        // iterator->second = value
      } // iterator

      this->retVal = medianval;
      // this->retVal.push_back(xSum/tSum);
    }

  }; // END measMedianValFunc

  /**
   * Get the median of the image histogram.
   */
  template <class T>
  T medianVal(const Image<T> &imIn, bool onlyNonZero = true)
  {
    measMedianValFunc<T> func;
    return func(imIn, onlyNonZero);
  }

  
  /**
   * Get image values along a profile.
   */
  template <class T>
  vector<T> profile(const Image<T> &im, size_t x0, size_t y0, size_t x1,
                    size_t y1, size_t z = 0)
  {
    vector<T> vec;
    ASSERT(im.isAllocated(), vec);

    size_t imW = im.getWidth();
    size_t imH = im.getHeight();

    vector<IntPoint> bPoints;
    if (x0 >= imW || y0 >= imH || x1 >= imW || y1 >= imH)
      bPoints = bresenhamPoints(x0, y0, x1, y1, imW, imH);
    else
      bPoints =
          bresenhamPoints(x0, y0, x1, y1); // no image range check (faster)

    typename Image<T>::sliceType lines = im.getSlices()[z];

    for (vector<IntPoint>::iterator it = bPoints.begin(); it != bPoints.end();
         it++)
      vec.push_back(lines[(*it).y][(*it).x]);

    return vec;
  }

  template <class T>
  struct measBarycenterFunc : public MeasureFunctionWithPos<T, Vector_double> {
    typedef typename Image<T>::lineType lineType;
    double xSum, ySum, zSum, tSum;
    virtual void initialize(const Image<T> & /*imIn*/)
    {
      this->retVal.clear();
      xSum = ySum = zSum = tSum = 0.;
    }
    virtual void processSequence(lineType lineIn, size_t size, size_t x,
                                 size_t y, size_t z)
    {
      for (size_t i = 0; i < size; i++, x++) {
        T pixVal = lineIn[i];
        xSum += double(pixVal) * x;
        ySum += double(pixVal) * y;
        zSum += double(pixVal) * z;
        tSum += double(pixVal);
      }
    }
    virtual void finalize(const Image<T> &imIn)
    {
      this->retVal.push_back(xSum / tSum);
      this->retVal.push_back(ySum / tSum);
      if (imIn.getDimension() == 3)
        this->retVal.push_back(zSum / tSum);
    }
  };

  template <class T> Vector_double measBarycenter(Image<T> &im)
  {
    measBarycenterFunc<T> func;
    return func(im, false);
  }

  template <class T>
  struct measBoundBoxFunc : public MeasureFunctionWithPos<T, vector<size_t>> {
    typedef typename Image<T>::lineType lineType;
    double xMin, xMax, yMin, yMax, zMin, zMax;
    bool im3d;
    virtual void initialize(const Image<T> &imIn)
    {
      this->retVal.clear();
      size_t imSize[3];
      imIn.getSize(imSize);
      im3d = (imSize[2] > 1);

      xMin = imSize[0];
      xMax = 0;
      yMin = imSize[1];
      yMax = 0;
      zMin = imSize[2];
      zMax = 0;
    }
    virtual void processSequence(lineType /*lineIn*/, size_t size, size_t x,
                                 size_t y, size_t z)
    {
      if (x < xMin)
        xMin = x;
      if (x + size - 1 > xMax)
        xMax = x + size - 1;
      if (y < yMin)
        yMin = y;
      if (y > yMax)
        yMax = y;
      if (im3d) {
        if (z < zMin)
          zMin = z;
        if (z > zMax)
          zMax = z;
      }
    }
    virtual void finalize(const Image<T> & /*imIn*/)
    {
      this->retVal.push_back(UINT(xMin));
      this->retVal.push_back(UINT(yMin));
      if (im3d)
        this->retVal.push_back(UINT(zMin));
      this->retVal.push_back(UINT(xMax));
      this->retVal.push_back(UINT(yMax));
      if (im3d)
        this->retVal.push_back(UINT(zMax));
    }
  };
  /**
   * Bounding Box measure
   *
   * @return xMin, yMin (,zMin), xMax, yMax (,zMax)
   */
  template <class T> vector<size_t> measBoundBox(Image<T> &im)
  {
    measBoundBoxFunc<T> func;
    return func(im, true);
  }

  template <class T>
  struct measImageMomentsFunc
      : public MeasureFunctionWithPos<T, Vector_double> {
    typedef typename Image<T>::lineType lineType;
    double m000, m100, m010, m110, m200, m020, m001, m101, m011, m002;
    bool im3d;
    virtual void initialize(const Image<T> &imIn)
    {
      im3d = (imIn.getDimension() == 3);
      this->retVal.clear();
      m000 = m100 = m010 = m110 = m200 = m020 = m001 = m101 = m011 = m002 = 0.;
    }
    virtual void processSequence(lineType lineIn, size_t size, size_t x,
                                 size_t y, size_t z)
    {
      for (size_t i = 0; i < size; i++, x++) {
        double pxVal = double(lineIn[i]);
        m000 += pxVal;
        m100 += pxVal * x;
        m010 += pxVal * y;
        m110 += pxVal * x * y;
        m200 += pxVal * x * x;
        m020 += pxVal * y * y;
        if (im3d) {
          m001 += pxVal * z;
          m101 += pxVal * x * z;
          m011 += pxVal * y * z;
          m002 += pxVal * z * z;
        }
      }
    }
    virtual void finalize(const Image<T> & /*imIn*/)
    {
      this->retVal.push_back(m000);
      this->retVal.push_back(m100);
      this->retVal.push_back(m010);
      if (im3d)
        this->retVal.push_back(m001);
      this->retVal.push_back(m110);
      if (im3d) {
        this->retVal.push_back(m101);
        this->retVal.push_back(m011);
      }
      this->retVal.push_back(m200);
      this->retVal.push_back(m020);
      if (im3d)
        this->retVal.push_back(m002);
    }
  };
  /**
   * Measure image moments
   *
   * @param[in] im : Input image
   * @param[in] onlyNonZero : use only non zero values
   * @return For 2D images: vector(m00, m10, m01, m11, m20, m02)
   * @return For 3D images: vector(m000, m100, m010, m001, m110, m101, m011, m200,
   * m020, m002)
   *
   * @see <a href="http://en.wikipedia.org/wiki/Image_moment">Image moment on
   * Wikipedia</a>
   *
   * @par Inertia matrix can be evaluated :
   * 
   *  @arg For @b 3D images :
   *    @f[
   *    M = 
   *      \begin{bmatrix}
   *        m020 + m002  & -m110 & -m101 \\
   *       -m110 & m200 + m002   & -m011 \\
   *       -m101 & -m011 &  m200 + m020 
   *      \end{bmatrix}
   *    @f]
   *    @arg For @b 2D images :   
   *    @f[
   *    M = 
   *      \begin{bmatrix}
   *        m20 & -m11 \\
   *       -m11 &  m20
   *      \end{bmatrix}
   *    @f]
   * 
   */
  template <class T>
  Vector_double measImageMoments(const Image<T> &im,
                                 const bool onlyNonZero = true)
  {
    measImageMomentsFunc<T> func;
    return func(im, onlyNonZero);
  }

  /**
   * Covariance between two images
   *
   * @param[in] imIn1, imIn2 : Input Images
   * @param[in] dx, dy, dz : direction
   * @param[in] maxSteps : number maximum of displacements to evaluate
   * @param[in] normalize : normalize result with respect to @b vec[0]
   * @return vec[h]
   *
   * The direction is given by @b dx, @b dy and @b dz.
   * The lenght corresponds to the max number of steps @b maxSteps. @b h are 
   * displacements in the direction defined by @b dx, @b dy and @b dz.
   *
   * @f[
   *    vec[h] = \sum_{p \:\in\: imIn1} \frac{imIn1(p) \;.\; imIn2(p + h)}{N_p}
   * @f]
   * @f$N_p@f$ is the number of pixels used in each term of the sum, which may 
   * different for each term in the sum.
   */
  template <class T>
  vector<double> measCovariance(const Image<T> &imIn1, const Image<T> &imIn2,
                                size_t dx, size_t dy, size_t dz,
                                size_t maxSteps = 0, bool normalize = false)
  {
    vector<double> vec;
    ASSERT(areAllocated(&imIn1, &imIn2, NULL), vec);
    ASSERT(haveSameSize(&imIn1, &imIn2, NULL),
           "Input images must have the same size", vec);
    ASSERT((dx + dy + dz > 0), 
           "dx, dy and dz can't be all zero at the same time", vec);

    size_t s[3];
    imIn1.getSize(s);
    
    size_t maxH = max(max(s[0], s[1]), s[2]);
    if (dx > 0)
      maxH = min(maxH, s[0]);
    if (dy > 0)
      maxH = min(maxH, s[1]);
    if (dz > 0)
      maxH = min(maxH, s[2]);
    if (maxH > 0)
      maxH--;

    if (maxSteps == 0)
      maxSteps = maxH;

    maxSteps = min(maxSteps, maxH);
    if (maxSteps == 0) {
      ERR_MSG("Too small");
      return vec;
    }

    vec.clear();

    typename ImDtTypes<T>::volType slicesIn1 = imIn1.getSlices();
    typename ImDtTypes<T>::volType slicesIn2 = imIn2.getSlices();
    typename ImDtTypes<T>::sliceType curSliceIn1;
    typename ImDtTypes<T>::sliceType curSliceIn2;
    typename ImDtTypes<T>::lineType lineIn1;
    typename ImDtTypes<T>::lineType lineIn2;
    typename ImDtTypes<T>::lineType bufLine = ImDtTypes<T>::createLine(s[0]);

    for (size_t len = 0; len <= maxSteps; len++) {
      double prod = 0;

      size_t mdx = min(dx * len, s[0] - 1);
      size_t mdy = min(dy * len, s[1] - 1);
      size_t mdz = min(dz * len, s[2] - 1);
      
      size_t xLen = s[0] - mdx;
      size_t yLen = s[1] - mdy;
      size_t zLen = s[2] - mdz;
      
      for (size_t z = 0; z < zLen; z++) {
        curSliceIn1 = slicesIn1[z];
        curSliceIn2 = slicesIn2[z + mdz];
        for (size_t y = 0; y < yLen; y++) {
          lineIn1 = curSliceIn1[y];
          lineIn2 = curSliceIn2[y + mdy];
          copyLine<T>(lineIn2 + mdx, xLen, bufLine);
          // Vectorized loop
          for (size_t x = 0; x < xLen; x++) {
            prod += lineIn1[x] * bufLine[x];
          }
        }
      }
      if (xLen * yLen * zLen != 0)
        prod /= (xLen * yLen * zLen);
      vec.push_back(prod);
    }

    if (normalize) {
      double orig = vec[0];
      for (vector<double>::iterator it = vec.begin(); it != vec.end(); it++)
        *it /= orig;
    }

    ImDtTypes<T>::deleteLine(bufLine);

    return vec;
  }

  /**
   * Auto-covariance
   *
   * The direction is given by @b dx, @b dy and @b dz.
   * The lenght corresponds to the max number of steps @b maxSteps
   */
  template <class T>
  vector<double> measAutoCovariance(const Image<T> &imIn, size_t dx, size_t dy,
                                size_t dz, size_t maxSteps = 0,
                                bool normalize = false)
  {
    return measCovariance(imIn, imIn, dx, dy, dz, maxSteps, normalize);
  }

  /**
   * Centered auto-covariance
   *
   * The direction is given by @b dx, @b dy and @b dz.
   * The lenght corresponds to the max number of steps @b maxSteps
   */
  template <class T>
  vector<double> measCenteredAutoCovariance(const Image<T> &imIn, size_t dx,
                                        size_t dy, size_t dz, size_t maxSteps = 0,
                                        bool normalize = false)
  {
    Image<float> imMean(imIn, true);
    float meanV = meanVal(imMean)[0];
    sub(imMean, meanV, imMean);
    return measAutoCovariance(imMean, dx, dy, dz, maxSteps, normalize);
  }

  /**
   * Centered covariance
   *
   * The direction is given by @b dx, @b dy and @b dz.
   * The lenght corresponds to the max number of steps @b maxSteps
   */
  template <class T>
  vector<double> measCenteredCovariance(const Image<T> &imIn1, const Image<T> &imIn2, size_t dx,
                                        size_t dy, size_t dz, size_t maxSteps = 0,
                                        bool normalize = false)
  {
    Image<float> imMean1(imIn1, true);
    float meanV1 = meanVal(imMean1)[0];
    sub(imMean1, meanV1, imMean1);

    Image<float> imMean2(imIn2, true);
    float meanV2 = meanVal(imMean2)[0];
    sub(imMean2, meanV2, imMean2);

    return measCovariance(imMean1, imMean2, dx, dy, dz, maxSteps, normalize);
  }

  /**
   * Non-zero point offsets.
   * Return a vector conatining the offset of all non-zero points in image.
   */
  template <class T> Vector_UINT nonZeroOffsets(Image<T> &imIn)
  {
    Vector_UINT offsets;

    ASSERT(CHECK_ALLOCATED(&imIn), RES_ERR_BAD_ALLOCATION, offsets);

    typename Image<T>::lineType pixels = imIn.getPixels();

    for (size_t i = 0; i < imIn.getPixelCount(); i++)
      if (pixels[i] != 0)
        offsets.push_back(i);

    return offsets;
  }

  /**
   * Test if an image is binary.
   * Return @b true if the only pixel values are ImDtTypes<T>::min() and
   * ImDtTypes<T>::max()
   */
  template <class T> bool isBinary(const Image<T> &imIn)
  {
    CHECK_ALLOCATED(&imIn);

    typename Image<T>::lineType pixels = imIn.getPixels();

    for (size_t i = 0; i < imIn.getPixelCount(); i++)
      if (pixels[i] != ImDtTypes<T>::min() && pixels[i] != ImDtTypes<T>::max())
        return false;

    return true;
  }

  /** @}*/

} // namespace smil

#endif // _D_MEASURES_HPP
