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
			"4C4B82D07B368E1C22D0DE3759C32D44DA71BE6283E8550A2468DC1FEC38919F7EDB6C1BA08378EC583AE612AB0E02BA");
	}

	@Test
	public void TestVector_02() {
		runTest(1, "abc",
			"9171D83EE7DEDE36CAF27C2644897F3114A0F67B6E9193AA1AB23462EA815EDEA535002671E086493B41A528A26FD8B3");
	}
	
	@Test
	public void TestVector_03() {
		runTest(1, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
			"290BD2162C2105A0824172A8875EE33BB65A98DC0928100441B41B9399F6A8EA09794834504A3E817D49D29BC20A520A");
	}

	@Test
	public void TestVector_04() {
		runTest(1, "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
			"0B3F13A68AA8D8F0C5B9BF8BE5AECCB73E0D13732C9290006B6DC939ADA79C48AE362E545A067D2C1FB0749C60A49243");
	}
	
	@Test
	public void TestVector_05() {
		runTest(0x186A0, "aaaaaaaaaa",
			"56228E9432471B09A7F696D0DEFA15E664D3E7ACD27E2D39F864C05006F1F77012F4F4CCE7450C52B6C1CFAB84FAEC63");
	}

	@Test
	public void TestVector_06() {
		runTest(0x1000000, "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno",
			"3A199A673FAB2900AB80FEC1185F79359FEC44B88728E3D62DC31A936C62DB05EF35716FED3074E9310DDDF69ED5671C");
	}
	

	@Test
	public void TestVector_07() {
		runTest(1, "The quick brown fox jumps over the lazy dog",
			"79F76CA53D529162E632152EDE82A403F8F996DEAA009CC512250BAFF910AC24DF1381F7EF1F43DAC26F63EE0CFF3CDF");
	}
	

	@Test
	public void TestVector_08() {
		runTest(1, "The quick brown fox jumps over the lazy cog",
			"8A2A58B20020F7700FFF629B0D7238D3D5311AC2A9ADA606E69AD7BEBF2B6258AEC74080DEC04AD59F3B9326121DFF66");
	}
	

	@Test
	public void TestVector_09() {
		runTest(1, "Franz jagt im komplett verwahrlosten Taxi quer durch Bayern",
			"D2E07EA37EF1E0E52BB704DEC3330C3378B943FE242CF3B08B93D18DBD61D4AB7C42E581DBFDBFD2F5D8EDF82C3B35D6");
	}
	
	@Test
	public void TestVector_10() {
		runTest(1, "Frank jagt im komplett verwahrlosten Taxi quer durch Bayern",
			"E97C790B194532A59BC84090B5C68C5B0D050C6FE937ABDF480CC19C345B72FEF925D83BF9B42D1A8F572ADE7A509FF9");
	}
	
	@Test
	public void TestVector_11() {
		runTest(1, "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
			"A772D7B984ABC790A9FFF51F3BD7C6A53844A233A564A970872C41345AFE19983B8D3CE30B900FD7FDD66CED03D0CD6E");
	}

	@Test
	public void TestVector_12() {
		runTest(1, "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamc0 laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
			"614A6B25BD673216EDEAB6A051A8B4869F9AD80CC5DD4AE629DDFB70CAA70E49D51E7027FF35A183A278FE97F8759CF9");
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
