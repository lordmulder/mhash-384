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
	// Public Methods
	//--------------------------------------------------------------------

	public MHash384() {
		m_handle = Internals.createInstance();
	}
	
	public void update(final byte[] data, final int offset, final int len) {
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
	
	protected void finalize() {
		close();
	}

	//--------------------------------------------------------------------
	// Native Methods
	//--------------------------------------------------------------------

	private static class Internals {
		native static long createInstance();
		native static void update(final long handle, final byte[] data, final int offset, final int len);
		native static byte[] finalize(final long handle);
		native static void destroyInstance(final long handle);
	}

}
