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
 *     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
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


#ifndef _DBENCH_H
#define _DBENCH_H

#include "DTime.h"
#include "DCommon.h"

    
#define BENCH(func, ...) \
{ \
      struct timeval t1,t2; \
      gettimeofday(&t1,0); \
      for (UINT i=0;i<BENCH_NRUNS;i++) \
	  func(__VA_ARGS__); \
      gettimeofday(&t2,0); \
      cout << #func << "\t" << displayTime(T_ELAPSED(t1, t2)/BENCH_NRUNS) << endl; \
}

#define BENCH_STR(func, str, ...) \
{ \
      struct timeval t1,t2; \
      gettimeofday(&t1,0); \
      for (UINT i=0;i<BENCH_NRUNS;i++) \
		func(__VA_ARGS__); \
      gettimeofday(&t2,0); \
      cout << #func << " " << str << "\t" << displayTime(T_ELAPSED(t1, t2)/BENCH_NRUNS) << endl; \
}

#define BENCH_IMG(func, ...) \
{ \
      struct timeval t1,t2; \
      gettimeofday(&t1,0); \
      for (UINT i=0;i<BENCH_NRUNS;i++) \
		func(__VA_ARGS__); \
      gettimeofday(&t2,0); \
      cout << #func << "\t" << GET_1ST_ARG(__VA_ARGS__).getTypeAsString() << "\t"; \
      cout << GET_1ST_ARG(__VA_ARGS__).getWidth() << "x" << GET_1ST_ARG(__VA_ARGS__).getHeight(); \
      if (GET_1ST_ARG(__VA_ARGS__).getDepth()>UINT(1)) cout << "x" << GET_1ST_ARG(__VA_ARGS__).getDepth(); \
      cout << "\t" << displayTime(T_ELAPSED(t1, t2)/BENCH_NRUNS) << endl; \
}

#define BENCH_IMG_STR(func, str, ...) \
{ \
      struct timeval t1,t2; \
      gettimeofday(&t1,0); \
      for (UINT i=0;i<BENCH_NRUNS;i++) \
		func(__VA_ARGS__); \
      gettimeofday(&t2,0); \
      cout << #func << " " << str << "\t" << GET_1ST_ARG(__VA_ARGS__).getTypeAsString() << "\t"; \
      cout << GET_1ST_ARG(__VA_ARGS__).getWidth() << "x" << GET_1ST_ARG(__VA_ARGS__).getHeight(); \
      if (GET_1ST_ARG(__VA_ARGS__).getDepth()>1) cout << "x" << GET_1ST_ARG(__VA_ARGS__).getDepth(); \
      cout << "\t" << displayTime(T_ELAPSED(t1, t2)/BENCH_NRUNS) << endl; \
}



#endif // _DBENCH_H

