#ifndef _DARROW_HPP_
#define _DARROW_HPP_

#include "Morpho/include/DMorpho.h"

namespace smil
{

    void __pause__ () {
        char garbage;
        cin.clear();
        cin.ignore (cin.rdbuf()->in_avail());
        cout << "\n*********Press any key to continue**********" << flush;
        cin.get(garbage);
        cin.clear();
        cin.ignore(cin.rdbuf()->in_avail());
    }

    template <class arrowT, class statutT, class outT, class containerType = STD_Queue<size_t> >
    class arrowPropagate {

        protected:
            containerType q;

            virtual bool testAndAssign (statutT &pS, outT &pO) 
            {
                if (pO != propagationValue) 
                {
                   // pS = numeric_limits<statutT>::max();
                    pO = propagationValue;
                    return true;
                }
                return false;
            }

        public:
            outT propagationValue;

            typedef Image<arrowT> arrowIT;
            typedef Image<statutT> statutIT;
            typedef Image<outT>  outIT;
            typedef typename arrowIT::lineType arrowLT;
            typedef typename statutIT::lineType statutLT;
            typedef typename outIT::lineType outLT;

            arrowPropagate () {}
            ~arrowPropagate () {}

            RES_T _exec ( const Image<arrowT> &imArrow, Image<statutT> &imStatut, Image<outT> &imOut, const StrElt &se, const size_t &offset ) 
            {
                arrowLT arrowP = imArrow.getPixels ();
                statutLT statutP = imStatut.getPixels ();
                outLT outP = imOut.getPixels ();

                bool oddLine;
                size_t size[3]; imArrow.getSize (size);
                UINT sePtsNumber = se.points.size ();

                size_t x, x0, y, y0, z, z0;
                arrowT arrow;

                size_t o, nb_o;

                q.push (offset);
                do
                {
                    o = q.front();
                    q.pop();
                    z0 = o / (size[1] * size[0]);
                    y0 = (o % (size[1]*size[0])) / size[0];
                    x0 = o % size[0];
                    oddLine = se.odd && y0%2;

                    for (UINT p=0; p<sePtsNumber; ++p)
                    {
                        arrow = (1UL << p);
                        if (arrowP[o] & arrow) 
                        {
                            x = x0 + se.points[p].x;
                            y = y0 + se.points[p].y;
                            z = z0 + se.points[p].z;
                            if (oddLine)
                                x += (y+1)%2;
                            nb_o = x + y*size[0] + z*size[1]*size[0];
        
                            if (testAndAssign (statutP[nb_o], outP[nb_o]))
                                q.push(nb_o);
                       }
                    }
                       
                } while (!q.empty());
                return RES_OK;
            }
            inline RES_T operator ()( const Image<arrowT> &imArrow, Image<statutT> &imStatut, Image<outT> &imOut, const StrElt &se, const size_t &offset ) 
            {
                return _exec (imArrow, imStatut, imOut, se, offset);
            }
            inline RES_T operator ()(const Image<arrowT> &imArrow, Image<outT> &imOut, const StrElt &se, const size_t &offset) 
            {
                return _exec (imArrow, imOut, imOut, se, offset);
            }
    };

    template <class arrowT, class statutT, class outT, class containerType = STD_Queue<size_t> >
    class arrowPropagateWithConflicts : public arrowPropagate<arrowT, statutT, outT, containerType> 
    {
        private :
            statutT conflictValue;
            virtual bool testAndAssign (statutT &pS, outT &pO) 
            {
                if ( pS == statutT(0) )
                {
                    if (pO == outT(0))
                    {
                        pO = this->propagationValue;
                        return true;
                    } else if (pO != this->propagationValue) 
                    {
                        pS = conflictValue;
                        pO = outT(0);
                        return true;
                    }
                }
                return false;
            }
        public :
            arrowPropagateWithConflicts (const statutT &_cV ) : conflictValue (_cV), arrowPropagate<arrowT, statutT, outT, containerType>() 
            {}
    };

    template <class arrowT, class statutT, class outT>
    class arrowDroplet : public arrowPropagate<arrowT, statutT, outT, STD_Stack<size_t> >
    {
        private:
            virtual bool  testAndAssign (statutT &pS, outT &pO)
            {
            }
        public:
            arrowDroplet () : arrowPropagate<arrowT, statutT, outT, STD_Stack<size_t> >()
            {} 
    };


    template < class T1,
	class T2 > struct equSupLine3:public tertiaryLineFunctionBase < T1 >
    {
	equSupLine3 (  ):trueVal ( ImDtTypes < T2 >::max (  ) ),
	    falseVal ( 0 )
	{
	}

	typedef typename Image < T1 >::lineType lineType1;
	typedef typename Image < T2 >::lineType lineType2;

	T2 trueVal, falseVal;

	inline void operator      (  ) ( const lineType1 lIn1,
					 const lineType1 lIn2,
					 const size_t size, lineType2 lOut )
	{
	    return _exec ( lIn1, lIn2, size, lOut );
	}
	inline void _exec ( const lineType1 lIn1,
			    const lineType1 lIn2,
			    const size_t size, lineType2 lOut )
	{
	    T2 _trueVal ( trueVal ), _falseVal ( falseVal );

	    for ( size_t i = 0; i < size; i++ )
	      {
		  lOut[i] |= ( lIn1[i] == lIn2[i] ) ? _trueVal : _falseVal;
	      }
	}
    };

