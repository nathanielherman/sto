/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "set/hdr_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_unordered_set.h>
#include <cds/intrusive/striped_set.h>

namespace set {
    namespace bi = boost::intrusive;

    namespace {
        typedef IntrusiveStripedSetHdrTest::base_item< bi::unordered_set_base_hook<> > base_item_type;
        typedef IntrusiveStripedSetHdrTest::member_item< bi::unordered_set_member_hook<> > member_item_type;

        struct hasher: private IntrusiveStripedSetHdrTest::hash_int
        {
            typedef IntrusiveStripedSetHdrTest::hash_int base_class;

            size_t operator()( int i ) const
            {
                return ~( base_class::operator()(i));
            }
            template <typename Item>
            size_t operator()( const Item& i ) const
            {
                return ~( base_class::operator()(i));
            }
            size_t operator()( IntrusiveStripedSetHdrTest::find_key const& i) const
            {
                return ~( base_class::operator()(i));
            }
        };

        template <typename T>
        struct is_equal: private IntrusiveStripedSetHdrTest::cmp<T>
        {
            typedef IntrusiveStripedSetHdrTest::cmp<T> base_class;

            bool operator ()(const T& v1, const T& v2 ) const
            {
                return base_class::operator()( v1, v2 ) == 0;
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return base_class::operator()( v1, v2 ) == 0;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return base_class::operator()( v1, v2 ) == 0;
            }
        };

        template <size_t Capacity, typename T, class Alloc = CDS_DEFAULT_ALLOCATOR>
        struct dyn_buffer: public co::v::dynamic_buffer< T, Alloc >
        {
            typedef co::v::dynamic_buffer< T, Alloc >   base_class;
        public:
            template <typename Q>
            struct rebind {
                typedef dyn_buffer<Capacity, Q, Alloc> other   ;  ///< Rebinding result type
            };

            dyn_buffer()
                : base_class( Capacity )
            {}
        };
    }

    void IntrusiveStripedSetHdrTest::Striped_unordered_set_basehook()
    {
        typedef ci::StripedSet<
            bi::unordered_set<base_item_type
                , bi::hash< hasher >
                , bi::equal< is_equal<base_item_type> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Striped_unordered_set_basehook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::unordered_set<base_item_type
                , bi::hash< hasher >
                , bi::equal< is_equal<base_item_type> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::buffer< co::v::static_buffer< cds::any_type, 64 > >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Striped_unordered_set_basehook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::unordered_set<base_item_type
                , bi::hash< hasher >
                , bi::equal< is_equal<base_item_type> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::buffer< dyn_buffer< 256, cds::any_type, std::allocator<int> > >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(512) );
        test_with( s );
    }

    void IntrusiveStripedSetHdrTest::Striped_unordered_set_memberhook()
    {
        typedef ci::StripedSet<
            bi::unordered_set<member_item_type
                , bi::member_hook< member_item_type, bi::unordered_set_member_hook<>, &member_item_type::hMember>
                , bi::hash< hasher >
                , bi::equal< is_equal<member_item_type> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Striped_unordered_set_memberhook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::unordered_set<member_item_type
                , bi::member_hook< member_item_type, bi::unordered_set_member_hook<>, &member_item_type::hMember>
                , bi::hash< hasher >
                , bi::equal< is_equal<member_item_type> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::buffer< dyn_buffer< 64, cds::any_type, std::allocator<int> > >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
            ,co::mutex_policy< ci::striped_set::striping<> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Striped_unordered_set_memberhook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::unordered_set<member_item_type
                , bi::member_hook< member_item_type, bi::unordered_set_member_hook<>, &member_item_type::hMember>
                , bi::hash< hasher >
                , bi::equal< is_equal<member_item_type> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::buffer< co::v::static_buffer< cds::any_type, 128 > >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }

} // namespace set


