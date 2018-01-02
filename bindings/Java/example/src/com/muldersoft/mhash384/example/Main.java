/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 for Java 1.7+                                                                        */
/* Copyright(c) 2016-2018 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

package com.muldersoft.mhash384.example;

import java.util.Locale;

import javax.swing.UIManager;

public class Main {
	//--------------------------------------------------------------------
	// MAIN
	//--------------------------------------------------------------------
	
	public static void main(String[] args) {
		initLookAndFeel();
		ExampleApp app = new ExampleApp();
		app.setVisible(true);
	}

	//--------------------------------------------------------------------
	// Initialization
	//--------------------------------------------------------------------

	private static void initLookAndFeel() {
		try {
			if(hasPrefix(getProperty("os.name", "windows"), "linux")) {
				javax.swing.UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel");
				return; /*success*/
			}
			if(hasPrefix(getProperty("os.name", "linux"), "windows")) {
				javax.swing.UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
				return; /*success*/
			}
		} 
		catch (Throwable e) {
			System.err.println("Failed to apply the system-specific 'native' look and feel!");
		}
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} 
		catch (Throwable e) {
			System.err.println("Failed to apply the system-specific 'native' look and feel!");
		}
	}

	//--------------------------------------------------------------------
	// Utility Functions
	//--------------------------------------------------------------------
	
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
	
	private static boolean hasPrefix(final String str, final String prefix) {
		final String clean =  str.toLowerCase(Locale.ENGLISH).trim();
		return clean.startsWith(prefix.toLowerCase(Locale.ENGLISH).trim());
	}
}
