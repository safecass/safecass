#-----------------------------------------------------------------------------------
#
# SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
#
# Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
#
#-----------------------------------------------------------------------------------
#
# Created on   : Oct 19, 2015
# Last revision: Oct 19, 2015
# Author       : Min Yang Jung (myj@jhu.edu)
# URL          : https://github.com/safecass/safecass
#
macro (add_ctest atest)
  add_test (NAME ${atest} COMMAND $<TARGET_FILE:${atest}>)
endmacro()
