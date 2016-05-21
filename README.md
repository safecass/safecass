# Welcome to SAFECASS

[![License](https://img.shields.io/github/license/safecass/safecass.svg)](https://github.com/safecass/safecass/blob/master/LICENSE)
[![Issues](https://img.shields.io/github/issues/safecass/safecass.svg)](https://github.com/safecass/safecass/issues)
[![Build Linux](https://img.shields.io/travis/safecass/safecass.svg)](https://travis-ci.org/safecass/safecass)


**SAFECASS** (**S**afety **A**rchitecture **f**or **E**ngineering
**C**omputer-**A**ssisted **S**urgical **S**ystems) is an open source framework
that enables reuse of experience and knowledge on safety, thereby facilitating
the development process of safety features for robot systems that physically
interact with humans.

SAFECASS is designed and developed as part of <a href='https://drive.google.com/open?id=0BwGcEiVvK4n9RUJmSXhUYW90SG8' target='_blank'>my PhD thesis</a>, which focuses on the safety of component-based medical and surgical robot systems.  Relevant publications are available [here](https://github.com/safecass/safecass/wiki/Publications).

*SAFECASS is currently undergoing major code restructuring to improve the
overall build process with better support for unit-testing.*  

For the time being, the master branch is not fully functional and only a
limited set of features is available.  The design documents are still
accessible via the [SAFECASS wiki](https://github.com/safecass/safecass/wiki).

NEWS:

 * 05/18/2016: SAFECASS overview paper has been presented in the IEEE International Conference on Robotics and Automation (ICRA), Stockholm, Sweden, 2016 [ [Abstract](https://ras.papercept.net/conferences/scripts/abstract.pl?ConfID=119&Number=2580) ]
 * April, 2016: Started using [Boost.Chrono](http://theboostcpplibraries.com/boost.graph) as base time representation and [Boost.Graph](http://theboostcpplibraries.com/boost.chrono) for graph management.
 * 03/15/2016: Design refactoring and new implementation of history buffer, common parameter types, and reference implementation of history buffer, adding a bunch of unit tests for the core SAFECASS library
 * 10/27/2015: Working on building the core safecass library
 * 10/19/2015: The build chain of SAFECASS is completely being re-written from
   scratch to make the overall build process mcuh simpler, easier, and cleaner.
 * 10/11/2015: The [wiki pages](https://github.com/safecass/safecass/wiki) have
 been updated.
 * 09/21/2015: SAFECASS is currently undergoing repository transition.

CONTACT:

 * Min Yang Jung, PhD (myj at jhu dot edu)
 * Peter Kazanzides, PhD (pkaz at jhu dot edu)