  template < class T, class arrowT = UINT8 > class arrowMinFunction:public unaryMorphImageFunctionBase < T, arrowT >
    {
      public:
	typedef unaryMorphImageFunctionBase < T, arrowT > parentClass;
	typedef typename parentClass::imageInType imageInType;
	typedef typename imageInType::lineType lineInType;
	typedef typename imageInType::lineType sliceInType;
	typedef typename imageInType::volType volInType;
	typedef typename parentClass::imageOutType imageArrowType;
	typedef typename imageArrowType::lineType lineArrowType;
	typedef typename imageArrowType::sliceType sliceArrowType;
	typedef typename imageArrowType::volType volArrowType;

      arrowMinFunction ( T border = numeric_limits < T >::max (  ) ):borderValue ( border ), unaryMorphImageFunctionBase < T,
	    arrowT >
	    (  )
	{
	}

	virtual RES_T _exec ( const imageInType & imIn,
			      imageArrowType & imOut, const StrElt & se );
	inline void extract_translated_line ( const imageInType & imIn,
					      const int &x,
					      const int &y,
					      const int &z,
					      lineInType outBuf )
	{
	    if ( z < 0 || z >= int ( imIn.getDepth (  ) ) || y < 0
		 || y >= int ( imIn.getHeight (  ) ) )
		copyLine < T > ( borderBuf, lineLen, outBuf );
	    else
		shiftLine < T > ( imIn.getSlices (  )[z][y], x, lineLen, outBuf,
				  borderValue );
	}
      private:
	T borderValue;
	lineInType borderBuf, cpBuf, minBuf, flagBuf, nullBuf;
	size_t lineLen;

	lowLine < T > low;
	infLine < T > inf;
	testLine < T, arrowT > test;
	equSupLine3 < T, arrowT > equSup;
	equLine < T > equ;
    };

    template < class T, class arrowT >
	RES_T arrowMinFunction < T, arrowT >::_exec ( const imageInType & imIn,
						      imageArrowType & arrow,
						      const StrElt & se )
    {
	ASSERT_ALLOCATED ( &imIn, &arrow );
	ASSERT_SAME_SIZE ( &imIn, &arrow );

	if ( !areAllocated ( &imIn, &arrow, NULL ) )
	    return RES_ERR_BAD_ALLOCATION;

	StrElt cpSe = se.noCenter ();

	UINT sePtsNumber = cpSe.points.size (  );

	if ( sePtsNumber == 0 )
	    return RES_OK;


	size_t nSlices = imIn.getSliceCount (  );
	size_t nLines = imIn.getHeight (  );

	lineLen = imIn.getWidth (  );

	volInType srcSlices = imIn.getSlices (  );
	volArrowType destSlices = arrow.getSlices (  );
	lineInType *srcLines;
	lineArrowType *destLines;

	bool oddSe = cpSe.odd, oddLine = 0;
	size_t x, y, z;

	borderBuf = ImDtTypes < T >::createLine ( lineLen );
	cpBuf = ImDtTypes < T >::createLine ( lineLen );
	minBuf = ImDtTypes < T >::createLine ( lineLen );
	nullBuf = ImDtTypes < T >::createLine ( lineLen );
	flagBuf = ImDtTypes < T >::createLine ( lineLen );

	fillLine < T > ( nullBuf, lineLen, arrowT ( 0 ) );
	fillLine < T > ( borderBuf, lineLen, T ( borderValue ) );

	equ.trueVal = 0;
	equ.falseVal = numeric_limits < T >::max (  );
	for ( size_t s = 0; s < nSlices; ++s )
	  {
	      srcLines = srcSlices[s];
	      destLines = destSlices[s];

	      for ( size_t l = 0; l < nLines; ++l )
		{
	        oddLine = oddSe && l %2;

		    lineInType lineIn = srcLines[l];
		    lineArrowType lineArrow = destLines[l];

		    fillLine < arrowT > ( lineArrow, lineLen, arrowT ( 0 ) );
		    copyLine < T > ( lineIn, lineLen, minBuf );

		    for ( UINT p = 0; p < sePtsNumber; ++p )
		      {
			  y = l + cpSe.points[p].y;
			  x = -cpSe.points[p].x - (oddLine && (y+1)%2);
			  z = s + cpSe.points[p].z;

			  equSup.trueVal = ( 1UL << p );

			  extract_translated_line ( imIn, x, y, z, cpBuf );

			  low._exec ( cpBuf, minBuf, lineLen, flagBuf );
			  inf._exec ( cpBuf, minBuf, lineLen, minBuf );
			  test._exec ( flagBuf, nullBuf, lineArrow, lineLen,
				       lineArrow );
			  equSup._exec ( cpBuf, minBuf, lineLen, lineArrow );
			  equ._exec ( lineIn, minBuf, lineLen, flagBuf );
			  test._exec ( flagBuf, lineArrow, nullBuf, lineLen,
				       lineArrow );
		      }
		}
	  }
    };

    template < class T > RES_T arrowMin ( const Image < T > &imIn,
							Image < T >
							&arrow,
							const StrElt & se,
							T borderValue =
							numeric_limits <
							T >::max (  ) )
    {
	arrowMinFunction < T, T > iFunc ( borderValue );
	return iFunc ( imIn, arrow, se );
    }
}

#endif // _DARROW_HPP_
