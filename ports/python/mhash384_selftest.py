"""MHash-384 - Simple fast portable secure hashing library
Copyright(c) 2016-2019 LoRd_MuldeR <mulder2@gmx.de>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""

import sys, unittest, mhash384

class TestStringMethods(unittest.TestCase):
	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	# INITIALIZATION
	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	@classmethod
	def setUpClass(cls):
		version, impl = mhash384.MHash384.version(), sys.implementation
		print('MHash-384 v{0}.{1:02d}-{2}, {3} v{4}.{5}.{6} [{7}]\n'.format(*version, impl.name, *impl.version[0:3], sys.platform))
		if sys.implementation.name.casefold() == "cpython".casefold():
			print("WARNING: You are running CPython, which is *very slow* for actual computations!")
			print("You can use, for example, PyPy (http://pypy.org/) for *much* improved speed ;-)\n")

	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	# TEST METHODS
	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	def test_vector_01(self):
		self.run_test(1, '',
			'D629CB37F76156621E09C93B8D1029C5F0F7B778DE5BD9578230673FE7FA4C45A2308AB4E9D99AA0EA194CBF6230C202')

	def test_vector_02(self):
		self.run_test(1, 'abc',
			'1D7B1593D7AA4DD7DC8DA6D6BD591D8B507CA514E1C7EC37F43E78DD6B83253C355403F61009582890480C4CD527F405')

	def test_vector_03(self):
		self.run_test(1, 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq',
			'9B98CE4C3F4EA9018E1ED3034717E3E5D8DA20AB375BAFE372D07780FFDEFDA32BA3C822155E2883E333AFCAAEAFA614')

	def test_vector_04(self):
		self.run_test(1, 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu',
			'45416D1FFD44D3940C5AF5B425B5DA07FD8C06BAB7C58CD13389BC284650ED6090A9A1952C29FEB1B2C6AFBF5F996108')

	def test_vector_05(self):
		self.run_test(0x186A0, 'aaaaaaaaaa',
			'20774C570AE2E6A49A01DAE948672AE94D465D9167130FE87BF24148649CF121E8A50759507BF97134F0D28CEE91DCA2')

	def test_vector_06(self):
		self.run_test(0x1000000, 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno',
			'C18843D4DD9E0479D38A55E8C75E2201F0E93B6EF1F916EBAF3C4074D188945C7B9657F45269A1DAAB4A8FB6F63D0BEE')

	def test_vector_07(self):
		self.run_test(1, 'The quick brown fox jumps over the lazy dog',
			'E2901EDB76AA83F2D52FEA9A34EB2BC05EDFA9571C812620245BBEBA95FA2D82CBB5BAD93EB64D4B72B46635D565400B')

	def test_vector_08(self):
		self.run_test(1, 'The quick brown fox jumps over the lazy cog',
			'8B81CD9C1DEE8BCC642F8B1EA982CE79D15F50E66939FF34294E63D93AE3C610F9464FEFEF30297409B62231F028ABC0')

	def test_vector_09(self):
		self.run_test(1, 'Franz jagt im komplett verwahrlosten Taxi quer durch Bayern',
			'A545E6319F6ACE0CEF62BCAAF5D488858751B96C2A488D5BE6EC63D50006C9D54DAD66168488F03B4A868D3BA610CCF8')

	def test_vector_10(self):
		self.run_test(1, 'Frank jagt im komplett verwahrlosten Taxi quer durch Bayern',
			'2BECB89DD55FC9F2F3384DF61D6240DDA54FC275D186A45A883C03AA873DDAF4FAD4744212B92D26054B92852DEA5F1D')

	def test_vector_11(self):
		self.run_test(1, 'Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.',
			'6283CE91DA8AF5091C7B06E1352DC947B9E3B89614807C04C43DA322321FBADAD3CCCABECB5C1B1B265432A6039121E1')

	def test_vector_12(self):
		self.run_test(1, 'Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamc0 laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.',
			'5DCB7C4B27BA841688FB6A860E97A45256917533F63448CF666781EA4B5FAB20E1C746C8821099A915222E0D77624098')

	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	# UTILITY METHODS
	# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	def run_test(self, count, input, expected):
		instance = mhash384.MHash384()
		for n in range(count):
			if not n & 0x3FFF:
				print('\r{0:.2f}%'.format(100 * (n / count)), end='')
			instance.update(input)
		digest = instance.digest().hex().upper()
		print('\r{0} - {1}'.format(digest, 'OK' if digest == expected else 'Error!'))
		self.assertEqual(digest, expected)

if __name__ == '__main__':
	unittest.main()
