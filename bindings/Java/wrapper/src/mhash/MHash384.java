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

import java.net.URL;
import java.security.CodeSource;
import java.util.Locale;
import java.io.File;

public class MHash384 implements AutoCloseable {

	private Long m_handle;
	
	//--------------------------------------------------------------------
	// Static Initializer
	//--------------------------------------------------------------------
	
	static {
		initializeLibrary();
	}
	
	private static void initializeLibrary() {
		//Try user-defined path
		final String libraryPath = getProperty("mhash384.library.path");
		if(libraryPath != null) {
			if(tryLoadLibrary(libraryPath, true)) {
				return; /*success*/
			}
		}

		//Try user-defined name
		final String libraryName = getProperty("mhash384.library.name");
		if(libraryName != null) {
			if(tryLoadLibrary(libraryName, false)) {
				return; /*success*/
			}
		}
		
		//Handle system-specific stuff
		final boolean x64 = getProperty("sun.arch.data.model", "32").equals("64");
		final boolean win32 = getProperty("os.name", "linux").toLowerCase(Locale.ROOT).startsWith("windows");
		final String defaultName = "MHashJava384" + (x64 ? ".x64" : ".x86");
		final String defaultNameWithSuffix = defaultName + (win32 ? ".dll" : ".so");
		
		//Try to load from resource path
		final URL resourcePath = MHash384.class.getResource(defaultNameWithSuffix);
		if(resourcePath != null) {
			final String fullPath = new File(resourcePath.getFile()).getAbsolutePath(); 
			if(tryLoadLibrary(fullPath, true)) {
				return; /*success*/
			}
		}
		
		//Try to load from code source path
		final String sourcePath = getCodeSourcePath(MHash384.class);
		if(sourcePath != null) {
			final String fullPath = sourcePath + File.separatorChar + defaultNameWithSuffix;
			if(tryLoadLibrary(fullPath, true)) {
				return; /*success*/
			}	
		}
		
		//Fall-back method
		System.loadLibrary(defaultName);
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
	
	private static String getCodeSourcePath(final Class<?> clazz) {
		try {
			final CodeSource codeSource = clazz.getProtectionDomain().getCodeSource();
			if(codeSource != null) {
				final URL url = codeSource.getLocation();
				if(url != null) {
					final File fileName = new File(url.getFile());
					if(fileName.isFile()) {
						return fileName.getParentFile().getAbsolutePath();
					}
					return fileName.getAbsolutePath();
				}
			}
		}
		catch(Throwable e) {}
		return null;
	}
	
	private static boolean tryLoadLibrary(final String libname, final boolean fullPath) {
		boolean success = false;
		try {
			if(fullPath) {
				System.load(libname);
			}
			else {
				System.loadLibrary(libname);
			}
			success = true;
		}
		catch(Throwable e) {
			System.err.printf("Failed to load MHash-384 library from \"%s\" (full: %s)\n", libname, Boolean.toString(fullPath));
			success = false;
		}
		return success;
	}
}
