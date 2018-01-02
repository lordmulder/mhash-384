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

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.KeyEventDispatcher;
import java.awt.KeyboardFocusManager;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JTextField;
import javax.swing.SwingWorker;
import javax.swing.border.EmptyBorder;

import com.muldersoft.mhash384.MHash384;
import com.muldersoft.mhash384.MHash384.ByteString;

public class ExampleApp extends JFrame {

	private static final long serialVersionUID = -3576679013210278556L;

	public ExampleApp() {
		initUI();
		
		addWindowListener(new WindowListener() {
			@Override
			public void windowOpened(WindowEvent e) {
				try {
					final List<Integer> version = MHash384.getVersion();
					setTitle(String.format("MHashJava384 - Example App v%d.%d.%d", version.get(0), version.get(1), version.get(2)));
				}
				catch (Throwable err) {
					err.printStackTrace();
					while(err != null) {
						JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);
						err = err.getCause();
					}
					dispose();
				}
			}

			@Override public void windowClosing(WindowEvent e) {}
			@Override public void windowClosed(WindowEvent e) {}
			@Override public void windowIconified(WindowEvent e) {}
			@Override public void windowDeiconified(WindowEvent e) {}
			@Override public void windowActivated(WindowEvent e) {}
			@Override public void windowDeactivated(WindowEvent e) {}
		});
	}
	
	private void initUI() {
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setTitle("MHashJava384 - Example App [Launching]");
		setSize(800, 384);
		setMinimumSize(getSize());
		setLocationRelativeTo(null);
				
		final JTextField editFile = new JTextField();
		final JTextField editHash = new JTextField();
		editFile.setEditable(false);
		editHash.setEditable(false);
		editFile.setText("Please click \"Choose File\" to select your input file!");
		
		final JProgressBar progressBar = new JProgressBar();
		progressBar.setMinimum(0);
		progressBar.setMaximum(100);
		progressBar.setValue(0);
		
		final JButton buttonBrowse = new JButton("Choose File");
		final JButton buttonExecute = new JButton("Compute Hash");
		buttonExecute.setEnabled(false);
		buttonBrowse.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				if(browseForFile(editFile, editHash, progressBar)) {
					buttonExecute.setEnabled(true);
					buttonExecute.requestFocus();
				}
			}
		});
		buttonExecute.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				computeHashAsync(editFile.getText().trim(), editHash, progressBar, new JButton[] { buttonBrowse, buttonExecute });
			}
		});
		
		final JPanel body = new JPanel();
		body.setLayout(new GridLayout(8, 1));
		body.setBorder(new EmptyBorder(new Insets(12,12,12,12)));
		body.add(new JLabel("Input File:"));
		body.add(editFile);
		body.add(new JPanel());
		body.add(new JLabel("File Digest:"));
		body.add(editHash);
		body.add(new JPanel());
		body.add(new JPanel());
		body.add(progressBar);

		final JPanel buttonBar = new JPanel();
		buttonBar.setLayout(new FlowLayout(FlowLayout.RIGHT, 12, 12));
		buttonBar.add(buttonBrowse);
		buttonBar.add(buttonExecute);
		
		final Container content = getContentPane();
		content.setLayout(new BorderLayout());
		content.add(body, BorderLayout.NORTH);
		content.add(buttonBar, BorderLayout.SOUTH);
		
		KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(new KeyEventDispatcher() {
			@Override
			public boolean dispatchKeyEvent(final KeyEvent e) {
				if((e.getKeyCode() == KeyEvent.VK_F12) && (e.getID() == KeyEvent.KEY_PRESSED)) {
					runSelfTest(new JButton[] { buttonBrowse, buttonExecute });
					return true;
				}
				return false;
			}
		});
	}
	
	private boolean browseForFile(final JTextField editFile, final JTextField editHash, final JProgressBar progressBar) {
		final JFileChooser chooser = new JFileChooser();
		boolean result = false;
		if(chooser.showOpenDialog(ExampleApp.this) == JFileChooser.APPROVE_OPTION) {
			try {
				editFile.setText(chooser.getSelectedFile().getCanonicalPath());
				editHash.setText("Please click \"Compute Hash\" to begin computation!");
				progressBar.setValue(progressBar.getMinimum());
				result = true;
			}
			catch (Throwable err) {
				err.printStackTrace();
				JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.WARNING_MESSAGE);
			}
		}
		return result;
	}
	
	private void computeHashAsync(final String path, final JTextField editHash, final JProgressBar progress, final JButton[] buttons) {
		try {
			final File inputFile = new File(path);
			if(!(inputFile.exists() && inputFile.isFile())) {
				JOptionPane.showMessageDialog(ExampleApp.this, "Input file could not be found!", "Not Found", JOptionPane.WARNING_MESSAGE);
				return;
			}
			final HashWorker worker = createHashWorker(editHash, progress, buttons, inputFile);
			for(final JButton button : buttons) {
				button.setEnabled(false);
			}
			editHash.setText("Working on it, please be patient...");
			worker.execute();
		}
		catch (Throwable err) {
			err.printStackTrace();
			JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.WARNING_MESSAGE);
		}
	}

	private HashWorker createHashWorker(final JTextField editHash, final JProgressBar progress, final JButton[] buttons, final File inputFile) {
		return new HashWorker(this, inputFile) {
			@Override
			protected void done()
			{
				try {
					editHash.setText(this.get());
				}
				catch (Exception err) {
					err.printStackTrace();
					editHash.setText(err.getMessage());
				}
				finally {
					for(final JButton button : buttons) {
						button.setEnabled(true);
					}
				}
			}
			
			@Override
			protected void process(List<Integer> chunks)
			{
				progress.setValue(maxValue(chunks));
			}
		};
	}
	
	private void runSelfTest(final JButton[] buttons) {
		System.out.println("[SELF-TEST]");
		for(final JButton button : buttons) {
			button.setEnabled(false);
		}
		final SwingWorker<Boolean, Void> worker = new SwingWorker<Boolean, Void>() {
			@Override
			protected Boolean doInBackground() throws Exception {
				MHash384.selfTest();
				return true;
			}
			
			@Override
			protected void done() {
				try { 
					if(get()) { /*check result*/
						JOptionPane.showMessageDialog(ExampleApp.this, "Self-test completed successfully.", "Self-test", JOptionPane.INFORMATION_MESSAGE);
					}
				}
				catch (Throwable err) {
					err.printStackTrace();
					while(err != null) {
						JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);
						err = err.getCause();
					}
				}
				finally {
					for(final JButton button : buttons) {
						button.setEnabled(true);
					}
				}
			}
		};
		worker.execute();
	}
	
	private static class HashWorker extends SwingWorker<String, Integer> {
		private final File inputFile;
		private final Component parent;
		
		public HashWorker(final Component parent, File inputFile) {
			this.parent = parent;
			this.inputFile = inputFile;
		}

		@Override
		protected String doInBackground() throws FileNotFoundException, IOException {
			try {
				final double totalSize = inputFile.length();
				final long timeBegin = System.currentTimeMillis();
				final MHash384 mhash384 = new MHash384();
				try(BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(inputFile))) {
					long processed = 0;
					final byte[] buffer = new byte[4096];
					int count;
					do {
						count = inputStream.read(buffer);
						if(count > 0) {
							mhash384.update(buffer, 0, count);
							processed += count;
							publish((int)Math.round((((double)processed) / totalSize) * 100.0));
						}
					}
					while(count == buffer.length);
				}
				final ByteString digest = mhash384.digest();
				final long totaltime = System.currentTimeMillis() - timeBegin;
				System.out.printf("Operation completed after %f seconds!\n", (double)totaltime / 1000.0);
				return digest.toString();
			}
			catch(Throwable err) {
				while(err != null) {
					JOptionPane.showMessageDialog(parent, getMessage(err), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);
					err = err.getCause();
				}
				return null;
			}
		}
	}
	
	private static int maxValue(final List<Integer> chunks) {
		int result = Integer.MIN_VALUE;
		for(final Integer value : chunks) {
			result = Math.max(result, value.intValue());
		}
		return result;
	}
	
	private static String getMessage(Throwable err) {
		final String message = err.getMessage();
		return ((message != null) && (!message.isEmpty())) ? message : err.getClass().getName();
	}
}
