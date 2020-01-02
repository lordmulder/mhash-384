/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Simple fast portable secure hashing library                                        */
/* Copyright(c) 2016-2020 LoRd_MuldeR <mulder2@gmx.de>                                            */
/*                                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy of this software  */
/* and associated documentation files (the "Software"), to deal in the Software without           */
/* restriction, including without limitation the rights to use, copy, modify, merge, publish,     */
/* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  */
/* Software is furnished to do so, subject to the following conditions:                           */
/*                                                                                                */
/* The above copyright notice and this permission notice shall be included in all copies or       */
/* substantial portions of the Software.                                                          */
/*                                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  */
/* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   */
/* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        */
/* ---------------------------------------------------------------------------------------------- */

package com.muldersoft.mhash384.test;

import com.muldersoft.mhash384.MHash384;

import org.junit.Test;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import static org.junit.Assert.assertTrue;

import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;
import java.util.concurrent.atomic.AtomicBoolean;

public class MHash384Test {

	private static final AtomicBoolean m_success = new AtomicBoolean();

	@BeforeClass
	public static void initialize() {
		final RuntimeMXBean rt = ManagementFactory.getRuntimeMXBean();
		System.out.printf("MHash-384 v%d.%02d-%d, Java v%s [%s %s]\n\n", MHash384.VERSION_MAJOR, MHash384.VERSION_MINOR, MHash384.VERSION_PATCH, rt.getSpecVersion(), rt.getVmName(), rt.getVmVersion());
		m_success.set(true);
	}
	
	@AfterClass
	public static void finish() {
		if(m_success.get()) {
			System.out.println("\nSelf-test completed successfully :-)");
		} else {
			System.out.println("\nError: Self-test has failed! :-(");
		}
	}
	
	//=======================================================================
	// TEST VECTORS
	//=======================================================================

	@Test
	public void TestVector_01() {
		runTest(1, "",
			"D629CB37F76156621E09C93B8D1029C5F0F7B778DE5BD9578230673FE7FA4C45A2308AB4E9D99AA0EA194CBF6230C202");
	}

	@Test
	public void TestVector_02() {
		runTest(1, "abc",
			"1D7B1593D7AA4DD7DC8DA6D6BD591D8B507CA514E1C7EC37F43E78DD6B83253C355403F61009582890480C4CD527F405");
	}
	
	@Test
	public void TestVector_03() {
		runTest(1, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
			"9B98CE4C3F4EA9018E1ED3034717E3E5D8DA20AB375BAFE372D07780FFDEFDA32BA3C822155E2883E333AFCAAEAFA614");
	}

	@Test
	public void TestVector_04() {
		runTest(1, "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
			"45416D1FFD44D3940C5AF5B425B5DA07FD8C06BAB7C58CD13389BC284650ED6090A9A1952C29FEB1B2C6AFBF5F996108");
	}
	
	@Test
	public void TestVector_05() {
		runTest(0x186A0, "aaaaaaaaaa",
			"20774C570AE2E6A49A01DAE948672AE94D465D9167130FE87BF24148649CF121E8A50759507BF97134F0D28CEE91DCA2");
	}

	@Test
	public void TestVector_06() {
		runTest(0x1000000, "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno",
			"C18843D4DD9E0479D38A55E8C75E2201F0E93B6EF1F916EBAF3C4074D188945C7B9657F45269A1DAAB4A8FB6F63D0BEE");
	}
	

	@Test
	public void TestVector_07() {
		runTest(1, "The quick brown fox jumps over the lazy dog",
			"E2901EDB76AA83F2D52FEA9A34EB2BC05EDFA9571C812620245BBEBA95FA2D82CBB5BAD93EB64D4B72B46635D565400B");
	}
	

	@Test
	public void TestVector_08() {
		runTest(1, "The quick brown fox jumps over the lazy cog",
			"8B81CD9C1DEE8BCC642F8B1EA982CE79D15F50E66939FF34294E63D93AE3C610F9464FEFEF30297409B62231F028ABC0");
	}
	

	@Test
	public void TestVector_09() {
		runTest(1, "Franz jagt im komplett verwahrlosten Taxi quer durch Bayern",
			"A545E6319F6ACE0CEF62BCAAF5D488858751B96C2A488D5BE6EC63D50006C9D54DAD66168488F03B4A868D3BA610CCF8");
	}
	
	@Test
	public void TestVector_10() {
		runTest(1, "Frank jagt im komplett verwahrlosten Taxi quer durch Bayern",
			"2BECB89DD55FC9F2F3384DF61D6240DDA54FC275D186A45A883C03AA873DDAF4FAD4744212B92D26054B92852DEA5F1D");
	}
	
	@Test
	public void TestVector_11() {
		runTest(1, "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
			"6283CE91DA8AF5091C7B06E1352DC947B9E3B89614807C04C43DA322321FBADAD3CCCABECB5C1B1B265432A6039121E1");
	}

	@Test
	public void TestVector_12() {
		runTest(1, "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamc0 laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
			"5DCB7C4B27BA841688FB6A860E97A45256917533F63448CF666781EA4B5FAB20E1C746C8821099A915222E0D77624098");
	}

	//=======================================================================
	// HELPER FUNCTIONS
	//=======================================================================

	private static final String HEX_CHARS = "0123456789ABCDEF";

	private static void runTest(final int repetitions, final String test, final String expected) {
		final String digest = getDigest(repetitions, test);
		final boolean matches = digest.equalsIgnoreCase(expected);
		System.out.printf("%s - %s\n", digest, matches ? "OK" : "Error!");
		if(!matches) {
			m_success.compareAndSet(true, false);
		}
		assertTrue(matches);
	}

	private static String getDigest(final int repetitions, final String test) {
		final MHash384 hash = new MHash384();
		for(int i = 0; i < repetitions; ++i) {
			hash.update(test);
		}
		return bytesToHex(hash.getDigest());
	}
	
	private static String bytesToHex(byte[] bytes) {
		final char[] hexChars = new char[bytes.length * 2];
		final char[] hexArray = HEX_CHARS.toCharArray();
		for ( int j = 0; j < bytes.length; j++ ) {
			final int v = bytes[j] & 0xFF;
			hexChars[j * 2] = hexArray[v >>> 4];
			hexChars[j * 2 + 1] = hexArray[v & 0x0F];
		}
		return new String(hexChars);
	}
}
