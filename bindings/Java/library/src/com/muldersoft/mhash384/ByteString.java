/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 for Java 1.7+                                                                        */
/* Copyright(c) 2016-2017 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

package com.muldersoft.mhash384;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Iterator;

public final class ByteString implements Iterable<Byte> {
	
	private final byte[] data;

	public ByteString(final byte[] data) {
		this.data = Arrays.copyOf(data, data.length);
	}

	public ByteString(final String data) {
		this.data = data.getBytes(StandardCharsets.ISO_8859_1);
	}
	
	public final byte at(final int index) {
		return data[index];
	}

	public final int size() {
		return data.length;
	}

	public final byte[] toArray() {
		return Arrays.copyOf(data, data.length);
	}
	
	@Override
	public final Iterator<Byte> iterator() {
		return new Iterator<Byte>() {
			private int index = 0;
			@Override
			public final boolean hasNext() {
				return (index < data.length);
			}
			@Override
			public final Byte next() {
				return data[index++];
			}
		};
	}
}
