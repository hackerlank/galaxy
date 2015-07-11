#ifndef _NA_MSG_BASE_
#define _NA_MSG_BASE_
#include <memory>
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <string.h>
#include <commom.h>
#include "nedhelper.hpp"

#define MAX_MSG_LENGTH 8192
#define na_create_msg_begin(msg_class,msg_type)		\
struct msg_class : public na::msg::msg_base \
		{\
		msg_class()\
			{\
				memset(this,0,sizeof(msg_class));\
				_type = msg_type;\
			}\

#define na_create_msg_end	};

namespace na
{
	namespace msg
	{
enum{ json_str_offset = 12};
#pragma pack(push,1)
		struct msg_base
		{
			msg_base(short type = 0):_total_len(12),_type(type),_net_id(0),_player_id(0){}
			short _total_len;
			short _type;
			int	  _net_id;
			int	  _player_id;
		};
		struct msg_json : public msg_base
		{
			typedef	boost::shared_ptr<na::msg::msg_json>	ptr;
			const char*			_json_str_utf8;
			int _post_times;

			msg_json(){
				_player_id = -1;
				_net_id = -1;
				_type = -1;
				_total_len = json_str_offset;
				_post_times = 0;
				_json_str_utf8 = NULL;
			}

			//取header的头, 取data的数据
			msg_json(msg_base& header, msg_json& data)
			{
				new(this) msg_json();
				_player_id = header._player_id;
				_net_id = header._net_id;
				_type = header._type;
				_total_len = data._total_len;
				_json_str_utf8 = data._json_str_utf8;
			}
			
			msg_json(const short op_type,std::string& json_str_utf8)
			{
				new(this) msg_json();
				json_str_utf8 = commom_sys.unshell(json_str_utf8);
				_type = op_type;
				_total_len = json_str_utf8.size() + json_str_offset;
				_json_str_utf8 = json_str_utf8.data();
				_post_times = 0;
			}

			msg_json(const int player_id, const int net_id, const short op_type)
			{
				new(this) msg_json();
				_player_id = player_id;
				_net_id = net_id;
				_type = op_type;
			}

			msg_json(const int player_id, const int net_id, const short op_type, string& json_str_utf8)
			{
				 new(this) msg_json(op_type, json_str_utf8);
				_player_id = player_id;
				_net_id = net_id;
			}

			msg_json(const char* data_ptr)
			{
				new(this) msg_json();
				memcpy(this,data_ptr,json_str_offset);
				size_t str_size = _total_len - json_str_offset;
				char* json_ptr = (char*)this + sizeof(msg_json);
				memcpy(json_ptr,data_ptr+json_str_offset,str_size);
				_json_str_utf8 = json_ptr;
			}
			static void	destory(msg_json* p)
			{
				::GLMEMORY::GLDelete(p);
			}
			static ptr	create(const char* data_ptr,short len)
			{
				void* m = ::GLMEMORY::GLNew(len+sizeof(long) + sizeof(int));
				return ptr(new(m) msg_json(data_ptr),destory);
			}

			static ptr create(const int player_id, const int net_id, const short type, std::string str)
			{
				str = commom_sys.unshell(str);
				void* m = ::GLMEMORY::GLNew(sizeof(msg_json) + str.length());
				ptr p = ptr(new(m) msg_json(),destory);
				p->_total_len = json_str_offset + str.length();
				p->_player_id = player_id;
				p->_net_id = net_id;
				p->_type = type;
				char* dp = (char*)m + sizeof(msg_json);
				memcpy(dp, str.c_str(), str.length());
				p->_json_str_utf8 = dp;				
				return p; 
			}

			static ptr create(msg_json& mj)
			{
				void* m = ::GLMEMORY::GLNew(mj._total_len+sizeof(long) + sizeof(int));
				ptr p = ptr(new(m) msg_json(),destory);
				memcpy(m, &mj, sizeof(msg_base));
				char* data_ptr =  (char*)m + sizeof(msg_json);
				p->_json_str_utf8 = data_ptr;
				memcpy(data_ptr, mj._json_str_utf8, mj._total_len - sizeof(msg_base));
				p->_post_times = mj._post_times;
				return p;
			}
		};
#pragma pack(pop)
	}
}
#endif
