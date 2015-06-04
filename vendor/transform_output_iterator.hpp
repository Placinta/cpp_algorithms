// (C) Copyright 2013 Louis Dionne
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
// Taken from https://raw.githubusercontent.com/ldionne/boost-submissions/master/boost/boost/iterator/transform_output_iterator.hpp

#ifndef BOOST_ITERATOR_TRANSFORM_OUTPUT_ITERATOR_HPP
#define BOOST_ITERATOR_TRANSFORM_OUTPUT_ITERATOR_HPP

#include <boost/iterator/iterator_categories.hpp>
#include <boost/mpl/bool_fwd.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/utility/enable_if.hpp>


namespace boost {

template <typename Iterator> struct is_transform_output_iterator_type;

template <typename UnaryFunction_, typename Iterator>
class transform_output_iterator {
    // Use decay so we can accept function types.
    typedef typename boost::decay<UnaryFunction_>::type UnaryFunction;

    UnaryFunction f_;
    Iterator out_;

    template <typename UnaryFunc, typename Iter>
    class output_proxy {
        UnaryFunc& f_;
        Iter& out_;

    public:
        output_proxy(UnaryFunc& f, Iter& out)
            : f_(f), out_(out)
        { }

        template <typename T>
        output_proxy& operator=(T const& x) {
            *out_ = f_(x);
            return *this;
        }
    };

    // Apply the append metafunction to a given type. This is only required
    // for the eval_if construct below.
    template <typename It, typename F>
    struct apply_append {
        typedef typename It::template append<F>::type type;
    };

    // Metafunction computing the type of a transform_output_iterator applying
    // the G function after applying all the function it currently applies.
    template <typename G>
    struct append {
        typedef transform_output_iterator<UnaryFunction,
            typename mpl::eval_if<is_transform_output_iterator_type<Iterator>,
                apply_append<Iterator, G>,
                mpl::identity<transform_output_iterator<G, Iterator> >
            >::type
        > type;
    };

    // We need to befriend all types of transform_output_iterator because we
    // need to access the append_to_chain method of out_.
    template <typename UF, typename I>
    friend class transform_output_iterator;

    template <typename G>
        typename enable_if<is_transform_output_iterator_type<Iterator>,
    typename append<G>::type>::type append_to_chain(G const& g) const {
        return typename append<G>::type(f_, out_.append_to_chain(g));
    }

    template <typename G>
        typename disable_if<is_transform_output_iterator_type<Iterator>,
    typename append<G>::type>::type append_to_chain(G const& g) const {
        typedef transform_output_iterator<G, Iterator> last_type;
        typedef transform_output_iterator<UnaryFunction, last_type>
                                                            before_last_type;
        return before_last_type(f_, last_type(g, out_));
    }

public:
    typedef void value_type;
    typedef void reference;
    typedef void pointer;
    typedef void difference_type;
    typedef incrementable_traversal_tag iterator_category;

    template <typename G>
    typename append<G>::type and_then(G const& g = G()) const {
        return append_to_chain(g);
    }

    explicit transform_output_iterator(UnaryFunction const& f,
                                       Iterator const& iterator)
        : f_(f), out_(iterator)
    { }

    explicit transform_output_iterator(Iterator const& iterator)
        : out_(iterator)
    { }

    transform_output_iterator& operator++() {
        ++out_;
        return *this;
    }

    transform_output_iterator operator++(int) {
        transform_output_iterator tmp(*this);
        ++out_;
        return tmp;
    }

    output_proxy<UnaryFunction, Iterator> operator*() {
        return output_proxy<UnaryFunction, Iterator>(f_, out_);
    }

    output_proxy<UnaryFunction const, Iterator const> operator*() const {
        return output_proxy<UnaryFunction const, Iterator const>(f_, out_);
    }

    template <typename G>
    friend typename append<G>::type
                operator|(transform_output_iterator const& self, G const& g) {
        return self.append_to_chain(g);
    }
};

template <typename Iterator>
struct is_transform_output_iterator_type
    : mpl::false_
{ };

template <typename UnaryFunction, typename Iterator>
struct is_transform_output_iterator_type<
                        transform_output_iterator<UnaryFunction, Iterator> >
    : mpl::true_
{ };

template <typename Iterator>
bool is_transform_output_iterator(Iterator const&) {
    return ::boost::is_transform_output_iterator_type<Iterator>::value;
}

template <typename UnaryFunction, typename Iterator>
transform_output_iterator<UnaryFunction, Iterator>
make_transform_output_iterator(UnaryFunction const& f,
                               Iterator const& iterator) {
    return transform_output_iterator<UnaryFunction, Iterator>(f, iterator);
}

template <typename UnaryFunction, typename Iterator>
transform_output_iterator<UnaryFunction, Iterator>
make_transform_output_iterator(Iterator const& iterator) {
    return transform_output_iterator<UnaryFunction, Iterator>(iterator);
}

} // end namespace boost

#endif // !BOOST_ITERATOR_TRANSFORM_OUTPUT_ITERATOR_HPP