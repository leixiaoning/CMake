/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "cmCTest.h"
#include "cmSystemTools.h"

// Need these for documentation support.
#include "cmake.h"
#include "cmDocumentation.h"

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationName[] =
{
  {0,
   "  ctest - Testing driver provided by CMake.", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationUsage[] =
{
  {0,
   "  ctest [options]", 0},
  {0,0,0}
};

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationDescription[] =
{
  {0,
   "The \"ctest\" executable is the CMake test driver program.  "
   "CMake-generated build trees created for projects that use "
   "the ENABLE_TESTING and ADD_TEST commands have testing support.  "
   "This program will run the tests and report results.", 0},
  CMAKE_STANDARD_INTRODUCTION,
  {0,0,0}
};

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationOptions[] =
{
  {"-C <config>", "Choose configuration to test.",
   "Some CMake-generated build trees can have multiple build configurations "
   "in the same tree.  This option can be used to specify which one should "
   "be tested.  Example configurations are \"Debug\" and \"Release\"."},
  {"-V,--verbose", "Enable verbose output from tests.",
   "Test output is normally suppressed and only summary information is "
   "displayed.  This option will show all test output."},
  {"-N,--show-only", "Disable actual execution of tests.",
   "This option tells ctest to list the tests that would be run but not "
   "actually run them.  Useful in conjunction with the -R and -E options."},
  {"-R <regex>", "Run tests matching regular expression.",
   "This option tells ctest to run only the tests whose names match the "
   "given regular expression."},
  {"-E <regex>", "Exclude tests matching regular expression.",
   "This option tells ctest to NOT run the tests whose names match the "
   "given regular expression."},
  {0,0,0}
};

//----------------------------------------------------------------------------
static const cmDocumentationEntry cmDocumentationSeeAlso[] =
{
  {0, "cmake", 0},
  {0, "ccmake", 0},
  {0, 0, 0}
};

// this is a test driver program for cmCTest.
int main (int argc, char *argv[])
{
  cmSystemTools::EnableMSVCDebugHook();
  
  // If there is a testing input file, check for documentation options
  // only if there are actually arguments.  We want running without
  // arguments to run tests.
  if(argc > 1 || !cmSystemTools::FileExists("DartTestfile.txt"))
    {
    if(argc == 1)
      {
      std::cout << "*********************************" << std::endl;
      std::cout << "No test configuration file found!" << std::endl;
      std::cout << "*********************************" << std::endl;
      }
    cmDocumentation doc;
    if(doc.CheckOptions(argc, argv))
      {
      // Construct and print requested documentation.
      doc.SetName("ctest");
      doc.SetNameSection(cmDocumentationName);
      doc.SetUsageSection(cmDocumentationUsage);
      doc.SetDescriptionSection(cmDocumentationDescription);
      doc.SetOptionsSection(cmDocumentationOptions);
      doc.SetSeeAlsoList(cmDocumentationSeeAlso);
      return doc.PrintRequestedDocumentation(std::cout)? 0:1;
      }
    }
  
  cmCTest inst;
  
  // look at the args
  std::vector<std::string> args;
  for(int i =0; i < argc; ++i)
    {
    args.push_back(argv[i]);
    }

#ifdef _WIN32
  std::string comspec = "cmw9xcom.exe";
  cmSystemTools::SetWindows9xComspecSubstitute(comspec.c_str());
#endif
  
  for(unsigned int i=1; i < args.size(); ++i)
    {
    std::string arg = args[i];
    if(arg.find("-C",0) == 0 && i < args.size() - 1)
      {
      inst.m_ConfigType = args[i+1];
      }

    if( arg.find("-V",0) == 0 || arg.find("--verbose",0) == 0 )
      {
      inst.m_Verbose = true;
      }

    if( arg.find("-N",0) == 0 || arg.find("--show-only",0) == 0 )
      {
      inst.m_ShowOnly = true;
      }

    if( arg.find("-D",0) == 0 && i < args.size() - 1 )
      {
      inst.m_DartMode = true;
      std::string targ = args[i+1];
      if ( targ == "Experimental" )
        {
        inst.SetTestModel(cmCTest::EXPERIMENTAL);
        inst.SetTest("Start");
        inst.SetTest("Configure");
        inst.SetTest("Build");
        inst.SetTest("Test");
        inst.SetTest("Coverage");
        inst.SetTest("Submit");
        }
      else if ( targ == "Continuous" )
        {
        inst.SetTestModel(cmCTest::CONTINUOUS);
        inst.SetTest("Start");
        inst.SetTest("Update");
        inst.SetTest("Configure");
        inst.SetTest("Build");
        inst.SetTest("Test");
        inst.SetTest("Coverage");
        inst.SetTest("Submit");
        }
      else if ( targ == "Nightly" )
        {
        inst.SetTestModel(cmCTest::NIGHTLY);
        inst.SetTest("Start");
        inst.SetTest("Update");
        inst.SetTest("Configure");
        inst.SetTest("Build");
        inst.SetTest("Test");
        inst.SetTest("Coverage");
        inst.SetTest("Submit");
        }
      else if ( targ == "MemoryCheck" )
        {
        inst.SetTestModel(cmCTest::EXPERIMENTAL);
        inst.SetTest("Start");
        inst.SetTest("Configure");
        inst.SetTest("Build");
        inst.SetTest("Purify");
        inst.SetTest("Coverage");
        inst.SetTest("Submit");
        }
      else if ( targ == "NightlyMemoryCheck" )
        {
        inst.SetTestModel(cmCTest::NIGHTLY);
        inst.SetTest("Start");
        inst.SetTest("Update");
        inst.SetTest("Configure");
        inst.SetTest("Build");
        inst.SetTest("Purify");
        inst.SetTest("Coverage");
        inst.SetTest("Submit");
        }
      }

    if( ( arg.find("-T",0) == 0 ) && 
        (i < args.size() -1) )
      {
      inst.m_DartMode = true;
      inst.SetTest(args[i+1].c_str());
      }
    
    if( ( arg.find("-M",0) == 0 || arg.find("--test-model",0) == 0 ) &&
        (i < args.size() -1) )
      {
      std::string& str = args[i+1];
      if ( str == "NIGHTLY" || str == "nightly" || str == "Nightly" )
        {
        inst.SetTestModel(cmCTest::NIGHTLY);
        }
      else if ( str == "CONTINUOUS" || str == "continuous" || 
                str == "Continuous" )
        {
        inst.SetTestModel(cmCTest::CONTINUOUS);
        std::cout << "Continuous" << std::endl;
        }
      else
        {
        inst.SetTestModel(cmCTest::EXPERIMENTAL);
        }
      }
    
    if(arg.find("-R",0) == 0 && i < args.size() - 1)
      {
      inst.m_UseIncludeRegExp = true;
      inst.m_IncludeRegExp  = args[i+1];
      }

    if(arg.find("-E",0) == 0 && i < args.size() - 1)
      {
      inst.m_UseExcludeRegExp = true;
      inst.m_ExcludeRegExp  = args[i+1];
      inst.m_UseExcludeRegExpFirst = inst.m_UseIncludeRegExp ? false : true;
      }
    }

  // call process directory
  inst.Initialize();
  int res = inst.ProcessTests();
  inst.Finalize();

  return res;
}

