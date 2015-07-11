#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <iostream>
#include "json/json.h"
#include "game_helper.hpp"
#include <boost/shared_ptr.hpp>
using namespace std;

//#pragma message("==========warning : may lose some data with use spXXX==========")

namespace gg
{
	typedef boost::shared_ptr<bool> VarWorkPtr;
	//ͬ������ָ��
	typedef boost::function<void()> updateNotice;
	template<typename Type>
	class basic_value
	{
	public:
		typedef Type _mType;
		typedef boost::function<void(_mType*)> deleteNotice;
		typedef boost::function<void(_mType, _mType)> typeNotice;
		explicit basic_value(VarWorkPtr listen = VarWorkPtr(), updateNotice uN = NULL, typeNotice tN = NULL, deleteNotice dN = NULL) :
			_Work(listen), _uNotice(uN), _tNotice(tN), _dNotice(dN)
		{
			_Ref = new(gg::GGNew(sizeof(unsigned))) unsigned(1);
			_mVal = new(gg::GGNew(sizeof(_mType))) _mType();
		}
		explicit basic_value(_mType val, VarWorkPtr listen = VarWorkPtr(), updateNotice uN = NULL, typeNotice tN = NULL, deleteNotice dN = NULL) :
			_Work(listen), _uNotice(uN), _tNotice(tN), _dNotice(dN)
		{
			_Ref = new(gg::GGNew(sizeof(unsigned))) unsigned(1);
			_mVal = new(gg::GGNew(sizeof(_mType))) _mType(val);
		}
		explicit basic_value(const basic_value& source):
			_Work(source._Work), _mVal(source._mVal), _uNotice(source._uNotice), _tNotice(source._tNotice), _Ref(source._Ref)
		{
			++*_Ref;
		}
		virtual ~basic_value()
		{
			--*_Ref;
			if (*_Ref == 0)
			{
				if ( isListen() && _dNotice != NULL )
				{
					_dNotice(_mVal);//������ʧ
				}
				gg::GGDelete(_Ref);
				gg::GGDelete(_mVal);
			}
		}
		inline _mType Value()
		{
			return Own();
		}
		inline const _mType& operator()()
		{
			return Own();
		}
		basic_value& rawset(_mType value)
		{
			Own() = value;
			return *this;
		}
		basic_value& AddHandler(VarWorkPtr listen = VarWorkPtr())
		{
			_Work = listen;
			return *this;
		}
		basic_value& AddDataUpdate(updateNotice uN = NULL)
		{
			_uNotice = uN;
			return *this;
		}
		basic_value& AddTypeUpdate(typeNotice tN = NULL)
		{
			_tNotice = tN;
			return *this;
		}
		basic_value& AddDeleteUpdate(deleteNotice dN = NULL)
		{
			_dNotice = dN;
			return *this;
		}
		basic_value& operator++()
		{
			_mType old(Own());
			++Own();
			onUpdate(old);
			return *this;
		}
		template<typename uType>
		basic_value& operator=(uType value)
		{
			_mType old(Own());
			Own() = _mType(value);
			onUpdate(old);
			return *this;
		}
		template<typename uType>
		basic_value& operator+=(uType value)
		{
			_mType old(Own());
			Own() += _mType(value);
			onUpdate(old);
			return *this;
		}
		template<typename uType>
		basic_value& operator-=(uType value)
		{
			_mType old(Own());
			Own() -= _mType(value);
			onUpdate(old);
			return *this;
		}
		template<typename uType>
		basic_value& operator/=(uType value)
		{
			_mType old(Own());
			Own() /= _mType(value);
			onUpdate(old);
			return *this;
		}
		template<typename uType>
		basic_value& operator*=(uType value)
		{
			_mType old(Own());
			Own() *= _mType(value);
			onUpdate(old);
			return *this;
		}
		template<typename uType>
		bool operator>(const uType value)
		{
			return Own() > value;
		}
		template<typename uType>
		bool operator>=(const uType value)
		{
			return Own() >= value;
		}
		template<typename uType>
		bool operator<(const uType value)
		{
			return Own() < value;
		}
		template<typename uType>
		bool operator<=(const uType value)
		{
			return Own() <= value;
		}
		template<typename uType>
		bool operator==(const uType value)
		{
			return Own() == value;
		}
		template<typename uType>
		bool operator!=(const uType value)
		{
			return Own() != value;
		}
		//Json�ȴ���������ʹ���ⷽ����������Ч��
		basic_value& constructData(basic_value& source)
		{
			_mType old(Own());
			Own() = source.Own();
			onUpdate(old);
			return *this;
		}
	protected:
		inline _mType& Own()
		{
			return *_mVal;
		}
	private:
		bool isListen()
		{
			if (_Work == NULL)return false;
			return *_Work;
		}
		void onUpdate(_mType oVal)
		{
			if (!isListen())return;

			//�������ݱ仯�Ƿ�������
			if (_tNotice != NULL)
			{
				_tNotice(oVal, Own());
			}

			//���������ݱ仯,����֪��˭�仯��,��ô�仯
			if (_uNotice != NULL)
			{
				_uNotice();
			}
		}
		VarWorkPtr _Work;
		unsigned* _Ref;
		_mType* _mVal;
		updateNotice _uNotice;
		typeNotice _tNotice;
		deleteNotice _dNotice;
	};
	
	typedef basic_value<int> spInt;
	typedef basic_value<unsigned> spUInt;
	typedef basic_value<double> spDouble;
	typedef basic_value<bool> spBool;
	typedef basic_value<string> spStr;
	typedef basic_value<Json::Value> spJson;
}