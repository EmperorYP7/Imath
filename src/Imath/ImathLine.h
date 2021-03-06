///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2012, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_IMATHLINE_H
#define INCLUDED_IMATHLINE_H

//-------------------------------------
//
//	A 3D line class template
//
//-------------------------------------

#include "ImathLimits.h"
#include "ImathMatrix.h"
#include "ImathNamespace.h"
#include "ImathVec.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

template <class T> class Line3
{
  public:
    Vec3<T> pos;
    Vec3<T> dir;

    //-------------------------------------------------------------
    //	Constructors - default is normalized units along direction
    //-------------------------------------------------------------

    IMATH_HOSTDEVICE constexpr Line3() noexcept {}
    IMATH_HOSTDEVICE IMATH_CONSTEXPR14 Line3 (const Vec3<T>& point1, const Vec3<T>& point2) noexcept;

    //------------------
    //	State Query/Set
    //------------------

    IMATH_HOSTDEVICE void set (const Vec3<T>& point1, const Vec3<T>& point2) noexcept;

    //-------
    //	F(t)
    //-------

    IMATH_HOSTDEVICE constexpr Vec3<T> operator() (T parameter) const noexcept;

    //---------
    //	Query
    //---------

    IMATH_HOSTDEVICE constexpr T distanceTo (const Vec3<T>& point) const noexcept;
    IMATH_HOSTDEVICE IMATH_CONSTEXPR14 T distanceTo (const Line3<T>& line) const noexcept;
    IMATH_HOSTDEVICE constexpr Vec3<T> closestPointTo (const Vec3<T>& point) const noexcept;
    IMATH_HOSTDEVICE IMATH_CONSTEXPR14 Vec3<T> closestPointTo (const Line3<T>& line) const noexcept;
};

//--------------------
// Convenient typedefs
//--------------------

typedef Line3<float> Line3f;
typedef Line3<double> Line3d;

//---------------
// Implementation
//---------------

template <class T> IMATH_CONSTEXPR14 inline Line3<T>::Line3 (const Vec3<T>& p0, const Vec3<T>& p1) noexcept
{
    set (p0, p1);
}

template <class T>
inline void
Line3<T>::set (const Vec3<T>& p0, const Vec3<T>& p1) noexcept
{
    pos = p0;
    dir = p1 - p0;
    dir.normalize();
}

template <class T>
constexpr inline Vec3<T>
Line3<T>::operator() (T parameter) const noexcept
{
    return pos + dir * parameter;
}

template <class T>
constexpr inline T
Line3<T>::distanceTo (const Vec3<T>& point) const noexcept
{
    return (closestPointTo (point) - point).length();
}

template <class T>
constexpr inline Vec3<T>
Line3<T>::closestPointTo (const Vec3<T>& point) const noexcept
{
    return ((point - pos) ^ dir) * dir + pos;
}

template <class T>
IMATH_CONSTEXPR14 inline T
Line3<T>::distanceTo (const Line3<T>& line) const noexcept
{
    T d = (dir % line.dir) ^ (line.pos - pos);
    return (d >= 0) ? d : -d;
}

template <class T>
IMATH_CONSTEXPR14 inline Vec3<T>
Line3<T>::closestPointTo (const Line3<T>& line) const noexcept
{
    // Assumes the lines are normalized

    Vec3<T> posLpos = pos - line.pos;
    T c             = dir ^ posLpos;
    T a             = line.dir ^ dir;
    T f             = line.dir ^ posLpos;
    T num           = c - a * f;

    T denom = a * a - 1;

    T absDenom = ((denom >= 0) ? denom : -denom);

    if (absDenom < 1)
    {
        T absNum = ((num >= 0) ? num : -num);

        if (absNum >= absDenom * limits<T>::max())
            return pos;
    }

    return pos + dir * (num / denom);
}

template <class T>
std::ostream&
operator<< (std::ostream& o, const Line3<T>& line)
{
    return o << "(" << line.pos << ", " << line.dir << ")";
}

template <class S, class T>
constexpr inline Line3<S>
operator* (const Line3<S>& line, const Matrix44<T>& M) noexcept
{
    return Line3<S> (line.pos * M, (line.pos + line.dir) * M);
}

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#endif // INCLUDED_IMATHLINE_H
