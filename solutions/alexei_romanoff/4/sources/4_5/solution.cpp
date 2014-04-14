#include <climits>

#include "solution.h"


task4_5::solution::solution( const data_type& data ) 
{
    thread_count_ = boost::thread::hardware_concurrency();
    min_value_ = 0;
    max_value_ = 0;
    data_ = data;
}

void task4_5::solution::calc_min(const data_container &data) const {
    for(data_container::const_iterator it = data.begin(); it != data.end(); ++it) {
        if (*it < min_value_) {
            boost::mutex::scoped_lock lock(min_lock_);
            min_value_ = *it;
        }
    }
}


void task4_5::solution::calc_max(const data_container &data) const {
    for(data_container::const_iterator it = data.begin(); it != data.end(); ++it) {
        if (*it > max_value_) {
            boost::mutex::scoped_lock lock(max_lock_);
            max_value_ = *it;
        }
    }
}

void task4_5::solution::do_thread_calculation(const task4_5::data_function df) const
{
    boost::thread_group thread_pool;
    thread_list tl;

    for(data_type::const_iterator it = data_.begin(); it != data_.end(); ++it) {
        boost::thread *cur_thread = thread_pool.create_thread(
                                        boost::bind( 
                                            df,
                                            this, *it
                                        ));
        tl.push_back(cur_thread);
        if (thread_pool.size() == thread_count_) {
            thread_pool.join_all();
            for(thread_list::const_iterator it = tl.begin(); it != tl.end(); ++it) {
                thread_pool.remove_thread(*it);
            }
            tl.clear();
        }
    }
    if (thread_pool.size() > 0) {
        thread_pool.join_all();
    }
}

int task4_5::solution::get_min() const
{
    if (data_.size() == 0) {
        return 0;
    }
    min_value_ = INT_MAX;
    do_thread_calculation(&task4_5::solution::calc_min);
    return min_value_;
}
int task4_5::solution::get_max() const
{
    if (data_.size() == 0) {
        return 0;
    }
    max_value_ = INT_MIN;
    do_thread_calculation(&task4_5::solution::calc_max);
    return max_value_;
}
