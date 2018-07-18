#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE SharedPointerTests


#include <boost/test/unit_test.hpp>
#include "../../src/include/shared_ptr.hpp"

template<class T>
struct CTestDeleter {
	long& _field = 0;
	bool& _isDeleterWasCalled = false;
	void operator()(T *data) {
		if (data) {
			_isDeleterWasCalled = true;
			delete data;
		}
	}
	CTestDeleter() = default;
	CTestDeleter(long& field, bool& isDeleterWasCalled)
		: _field(field), _isDeleterWasCalled(isDeleterWasCalled) 
	{ }
};
struct CTestBase {
	long _value;
	bool &_isBaseDestructorWasCalled;
	virtual ~CTestBase() { _isBaseDestructorWasCalled = true; }
	CTestBase(long value, bool &isBaseDestructorWasCalled) 
		: _value(value), _isBaseDestructorWasCalled(isBaseDestructorWasCalled) 
	{ }
};

struct CTestDerived : CTestBase {
	bool &_isDerivedDestructorWasCalled;
	~CTestDerived() { _isDerivedDestructorWasCalled = true; }
	CTestDerived(long value, bool &isBaseDestructorWasCalled, bool &isDerivedDestructorWasCalled) 
		: CTestBase(value, isBaseDestructorWasCalled), _isDerivedDestructorWasCalled(isDerivedDestructorWasCalled)
	{ }
};

BOOST_AUTO_TEST_CASE(SharedPointerConstructorsTest)
{
	// Default constructor
	stl_like::shared_ptr<long>ptr1;

	BOOST_REQUIRE(ptr1.use_count() == 0);      
	
	// Capture constructor 
	stl_like::shared_ptr<long>ptr2{ new long(3) };

	BOOST_REQUIRE(ptr2.use_count() == 1);

	// Copy constructor
	stl_like::shared_ptr<long>ptr3{ ptr2 };

	BOOST_REQUIRE(ptr2.use_count() == 2);
	BOOST_REQUIRE(ptr3.use_count() == 2);

	BOOST_REQUIRE(*ptr2.get() == 3);
	BOOST_REQUIRE(*ptr3.get() == 3);

	// Move constructor
	stl_like::shared_ptr<long>ptr4 { std::move(ptr3)  };

	BOOST_REQUIRE(*ptr4.get() == 3);
	BOOST_REQUIRE(ptr4.use_count() == 2);
	BOOST_REQUIRE(ptr3.use_count() == 0);
	BOOST_REQUIRE(ptr3.get() == nullptr);

	// Convertible constructors 
	bool isBaseDestructorWasCalled = false;
	bool isDerivedDestructorWasCalled = false;
	stl_like::shared_ptr<CTestDerived>ptr5{ new CTestDerived(5, isBaseDestructorWasCalled, isDerivedDestructorWasCalled) };
	stl_like::shared_ptr<CTestBase>ptr6{ ptr5 };

	BOOST_REQUIRE(ptr5.get()->_value == 5);
	BOOST_REQUIRE(ptr5.use_count() == 2);
	BOOST_REQUIRE(ptr6.get()->_value == 5);
	BOOST_REQUIRE(ptr6.use_count() == 2);
	

	stl_like::shared_ptr<CTestBase>ptr7{ std::move(ptr5) };

	BOOST_REQUIRE(ptr7.get()->_value == 5);
	BOOST_REQUIRE(ptr5.get() == nullptr);
	BOOST_REQUIRE(ptr7.use_count() == 2);
	BOOST_REQUIRE(ptr5.use_count() == 0);

	ptr7.reset();
	ptr6.reset();

	BOOST_REQUIRE(ptr7.get() == nullptr);
	BOOST_REQUIRE(ptr6.get() == nullptr);
	BOOST_REQUIRE(ptr7.use_count() == 0);
	BOOST_REQUIRE(ptr6.use_count() == 0);

	BOOST_REQUIRE(isDerivedDestructorWasCalled);
	BOOST_REQUIRE(isBaseDestructorWasCalled);
}

