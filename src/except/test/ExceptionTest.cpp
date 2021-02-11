// Copyright 2008 Drew Olbrich

#include <sstream>
#include <exception>

#include <cppunit/extensions/HelperMacros.h>

#include <except/Exception.h>
#include <except/ExceptionMessageAppender.h>
#include <except/SourceLine.h>

// Exception can't be instantiated directly, so we use this concrete exception for
// testing. A derived class with a destructor must be defined to make Exception usable.
class ConcreteException 
    : public except::Exception,
        public except::ExceptionMessageAppender<ConcreteException> 
{
public:
    explicit ConcreteException(const except::SourceLine &sourceLine)
        : Exception(sourceLine) {
    }
    ~ConcreteException() throw() {
    }
};

class ExceptionTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ExceptionTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testStreamOutput);
    CPPUNIT_TEST(testCatchConcrete);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testConstructor() {
        except::SourceLine undefinedSourceLine;
        ConcreteException exception(undefinedSourceLine);
        exception << "foo" << 5 << "bar";
        CPPUNIT_ASSERT(exception.asString() == "foo5bar");
    }

    void testStreamOutput() {
        except::SourceLine undefinedSourceLine;
        ConcreteException exception(undefinedSourceLine);
        exception << "foo" << 5 << "bar";
        std::ostringstream ostr;
        ostr << exception;
        CPPUNIT_ASSERT(ostr.str() == "foo5bar");
    }

    void testCatchConcrete() {
        std::string description;
        try {
            except::SourceLine undefinedSourceLine;
            throw ConcreteException(undefinedSourceLine) << "foo" << 5 << "bar";
        } catch (const ConcreteException &exception) {
            description = exception.asString();
        } catch (...) {
            // If we reach this point, the thrown ConcreteException is being sliced,
            // probably resulting in a reference to an Exception instance.
            CPPUNIT_ASSERT(0);
        }
        CPPUNIT_ASSERT(description == "foo5bar");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ExceptionTest);
