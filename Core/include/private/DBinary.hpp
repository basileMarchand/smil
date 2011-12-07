/*
 * Copyright (c) 2011, Matthieu FAESSEL and ARMINES
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
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _bitArray_HPP
#define _bitArray_HPP

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

template <class T>
struct bitIndex
{
    T *byte;
    unsigned int index;
    
    bitIndex& operator=(bool val)
    {
      if (val)
	(*byte) |= (1<<index);
      else
	(*byte) &= ~(1<<index);
      return *this;
    }
    operator bool()
    {
      return (*byte) & (1<<index);
    }
};

template <class T>
struct bitArray
{
    bitArray(int v = 0) : val(v) {}
    bitArray(bool b) : val(b ? ~0 : 0) {}
    T val;
    
    static const UINT64 SIZE = sizeof(T)*CHAR_BIT;
    
    //! Most significant bit
    static const T MS_BIT = (1 << (SIZE - 2));
    
    typedef T Type;

    inline bitIndex<T>& operator[] (UINT8 pos)
    {
	static bitIndex<T> b;
	b.byte = &val;
	b.index = pos;
 	return b;
    }
    ostream& printSelf(ostream &os=cout)
    {
	for (int i=0;i<SIZE;i++)
	  os << this->operator[](i) << " ";
	return os;
    }
    inline bitArray& operator=(T v)
    {
	val = v;
	return *this;
    }
    inline bitArray& operator=(bool b)
    {
	val = b ? ~0 : 0;
	return *this;
    }
    inline bitArray& operator=(const char* s)
    {
	UINT iMax = strlen(s) < SIZE ? strlen(s) : SIZE;
	
	val = 0;
	for (int i=0;i<iMax;i++)
	  val += (s[i]-48) * (1<<i);
	return *this;
    }
    inline operator bool()
    {
	return val!=0;
    }
};

template <class T>
inline ostream& operator << (ostream &os, bitArray<T> &b)
{
    return b.printSelf(os);
}

#define BIN bitArray<UINT32>




// 
// inline RES_T setPixel(UINT x, UINT y, UINT z, pixelType value)
// {
//     if (x>=width || y>=height || z>=depth)
// 	return RES_ERR;
//     pixels[z*width*height+y*width+x] = value;
//     modified();
//     return RES_OK;
// }
// inline RES_T setPixel(UINT x, UINT y, pixelType value)
// {
//     if (x>=width || y>=height)
// 	return RES_ERR;
//     pixels[height+y*width+x] = value;
//     modified();
//     return RES_OK;
// }
// inline RES_T setPixel(UINT offset, pixelType value)
// {
//     if (offset >= pixelCount)
// 	return RES_ERR;
//     pixels[offset] = value;
//     modified();
//     return RES_OK;
// }



#endif // _bitArray_HPP