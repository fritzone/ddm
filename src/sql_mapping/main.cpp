#include <string>
#include <stdio.h>

static std::string default("_default");


/**
 * Class representing a Table
 */
class Table
{
public:
	virtual std::string operator()() const { return default; };
};

/**
* Class representing a column
*/
class Column
{
public:
	virtual std::string operator()() const { return default; };
	virtual std::string operator == (int nr) 
	{ 
		char s[32]; sprintf(s, "%d", nr);
		return (*this)() + std::string(" == ") + std::string(s);
	}

};

std::string operator == (const Column& c1, const Column& c2)
{
	return c1() + "==" + c2();
}

class Person : public Table
{
public:

	class Id : public Column
	{
	public:
		virtual std::string operator()() const {return std::string("PERSON.ID");}
	};

	class Name : public Column
	{
	public:
		virtual std::string operator()() const {return std::string("PERSON.NAME");}
	
	};

	virtual std::string operator()() const {return std::string("PERSON");}

};

template < class C1 = Column, class C2 = Column, class C3 = Column, class C4 = Column >
class ResultSet
{

};

template <class C1 = Column, class C2 = Column, class C3 = Column, class C4 = Column>
class Select
{
public:
	template <class T1 = Table, class T2 = Table, class T3 = Table, class T4 = Table>
	class From
	{
	public:
		From(){}

		
		static ResultSet<C1,C2, C3, C4> Where(std::string whereClause)
		{
			std::string s = "SELECT ";
			if(C1()() != default) s += C1()();
			if(C2()() != default) { s += ", "; s += C2()(); }
			if(C3()() != default) { s += ", "; s += C3()(); }
			if(C4()() != default) { s += ", "; s += C3()(); }
			s += " FROM ";
			if(T1()() != default) s += T1()();
			if(T2()() != default) { s += ", "; s += T2()(); }
			if(T3()() != default) { s += ", "; s += T3()(); }
			if(T4()() != default) { s += ", "; s += T4()(); }


			return ResultSet<C1,C2,C3,C4>();
		}
	};
};

int main()
{
	ResultSet<Person::Id, Person::Name> rs = Select<Person::Id, Person::Name>::From<Person>::Where( Person::Id() == 123);
}
