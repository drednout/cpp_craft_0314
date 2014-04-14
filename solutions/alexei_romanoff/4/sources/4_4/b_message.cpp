#include "b_message.h"

#include <stdexcept>
#include <cstring>
#include <sstream>

task4_4::b_message::b_message( std::istream& inp )
{
    length_ = 0;
    content_ = NULL;
	inp >> length_;
	if ( !inp.good() )
		throw std::logic_error("bad input stream, b_message length cannot be read");
    inp.seekg(1, std::ios_base::cur);
	content_ = new char[ length_ ];
    //valgrind pacifier
    memset(content_, 0, length_);

	inp.read( content_, length_ );
	if ( !inp.good() )
		throw std::logic_error("bad input stream, b_message cannot be read");
}

task4_4::message_ptr task4_4::b_message::create_message( std::istream& inp )
{
	return message_ptr( new b_message( inp ) );
}

task4_4::b_message::~b_message()
{
	delete [] content_;
}

const char task4_4::b_message::type() const
{
	return 'B';
}
const std::string task4_4::b_message::str() const
{
    std::ostringstream convert;
    convert << "b_message(";
    convert << length_ << "|";
	convert << std::string(content_, length_);
    convert << ")";
    return convert.str();
}

task4_4::b_message::b_message(b_message &msg)
{
    length_ = msg.length_;
    memcpy(content_, msg.content_, length_);
}

const task4_4::b_message& task4_4::b_message::operator=(task4_4::b_message& msg)
{
    length_ = msg.length_;
    memcpy(content_, msg.content_, length_);
    return *this;
}
