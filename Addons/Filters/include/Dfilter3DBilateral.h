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

#ifndef _D3DBILATERALFILTER_H_
#define _D3DBILATERALFILTER_H_

namespace smil
{
  /**
   * @ingroup AddonFilters
   * @defgroup Addon3DBilateral 3D Bilateral Filter
   *
   * Fast 3D bilateral filter implementation
   *
   * * Possible problems with very large images or images of type UINT32 and
   * bigger
   * * Works only with images of integer type.
   * * lacking documentation about parameters
   *
   * @author Theodore Chabardes
   * @{
   * */

  /**
   * @brief recursiveBilateralFilter 3D Bilateral Filter
   * @param[in]  imIn : input Image
   * @param[in]  sigmaW :
   * @param[in]  sigmaR :
   * @param[out] imOut : output Image
   */
  template <class T>
  RES_T recursiveBilateralFilter(const Image<T> &imIn, float sigmaW,
                                 float sigmaR, Image<T> &imOut);

  /** @} */
} // namespace smil

#include "private/filter3DBilateral/filter3DBilateral.hpp"

#endif // _D3DBILATERALFILTER_H_