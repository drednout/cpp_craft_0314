#ifndef _TASK4_5_SOLUTION_H_
#define _TASK4_5_SOLUTION_H_

#include <vector>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace task4_5
{
    class solution;

	typedef std::vector< std::vector< int > > data_type;
    typedef std::vector< int > data_container;
    typedef std::list< boost::thread* > thread_list;
    typedef void (solution::* data_function)(const data_container &data) const;

	class solution
	{
	public:
		explicit solution( const data_type& data );
		int get_min() const;
		int get_max() const;
    private:
        unsigned int thread_count_;

        mutable int min_value_;
        mutable boost::mutex min_lock_;

        mutable int max_value_;
        mutable boost::mutex max_lock_;

        data_type data_;

        void calc_min(const data_container &data) const;
        void calc_max(const data_container &data) const;
        void do_thread_calculation(const data_function f) const;
	};

}

#endif // _TASK4_5_SOLUTION_H_

