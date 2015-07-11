#include "params.hpp"

namespace Params
{
	static Json::Value NullJsonValue;
	ObjectValue ObjectValue::EmptyValue =  ObjectValue(NullJsonValue);
	ArrayValue ArrayValue::EmptyValue = ArrayValue(NullJsonValue);

	void IntegerValue::format(Json::Value& dataJson)
	{
		if(dataJson.isInt())
			value = dataJson.asInt();
		else if(dataJson.isDouble())
			value = (int)dataJson.asDouble();
	}

	void DoubleValue::format(Json::Value& dataJson)
	{
		if(dataJson.isDouble())
			value = dataJson.asDouble();
		else if(dataJson.isInt())
			value = (double)dataJson.asInt();
	}

	void StringValue::format(Json::Value& dataJson)
	{
		if(dataJson.isString())
			value = dataJson.asString();
	}

	Json::Value ArrayValue::toJson(Var& arrayVar)
	{
		Json::Value value;
		if(arrayVar.getType() != Array)return value;
		for (unsigned i = 0; i < arrayVar.getSize(); ++i)
		{
			Var& var = arrayVar[i];
			if(var.getType() == Integer)
				value.append(var.asInt());
			else if (var.getType() == Double)
				value.append(var.asDouble());
			else if(var.getType() == String)
				value.append(var.asString());
			else if(var.getType() == Array)
				value.append(ArrayValue::toJson(var));
			else if(var.getType() == Object)
				value.append(ObjectValue::toJson(var));
			else
				continue;
		}
		return value;
	}

	mongo::BSONArray ArrayValue::toBson(Var& arrayVar)
	{
		mongo::BSONArrayBuilder arrayBuilder;
		if(arrayVar.getType() != Array)return arrayBuilder.arr();
		for (unsigned i = 0; i < arrayVar.getSize(); ++i)
		{
			Var& var = arrayVar[i];
			if(var.getType() == Integer)
				arrayBuilder << var.asInt();
			else if (var.getType() == Double)
				arrayBuilder << var.asDouble();
			else if(var.getType() == String)
				arrayBuilder << var.asString();
			else if(var.getType() == Array)
				arrayBuilder << ArrayValue::toBson(var);
			else if(var.getType() == Object)
				arrayBuilder << ObjectValue::toBson(var);
			else
				continue;
		}
		return arrayBuilder.arr();
	}

	void ArrayValue::format(Json::Value& dataJson)
	{
		if(dataJson.isArray())
		{
			for (unsigned i = 0; i < dataJson.size(); ++i)
			{
				Json::Value& currentJson = dataJson[i];
				if(currentJson.isInt())
					arrayVar.push_back(IntegerValue::Create(currentJson));
				else if (currentJson.isDouble())
					arrayVar.push_back(DoubleValue::Create(currentJson));
				else if (currentJson.isString())
					arrayVar.push_back(StringValue::Create(currentJson));
				else if(currentJson.isArray())
					arrayVar.push_back(ArrayValue::Create(currentJson));
				else if(currentJson.isObject())
					arrayVar.push_back(ObjectValue::Create(currentJson));
				else
					continue;
			}
		}
	}

	Json::Value ObjectValue::toJson(Var& objVar)
	{
		Json::Value value;
		if(objVar.getType() != Object)return value;
		ObjectValue* objPoint = (ObjectValue*)&objVar;
		MapVar& cMap = objPoint->mapVar;
		for (MapVar::iterator it = cMap.begin(); it != cMap.end(); ++it)
		{
			string key = it->first;
			Var& var = *it->second.get();
			if(var.getType() == Integer)
				value[key] = var.asInt();
			else if (var.getType() == Double)
				value[key] = var.asDouble();
			else if(var.getType() == String)
				value[key] = var.asString();
			else if(var.getType() == Array)
				value[key] = ArrayValue::toJson(var);
			else if(var.getType() == Object)
				value[key] = ObjectValue::toJson(var);
			else
				continue;
		}
		return value;
	}

	mongo::BSONObj ObjectValue::toBson(Var& objVar)
	{
		mongo::BSONObjBuilder objBuilder;
		if(objVar.getType() != Object)return objBuilder.obj();
		ObjectValue* objPoint = (ObjectValue*)&objVar;
		MapVar& cMap = objPoint->mapVar;
		for (MapVar::iterator it = cMap.begin(); it != cMap.end(); ++it)
		{
			string key = it->first;
			Var& var = *it->second.get();
			if(var.getType() == Integer)
				objBuilder << key << var.asInt();
			else if (var.getType() == Double)
				objBuilder << key << var.asDouble();
			else if(var.getType() == String)
				objBuilder << key << var.asString();
			else if(var.getType() == Array)
				objBuilder << key << ArrayValue::toBson(var);
			else if(var.getType() == Object)
				objBuilder << key << ObjectValue::toBson(var);
			else
				continue;
		}
		return objBuilder.obj();
	}

	void ObjectValue::format(Json::Value& dataJson)
	{
		if(dataJson.isObject())
		{
			for (Json::Value::iterator it = dataJson.begin(); it != dataJson.end(); ++it)
			{
				string key = it.key().asString();
				Json::Value& currentJson = (*it);
				if(currentJson.isInt())
					mapVar[key] = IntegerValue::Create(currentJson);
				else if (currentJson.isDouble())
					mapVar[key] = DoubleValue::Create(currentJson);
				else if (currentJson.isString())
					mapVar[key] = StringValue::Create(currentJson);
				else if(currentJson.isArray())
					mapVar[key] = ArrayValue::Create(currentJson);
				else if(currentJson.isObject())
					mapVar[key] = ObjectValue::Create(currentJson);
				else
					continue;
			}
		}
	}
}