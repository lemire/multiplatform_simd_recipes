From VC:

template <class Iterator, class T>
inline Iterator find(Iterator first, Iterator last, const T &value)
{
    typename simdize<Iterator>::value_type value_v = value;
    for (simdize<Iterator> it = first; it < last; ++it) {
        const auto mask = *it == value_v;
        if (any_of(mask)) {
            return static_cast<Iterator>(it) + mask.firstOne();
        }
    }
    return last;
}

https://github.com/VcDevel/Vc/blob/master/examples/linear_find/main.cpp
