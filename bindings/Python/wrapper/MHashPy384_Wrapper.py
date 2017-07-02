##################################################################################################
# MHash-384 - Language bindings for Python                                                       #
# Copyright(c) 2016-2017 LoRd_MuldeR <mulder2@gmx.de>                                            #
#                                                                                                #
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software  #
# and associated documentation files (the "Software"), to deal in the Software without           #
# restriction, including without limitation the rights to use, copy, modify, merge, publish,     #
# distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  #
# Software is furnished to do so, subject to the following conditions:                           #
#                                                                                                #
# The above copyright notice and this permission notice shall be included in all copies or       #
# substantial portions of the Software.                                                          #
#                                                                                                #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  #
# BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     #
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   #
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        #
##################################################################################################

import MHashPy384_Native as native

class MHash384:
    def __init__(self):
        self.__handle = native.create()

    def __enter__(self):
        return self

    def update(self, data, offset = None, len = None):
        if not native.update(self.__handle, data, offset, len):
            raise Exception("Something went wrong!")

    def result(self):
        return native.result(self.__handle)

    def freeup(self):
        if self.__handle is not None:
            native.freeup(self.__handle)
            self.__handle = None

    def __exit__(self, type, value, traceback):
        self.freeup()

    @staticmethod
    def getver():
        return native.getver()

