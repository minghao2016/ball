// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
#include <BALL/CONCEPT/classTest.h>
#include <BALLTestConfig.h>

///////////////////////////

#include <BALL/NMR/shiftModule.h>

///////////////////////////

START_TEST(ShiftModule)

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

using namespace BALL;

ShiftModule* sm = 0;
CHECK(ShiftModule::ShiftModule() throw())
	sm = new ShiftModule;
	TEST_NOT_EQUAL(sm, 0)
RESULT


CHECK(ShiftModule::~ShiftModule() throw())
  delete sm;
RESULT


CHECK(ShiftModule::setName(const String& name) throw())
  ShiftModule sm;
	sm.setName("TEST1");
	sm.setName("");
RESULT


CHECK(ShiftModule::getName() const  throw())
  ShiftModule sm;
	TEST_EQUAL(sm.getName(), "")
	sm.setName("TEST1");
	TEST_EQUAL(sm.getName(), "TEST1")
	sm.setName("");
	TEST_EQUAL(sm.getName(), "")
RESULT


Parameters parameters(BALL_TEST_DATA_PATH(ShiftModule_test.ini));
CHECK(ShiftModule::setParameters(Parameters& parameters) throw())
	ShiftModule sm;
	sm.setParameters(parameters);
RESULT


CHECK(ShiftModule::getParameters() const  throw())
	ShiftModule sm;
	TEST_EQUAL(sm.getParameters(), 0)
	sm.setParameters(parameters);
	TEST_EQUAL(sm.getParameters(), &parameters)
RESULT



CHECK(ShiftModule::ShiftModule(Parameters& parameters, const String& name = "") throw())
	ShiftModule sm(parameters, "test_name");
	TEST_EQUAL(sm.getName(), "test_name")
	TEST_EQUAL(sm.getParameters(), &parameters)
RESULT


CHECK(ShiftModule::ShiftModule(const ShiftModule& module) throw())
	ShiftModule sm1(parameters, "test_name");
	ShiftModule sm2(sm1);
	TEST_EQUAL(sm2.getName(), "test_name")
	TEST_EQUAL(sm2.getParameters(), &parameters)
RESULT


CHECK(ShiftModule::clear() throw())
	ShiftModule sm(parameters, "test_name");
	sm.clear();
	TEST_EQUAL(sm.getName(), "")
	TEST_EQUAL(sm.getParameters(), 0)
RESULT


CHECK(ShiftModule::init() throw())
	ShiftModule sm(parameters, "test_name");
	sm.init();
RESULT


CHECK(ShiftModule::ShiftModule& operator = (const ShiftModule& module) throw())
	ShiftModule sm1(parameters, "test_name");
	ShiftModule sm2;
	sm2 = sm1;
	TEST_EQUAL(sm2.getName(), "test_name")
	TEST_EQUAL(sm2.getParameters(), &parameters)
RESULT


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
END_TEST
