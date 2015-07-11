#pragma once

#include <exception>
#include "game_helper.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
//#include <boost/unordered_map.hpp>
#include <map>
#include "json/json.h"
#include "mongoDB.h"
using namespace std;

namespace Params
{
	enum
	{
		Integer,
		Double,
		String,
		Array,
		Object,
	};

#define initialParamVar(Name) \
	static VarPtr Create(Json::Value& dataJson)\
	{\
		void* m = gg::GGNew(sizeof(Name));\
		return VarPtr(new(m) Name(dataJson), Var::Destory);\
	}\


	class Var;
	typedef boost::shared_ptr<Var> VarPtr;
	class Var
	{
	public:
		Var(const int t) : type(t){}
		virtual ~Var(){}
		static void Destory(Var* ptr){
			ptr->~Var();
			gg::GGDelete(ptr);
		}
		virtual int getType(){return type;}
		virtual unsigned getSize(){return 0;}
		virtual int asInt(){return 0;}
		virtual double asDouble(){return 0.0;}
		virtual string asString(){return string("");}
		virtual Var& operator[](string key){return *this;}
		virtual Var& operator[](int idx){return *this;}
		virtual bool isMember(string key){return false;}
		virtual bool isMember(int idx){return false;}
	protected:
		virtual void format(Json::Value& dataJson) = 0;
	private:
		const int type;
	};
	

	class IntegerValue : public Var
	{
	public:
		initialParamVar(IntegerValue)
		virtual ~IntegerValue(){}
		virtual int asInt(){return value;}
		virtual double asDouble()
		{
			return (double)value;
		}
	protected:
		virtual void format(Json::Value& dataJson);
	private:
		IntegerValue(Json::Value& dataJson) : Var(Integer)
		{
			value = 0;
			format(dataJson);
		}
		int value;
	};

	class DoubleValue : public Var
	{
	public:
		initialParamVar(DoubleValue)
		virtual ~DoubleValue(){}
		virtual int asInt()
		{
			return (int)value;
		}
		virtual double asDouble()
		{
			return value;
		}
	protected:
		virtual void format(Json::Value& dataJson);
	private:
		DoubleValue(Json::Value& dataJson) : Var(Double)
		{
			value = 0.0;
			format(dataJson);
		}
		double value;
	};

	class StringValue : public Var
	{
	public:
		initialParamVar(StringValue)
		virtual ~StringValue(){}
		virtual string asString()
		{
			return value;
		}
	protected:
		virtual void format(Json::Value& dataJson);
	private:
		StringValue(Json::Value& dataJson) : Var(String)
		{
			value = "";
			format(dataJson);
		}
		string value;
	};

	class ArrayValue : public Var
	{
	public:
		static ArrayValue EmptyValue;
		static mongo::BSONArray toBson(Var& arrayVar);
		static Json::Value toJson(Var& arrayVar);
		bool isEmpty()
		{
			return arrayVar.empty();
		}
		initialParamVar(ArrayValue)
		ArrayValue(Json::Value& dataJson) : Var(Array)
		{
			arrayVar.clear();
			format(dataJson);
		}
		ArrayValue() : Var(Array)
		{
			arrayVar.clear();
		}
		virtual ~ArrayValue(){}
		ArrayValue& operator=(ArrayValue& source)
		{
			arrayVar.clear();
			arrayVar = source.arrayVar;
			return *this;
		}
		virtual unsigned getSize()
		{
			return arrayVar.size();
		}
		virtual Var& operator[](int idx){
			if(idx < 0 || (unsigned)idx >= arrayVar.size())
				return *this;
			return *(arrayVar[idx].get());
		}
		virtual bool isMember(int idx)
		{
			return (idx >= 0 && (unsigned)idx < arrayVar.size());
		}
	protected:
		virtual void format(Json::Value& dataJson);
		typedef vector< VarPtr > ArrayVar;
		ArrayVar arrayVar;
	};

	class ObjectValue : public Var
	{
	public:
		static ObjectValue EmptyValue;
		static mongo::BSONObj toBson(Var& objVar);
		static Json::Value toJson(Var& objVar);
		bool isEmpty(){
			return mapVar.empty();
		}
		initialParamVar(ObjectValue)
		ObjectValue(Json::Value& dataJson) : Var(Object)
		{
			mapVar.clear();
			format(dataJson);
		}
		ObjectValue() : Var(Object)
		{
			mapVar.clear();
		}
		virtual ~ObjectValue(){}
		ObjectValue& operator=(ObjectValue& source)
		{
			mapVar.clear();
			mapVar = source.mapVar;
			return *this;
		}
		virtual unsigned getSize()
		{
			return mapVar.size();
		}
		virtual Var& operator[](string key){
			MapVar::iterator it = mapVar.find(key);
			if(it == mapVar.end())
				return *this;
			return *(it->second.get());
		}
		virtual bool isMember(string key)
		{
			return mapVar.find(key) != mapVar.end();
		}
	protected:
		virtual void format(Json::Value& dataJson);
		typedef map< string, VarPtr > MapVar;
		MapVar mapVar;
	};

}