BOOST_AUTO_TEST_CASE(SharedPointerAssigmentOperatorsTest)
{
	stl_like::shared_ptr<long>ptr1{ new long(5) };
	stl_like::shared_ptr<long>ptr2{ new long(7) };

	BOOST_REQUIRE(ptr1.use_count() == 1);
	BOOST_REQUIRE(*ptr1.get() == 5);

	BOOST_REQUIRE(ptr2.use_count() == 1);
	BOOST_REQUIRE(*ptr2.get() == 7);

	ptr1 = ptr2;
	
	BOOST_REQUIRE(ptr1.use_count() == 2);
	BOOST_REQUIRE(*ptr1.get() == 7);

	BOOST_REQUIRE(ptr2.use_count() == 2);
	BOOST_REQUIRE(*ptr2.get() == 7);

	ptr1 = std::move(ptr2);

	BOOST_REQUIRE(ptr1.use_count() == 1);
	BOOST_REQUIRE(*ptr1.get() == 7);

	BOOST_REQUIRE(ptr2.use_count() == 0);
	BOOST_REQUIRE(ptr2.get() == nullptr);


	bool isBaseDestructorWasCalled = false, isBaseDestructorWasCalled2 = false;
	bool isDerivedDestructorWasCalled = false, isDerivedDestructorWasCalled2 = false;
	stl_like::shared_ptr<CTestDerived>ptr5{ new CTestDerived(5, isBaseDestructorWasCalled, isDerivedDestructorWasCalled) };
	stl_like::shared_ptr<CTestBase>ptr6{ new CTestDerived(42, isBaseDestructorWasCalled2, isDerivedDestructorWasCalled2) };

	BOOST_REQUIRE(ptr5.get()->_value == 5);
	BOOST_REQUIRE(ptr5.use_count() == 1);
	BOOST_REQUIRE(ptr6.get()->_value == 42);
	BOOST_REQUIRE(ptr6.use_count() == 1);

	ptr6 = ptr5;

	BOOST_REQUIRE(ptr5.get()->_value == 5);
	BOOST_REQUIRE(ptr5.use_count() == 2);
	BOOST_REQUIRE(ptr6.get()->_value == 5);
	BOOST_REQUIRE(ptr6.use_count() == 2);

	BOOST_REQUIRE(isBaseDestructorWasCalled2);
	BOOST_REQUIRE(isDerivedDestructorWasCalled2);

	ptr6 = std::move(ptr5);

	BOOST_REQUIRE(ptr5.get() == nullptr);
	BOOST_REQUIRE(ptr5.use_count() == 0);

	BOOST_REQUIRE(ptr6.get()->_value == 5);
	BOOST_REQUIRE(ptr6.use_count() == 1);

	ptr6.reset();

	BOOST_REQUIRE(ptr6.get() == nullptr);
	BOOST_REQUIRE(ptr6.use_count() == 0);

	BOOST_REQUIRE(isBaseDestructorWasCalled);
	BOOST_REQUIRE(isDerivedDestructorWasCalled);
}

BOOST_AUTO_TEST_CASE(SharedPointerDeleterTest)
{
	long value = 0;
	bool isDeleterWasCalled = false;
	stl_like::shared_ptr<long>ptr1{ new long(5), CTestDeleter<long>(value, isDeleterWasCalled) };
	stl_like::shared_ptr<long>ptr2;

	BOOST_REQUIRE(ptr1.use_count() == 1);
	BOOST_REQUIRE(*ptr1.get() == 5);

	CTestDeleter<long>* deleter = ptr1.get_deleter<CTestDeleter<long>>();
	deleter->_field = 5;

	BOOST_REQUIRE(value == 5);
	
	// After assigment we should have the same deleter in ptr2
	ptr2 = ptr1;

	BOOST_REQUIRE(ptr1.use_count() == 2);
	BOOST_REQUIRE(*ptr1.get() == 5);

	BOOST_REQUIRE(ptr2.use_count() == 2);
	BOOST_REQUIRE(*ptr2.get() == 5);

	deleter = ptr2.get_deleter<CTestDeleter<long>>();
	deleter->_field += 5;

	BOOST_REQUIRE(value == 10);

	ptr1.reset();
	ptr2.reset();

	BOOST_REQUIRE(isDeleterWasCalled);

	BOOST_REQUIRE(ptr1.use_count() == 0);
	BOOST_REQUIRE(ptr1.get() == nullptr);

	BOOST_REQUIRE(ptr2.use_count() == 0);
	BOOST_REQUIRE(ptr2.get() == nullptr);

}