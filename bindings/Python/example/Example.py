##################################################################################################
# MHash-384 - Language bindings for Python                                                       #
# Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                 #
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

import sys
import binascii
from MHashPy384_Wrapper import MHash384

def read_chunks(fs, chunk_size=4096):
    while True:
        data = fs.read(chunk_size)
        if not data:
            break
        yield data

def main(argv):
    if(len(argv) < 1):
        print("Command-line argument is missing!")
        return
    with MHash384() as digest:
        with open(argv[0], 'rb') as fs:
            for chunk in read_chunks(fs):
                digest.update(chunk)
            print(binascii.hexlify(digest.result()))

if __name__ == "__main__":
    main(sys.argv[1:])
