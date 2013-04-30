/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000-2002 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

OSG_BEGIN_NAMESPACE

#if defined(OSG_LINUX_ICC) && !defined(OSG_ICC_GNU_COMPAT)
#    define MYFIRST _Myfirst
#    define MYLAST  _Mylast
#    define MYEND   _Myend
#elif defined(OSG_SGI_CC) 
#    define MYFIRST _M_start
#    define MYLAST  _M_finish
#    define MYEND   _M_end_of_storage
#elif defined(OSG_LINUX_GCC)
#    if (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || __GNUC__ >= 4
#        define MYFIRST _M_impl._M_start
#        define MYLAST  _M_impl._M_finish
#        define MYEND   _M_impl._M_end_of_storage
#    else
#        define MYFIRST _M_start
#        define MYLAST  _M_finish
#        define MYEND   _M_end_of_storage
#    endif
#else
#    define MYFIRST _Myfirst
#    define MYLAST  _Mylast
#    define MYEND   _Myend
#endif

#if defined(__sgi) || defined(__linux) || defined(darwin) || \
    defined(__sun) || defined(__hpux)


template <class Tp, class Alloc> inline
MFieldVector<Tp, Alloc>::MFieldVector(const allocator_type& __a) :
    Inherited(__a) 
{
}

template <class Tp, class Alloc> inline
MFieldVector<Tp, Alloc>::MFieldVector(      size_type       __n, 
                                      const Tp             &__value,
                                      const allocator_type &__a    ) :
    Inherited(__n, __value, __a)
{
}
    
template <class Tp, class Alloc> inline
MFieldVector<Tp, Alloc>::MFieldVector(size_type __n) :
    Inherited(__n)
{
}
    
template <class Tp, class Alloc> inline
MFieldVector<Tp, Alloc>::MFieldVector(const std::vector<Tp, Alloc>& __x) :
    Inherited(__x)
{
}

template <class Tp, class Alloc> inline
MFieldVector<Tp, Alloc>::MFieldVector(const MFieldVector<Tp, Alloc>& __x) :
    Inherited(__x)
{
}

#ifdef __STL_MEMBER_TEMPLATES
  // Check whether it's an integral type.  If so, it's not an iterator.
template <class Tp, class Alloc> 
template <class InputIterator> inline
MFieldVector<Tp, Alloc>::MFieldVector(      InputIterator   __first, 
                                            InputIterator   __last,
                                      const allocator_type &__a    ) :
    Inherited(__first, __last, __a) 
{
}

#else

template <class Tp, class Alloc> inline
MFieldVector<Tp, Alloc>::MFieldVector(const Tp             *__first, 
                                      const Tp             *__last,
                                      const allocator_type &__a    ) :
    Inherited(__first, __last, __a) 
{
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class Tp, class Alloc> inline
MFieldVector<Tp, Alloc>::~MFieldVector() 
{
}

template <class Tp, class Alloc> inline
void MFieldVector<Tp, Alloc>::shareValues(Self &other, bool bDeleteOld)
{
    if(bDeleteOld == true)
    {
#if (defined(OSG_LINUX_ICC) && !defined(OSG_ICC_GNU_COMPAT)) || defined(__clang__)
        this->_Destroy(this->MYFIRST, this->MYLAST);

        this->_Alval.deallocate(this->MYFIRST,
                                this->MYEND - this->MYFIRST); 
#else
        std::_Destroy(this->MYFIRST, this->MYLAST);

        this->_M_deallocate(this->MYFIRST,
                            this->MYEND - this->MYFIRST); 
#endif
    }

    this->MYFIRST = other.MYFIRST; 
    this->MYLAST  = other.MYLAST;
    this->MYEND   = other.MYEND;
}

template <class Tp, class Alloc> inline
void MFieldVector<Tp, Alloc>::resolveShare(void)
{
    this->MYFIRST = NULL;
    this->MYLAST  = NULL;
    this->MYEND   = NULL;
}

#elif defined(WIN32)

template<class Ty, class A> inline
MFieldVector<Ty, A>::MFieldVector(const A &_Al) : 
    Inherited(_Al) 
{
}

template<class Ty, class A> inline
MFieldVector<Ty, A>::MFieldVector(      size_type  _N, 
                                  const Ty        &_V,
                                  const A         &_Al) : 
    Inherited(_N, _V, _Al)
{
}

template<class Ty, class A> inline
MFieldVector<Ty, A>::MFieldVector(const MFieldVector<Ty, A> &_X) :
    Inherited(_X)
{
}


template<class Ty, class A> inline
MFieldVector<Ty, A>::MFieldVector(      It  _F, 
                                        It  _L, 
                                  const A  &_Al) :
    Inherited(_F, _L, _Al)
{
}

template<class Ty, class A> inline
MFieldVector<Ty, A>::~MFieldVector()
{
}

template<class Ty, class A> inline
void MFieldVector<Ty, A>::shareValues(Self &other, bool bDeleteOld)
{
    if(bDeleteOld == true)
    {
        delete this->MYFIRST;
    }

    this->MYFIRST = other.MYFIRST; 
    this->MYLAST  = other.MYLAST;
    this->MYEND   = other.MYEND;
}

template <class Tp, class A> inline
void MFieldVector<Tp, A>::resolveShare(void)
{
    this->MYFIRST = NULL;
    this->MYLAST  = NULL;
    this->MYEND   = NULL;
}

#if defined(OSG_WIN32_CL) && defined(OSG_FIXED_MFIELDSYNC)
template<> inline
void MFieldVector<bool, std::allocator<bool> >::shareValues(
    Self &other, 
    bool  bDeleteOld)
{
    fprintf(stderr, "MFieldVector<_Bool, _Alloc>::shareValues not impl\n");
}

template <> inline
void MFieldVector<bool, std::allocator<bool> >::resolveShare(void)
{
    fprintf(stderr, "MFieldVector<_Bool, _Alloc>::resolveShare not impl\n");
}
#endif

#endif

OSG_END_NAMESPACE

#define OSGMFIELDVECTOR_INLINE_CVSID "@(#)$Id: $"

