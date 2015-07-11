#pragma once

class base_updater
{
	public:
		virtual void update(playerDataPtr d) = 0;
};

class task_base
{
	public:
		void init(base_updater* ptr);
		void update(playerDataPtr d);

	private:
		base_updater* _updater;
};

class normal_task : public task_base
{
	public:
		

	private:
		int _type;
		int _argX;
		int _argY;
};

class count_updater : public base_updater
{
	public:
		void update(playerDataPtr d);

		static void setCount(int c);

	private:
		static int _count;
};

class value_updater : public base_updater
{
	public:
		void update(playerDataPtr d);

	
		static void setValue(int v);

	private:
		static int _value;
};
