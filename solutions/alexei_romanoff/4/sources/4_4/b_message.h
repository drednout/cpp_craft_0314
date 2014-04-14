#ifndef _TASK4_4_B_MESSAGE_H_
#define _TASK4_4_B_MESSAGE_H_

#include "message.h"

namespace task4_4
{

	class b_message : public message
	{
		size_t length_;
		char *content_;

		explicit b_message( std::istream& inp );
	public:
		static message_ptr create_message( std::istream& inp );
        explicit b_message(b_message &msg);
        const b_message& operator=(b_message& msg);
		~b_message();

		const char type() const;
		const std::string str() const;
	};

}

#endif // _TASK4_4_A_MESSAGE_H_

