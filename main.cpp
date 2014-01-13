#include <iostream>

// #include <boost/range/adaptor/tokenized.hpp>
#include <boost/range/any_range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/join.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>


    namespace range_detail
    {

        template<class R>
        class joiner
        {
        public:
            typedef typename R::iterator::value_type value_type;
            void operator()(R& r) { _result = boost::join(_result, r); }
            boost::any_range<value_type,
            //boost::bidirectional_traversal_tag,
            typename boost::iterator_traversal<typename R::iterator>::type,
            value_type, std::ptrdiff_t>
            result()
            { return _result; }
        private:
            boost::any_range<value_type,
            //boost::bidirectional_traversal_tag,
            typename boost::iterator_traversal<typename R::iterator>::type,
            value_type, std::ptrdiff_t> _result;
        };

        template< class R>
        struct dyn_joined_range :
            public boost::any_range<
              typename R::iterator::value_type::iterator::value_type
            //, boost::bidirectional_traversal_tag
            , typename boost::iterator_traversal<typename R::iterator::value_type::iterator>::type
            //, typename boost::detail::iterator_traits<typename R::iterator>::iterator_category
            , typename R::iterator::value_type::iterator::value_type
            , std::ptrdiff_t>
            //          boost::range_iterator<
            //            BOOST_DEDUCED_TYPENAME range_iterator<V>::type
            //                                  >
            //                             >
        {
        private:
            typedef typename R::iterator::value_type::iterator::value_type value_type;
            typedef boost::any_range<value_type
            , typename boost::iterator_traversal<typename R::iterator::value_type::iterator>::type,
                    //,boost::bidirectional_traversal_tag,
                    value_type,
                    std::ptrdiff_t>
            base;
            // typedef boost::iterator_range<
            //           boost::range_iterator<
            //             BOOST_DEDUCED_TYPENAME range_iterator<V>::type
            //                                   >
            //                              >
            //     base;

        public:
            //typedef boost::range_iterator<BOOST_DEDUCED_TYPENAME range_iterator<V>::type> iterator;

            explicit dyn_joined_range( R& r )
                //: base( iterator(boost::end(r)), iterator(boost::begin(r)) )
                //: base( std::for_each(boost::begin(r), boost::end(r), joiner<V>()).result())
                :base(
                    std::for_each(boost::begin(r), boost::end(r),
                        joiner<typename R::iterator::value_type>()).result() )
            {
            // std::for_each(r.begin(), r.end(),
            //         joiner<typename std::iterator_traits<typename R::iterator>::value_type, V>());
            }
            // template<class Iterator>
            dyn_joined_range(typename R::iterator& first, typename R::iterator& last)
                : base(
                    std::for_each(first, last,
                        joiner<typename R::iterator::value_type>()).result() )
            {
            }
        };

        struct join_forwarder {};

        template< class BidirectionalRng >
        inline dyn_joined_range<BidirectionalRng>
        operator|( BidirectionalRng& r, join_forwarder )
        {
            return dyn_joined_range<BidirectionalRng>( r );
        }

        template< class BidirectionalRng >
        inline dyn_joined_range<const BidirectionalRng>
        operator|( const BidirectionalRng& r, join_forwarder )
        {
            return dyn_joined_range<const BidirectionalRng>( r );
        }

    } // 'range_detail'

    using range_detail::dyn_joined_range;

    namespace adaptors
    {
        namespace
        {
            const range_detail::join_forwarder dyn_joined =
                                            range_detail::join_forwarder();
        }

        template<class BidirectionalRange>
        inline dyn_joined_range<BidirectionalRange>
        dyn_join(BidirectionalRange& rng)
        {
            return dyn_joined_range<BidirectionalRange>(rng);
        }

        template<class BidirectionalRange>
        inline dyn_joined_range<const BidirectionalRange>
        dyn_join(const BidirectionalRange& rng)
        {
            return dyn_joined_range<const BidirectionalRange>(rng);
        }
    } // 'adaptors'
#include <boost/range/adaptor/type_erased.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
# include <boost/detail/iterator.hpp>
# include <boost/iterator/iterator_categories.hpp>
int main() {
    std::vector<int> vec {1,2,3,3,3,4,4,3,5,6};
    boost::copy( vec | boost::adaptors::reversed,
            std::ostream_iterator<int>(std::cout) );
    std::cout << std::endl;
    for (auto elt : boost::join(vec, vec)) {
        std::cout << elt << ",";
    }
    std::cout << std::endl;

    std::vector<std::vector<int>> vv{{1,2,3},{42, 4,5,6}, {7,8,9, 42, 42, 41, 42, 42}};
    //boost::iterator_range<BOOST_DEDUCED_TYPENAME range_iterator<std::vector<int> >::type>
    boost::any_range<int, boost::forward_traversal_tag, int, std::ptrdiff_t> vr;
    //boost::joined_range<std::vector<int>, std::vecto<int>>
    // vr
    //     = boost::join(boost::make_iterator_range(vv[0].begin(), vv[0].end()),
    //             boost::join(vv[1], vv[2]));
    for (auto& v : vv)
    {
        auto tmp = boost::join(vr, v);
        vr = tmp;
    }
    for (auto v : vr)
    {
        std::cout << v << ", ";
    }
    std::cout << std::endl;
    // boost::copy( vv | boost::adaptors::tokenized(boost::regex("\\w+")),
    //         std::ostream_iterator<std::vector<int>>(std::cout) );
    // std::cout << std::endl;

    // dyn_joined_range<std::vector<std::vector<int>>> jr(vv);
    for (int v :
            // jr)
            // vv | boost::adaptors::reversed | adaptors::dyn_joined)
             vv | boost::adaptors::reversed | adaptors::dyn_joined | boost::adaptors::uniqued )
            //  vv | adaptors::dyn_joined | boost::adaptors::copied(1,5))
            //  vv | boost::adaptors::uniqued | boost::adaptors::copied(1,5))
            // std::for_each(boost::begin(vv), boost::end(vv),
                                        // joiner<std::vector<int>>()).result())
                                     // [](std::vector<int>& v) { for (auto e : v) { std::cout << e << ", "; } });

    {
        std::cout << v << ", ";
    }
    std::cout << std::endl;
    return 0;
}
