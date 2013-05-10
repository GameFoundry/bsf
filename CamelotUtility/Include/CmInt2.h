#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	struct CM_UTILITY_EXPORT Int2
	{
		int x;
		int y;

		inline Int2()
			: x(0), y(0)
		{
		}

		inline Int2(const int _x, const int _y )
            : x( _x ), y( _y )
        {
        }

        inline explicit Int2( const int scaler )
            : x( scaler), y( scaler )
        {
        }

        inline explicit Int2( const int afCoordinate[2] )
        {
            x = afCoordinate[0];
            y = afCoordinate[1];
        }

        inline explicit Int2( int* const r )
            : x( r[0] ), y( r[1] )
        {
        }

		/** Exchange the contents of this vector with another. 
		*/
		inline void swap(Int2& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
		}

		inline int operator [] ( const size_t i ) const
        {
            assert( i < 2 );

            return *(&x+i);
        }

		inline int& operator [] ( const size_t i )
        {
            assert( i < 2 );

            return *(&x+i);
        }

		/// Pointer accessor for direct copying
		inline int* ptr()
		{
			return &x;
		}
		/// Pointer accessor for direct copying
		inline const int* ptr() const
		{
			return &x;
		}

        /** Assigns the value of the other vector.
            @param
                rhs The other vector
        */
        inline Int2& operator = ( const Int2& rhs )
        {
            x = rhs.x;
            y = rhs.y;

            return *this;
        }

		inline Int2& operator = ( const int fScalar)
		{
			x = fScalar;
			y = fScalar;

			return *this;
		}

        inline bool operator == ( const Int2& rhs ) const
        {
            return ( x == rhs.x && y == rhs.y );
        }

        inline bool operator != ( const Int2& rhs ) const
        {
            return ( x != rhs.x || y != rhs.y  );
        }

        // arithmetic operations
        inline Int2 operator + ( const Int2& rhs ) const
        {
            return Int2(
                x + rhs.x,
                y + rhs.y);
        }

        inline Int2 operator - ( const Int2& rhs ) const
        {
            return Int2(
                x - rhs.x,
                y - rhs.y);
        }

        inline Int2 operator * ( const int fScalar ) const
        {
            return Int2(
                x * fScalar,
                y * fScalar);
        }

        inline Int2 operator * ( const Int2& rhs) const
        {
            return Int2(
                x * rhs.x,
                y * rhs.y);
        }

        inline Int2 operator / ( const int fScalar ) const
        {
            assert( fScalar != 0 );

            return Int2(
                x / fScalar,
                y / fScalar);
        }

        inline Int2 operator / ( const Int2& rhs) const
        {
            return Int2(
                x / rhs.x,
                y / rhs.y);
        }

        inline const Int2& operator + () const
        {
            return *this;
        }

        inline Int2 operator - () const
        {
            return Int2(-x, -y);
        }

        // overloaded operators to help Vector2
        inline friend Int2 operator * ( const int fScalar, const Int2& rhs )
        {
            return Int2(
                fScalar * rhs.x,
                fScalar * rhs.y);
        }

        inline friend Int2 operator / ( const int fScalar, const Int2& rhs )
        {
            return Int2(
                fScalar / rhs.x,
                fScalar / rhs.y);
        }

        // arithmetic updates
        inline Int2& operator += ( const Int2& rhs )
        {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        inline Int2& operator -= ( const Int2& rhs )
        {
            x -= rhs.x;
            y -= rhs.y;

            return *this;
        }

        inline Int2& operator *= ( const int fScalar )
        {
            x *= fScalar;
            y *= fScalar;

            return *this;
        }

        inline Int2& operator *= ( const Int2& rhs )
        {
            x *= rhs.x;
            y *= rhs.y;

            return *this;
        }

        inline Int2& operator /= ( const int fScalar )
        {
            assert( fScalar != 0 );

            x /= fScalar;
            y /= fScalar;

            return *this;
        }

        inline Int2& operator /= ( const Int2& rhs )
        {
            x /= rhs.x;
            y /= rhs.y;

            return *this;
        }
	};
}