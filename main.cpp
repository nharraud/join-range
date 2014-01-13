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
            typedef typename boost::range_value<R>::type value_type;
            typedef typename boost::range_iterator<R>::type iterator_type;
            void operator()(R& r) { _result = boost::join(_result, r); }
            boost::any_range<value_type,
            typename boost::iterator_traversal<iterator_type>::type,
            value_type, std::ptrdiff_t>
            result()
            { return _result; }
        private:
            boost::any_range<value_type,
            typename boost::iterator_traversal<iterator_type>::type,
            value_type, std::ptrdiff_t> _result;
        };

        template< class R>
        struct dyn_joined_range :
            public boost::any_range<
                          typename boost::range_value<typename boost::range_value<R>::type>::type
            , typename boost::iterator_traversal<
                //typename R::iterator::value_type::iterator
              typename boost::range_iterator<typename boost::range_value<R>::type>::type
              >::type
            , typename boost::range_value<typename boost::range_value<R>::type>::type
            , std::ptrdiff_t>
        {
        private:
            typedef typename boost::range_value<R>::type first_level_value_type;
            typedef typename boost::range_iterator<R>::type first_level_iterator_type;
            typedef typename boost::range_value<typename boost::range_value<R>::type>::type second_level_value_type;
            typedef typename boost::range_iterator<typename boost::range_value<R>::type>::type second_level_iterator_type;
            typedef boost::any_range<second_level_value_type
            , typename boost::iterator_traversal<second_level_iterator_type>::type,
                    second_level_value_type,
                    std::ptrdiff_t>
            base;

        public:

            explicit dyn_joined_range( R& r )
                :base(
                    std::for_each(boost::begin(r), boost::end(r),
                        joiner<first_level_value_type>()).result() )
            {
            }
            dyn_joined_range(first_level_iterator_type& first, first_level_iterator_type& last)
                : base(
                    std::for_each(first, last,
                        joiner<first_level_value_type>()).result() )
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
             vv
             | boost::adaptors::transformed([](std::vector<int>& a) -> std::vector<int>& { a[0] = 51; return a; })
             //|  boost::adaptors::reversed
             //| boost::adaptors::filtered([](std::vector<int>& v) {return v[0] != 1;})
             | adaptors::dyn_joined
             //| boost::adaptors::uniqued
             )
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
