/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Language bindings for Java                                                         */
/* Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                 */
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

package mhash;

public class MHash384 implements AutoCloseable {

	private Long m_handle;
	
	//--------------------------------------------------------------------
	// Static Initializer
	//--------------------------------------------------------------------
	
	static {
		final String libraryPath = getProperty("mhash384.library.path");
		final String libraryName = getProperty("mhash384.library.name");
		if(libraryPath != null) {
			System.load(libraryPath);
		}
		else if(libraryName != null) {
			System.loadLibrary(libraryName);
		}
		else {
			final boolean x64 = getProperty("sun.arch.data.model", "32").equals("64");
			System.loadLibrary("MHashJava384." + (x64 ? "x64" : "x86"));
		}
	}
	
	//--------------------------------------------------------------------
	// Constructor / Finalizer
	//--------------------------------------------------------------------

	public MHash384() {
		m_handle = Internals.createInstance();
	}
	
	protected void finalize() {
		close();
	}

	//--------------------------------------------------------------------
	// Public Methods
	//--------------------------------------------------------------------

	public void update(final byte[] data, final int offset, final int len) {
		if((offset < 0) || (len <= 0)) {
			throw new IllegalArgumentException("offset or len must not be negative!");
		}
		if((offset + len < 0) || (offset + len > data.length)) {
			throw new IndexOutOfBoundsException("offset + len exceeds array limits!");
		}
		Internals.update(m_handle, data, offset, len);
	}
	
	public byte[] result() {
		return Internals.finalize(m_handle);
	}
	
	@Override
	public void close() {
		if(m_handle != null) {
			Internals.destroyInstance(m_handle);
			m_handle = null;
		}
	}
	
	public static Version getVersion()
	{
		final int[] version = Internals.getVersion();
		return new Version(version[0], version[1], version[2]);
	}
	
	//--------------------------------------------------------------------
	// Native Methods
	//--------------------------------------------------------------------

	private static class Internals {
		native static long createInstance();
		native static void update(final long handle, final byte[] data, final int offset, final int len);
		native static byte[] finalize(final long handle);
		native static void destroyInstance(final long handle);
		native static int[] getVersion();
	}

	//--------------------------------------------------------------------
	// Helper Class
	//--------------------------------------------------------------------
	
	public static class Version {
		public final int major, minor, patch;
		protected Version(int major, int minor, int patch) {
			this.major = major;
			this.minor = minor;
			this.patch = patch;
		}
	}
	
	//--------------------------------------------------------------------
	// Utility Functions
	//--------------------------------------------------------------------
	
	private static String getProperty(final String key) {
		return getProperty(key, null);
	}
	
	private static String getProperty(final String key, final String fallback) {
		final String val = System.getProperty(key);
		if(val != null) {
			final String trimmed = val.trim();
			if(!trimmed.isEmpty()) {
				return trimmed;
			}
		}
		return fallback;
	}
}
