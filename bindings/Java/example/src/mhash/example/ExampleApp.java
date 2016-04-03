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

package mhash.example;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

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

import mhash.MHash384;

public class ExampleApp extends JFrame {

	private static final long serialVersionUID = -3576679013210278556L;

	public ExampleApp() {
		initUI();
		
		addWindowListener(new WindowListener() {
			@Override
			public void windowOpened(WindowEvent e) {
				try {
					final MHash384.Version version = MHash384.getVersion();
					setTitle(String.format("MHashJava384 - Example App v%d.%d.%d", version.major, version.minor, version.patch));
				}
				catch (UnsatisfiedLinkError err) {
					err.printStackTrace();
					JOptionPane.showMessageDialog(ExampleApp.this, "Failed to load native MHash384 library!\n\nDETAILS:\n" + err.getMessage(), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);
					dispose();
				}
				catch (Throwable err) {
					err.printStackTrace();
					JOptionPane.showMessageDialog(ExampleApp.this, err.getMessage(), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);
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

		final JProgressBar progressBar = new JProgressBar();
		progressBar.setMinimum(0);
		progressBar.setMaximum(100);
		progressBar.setValue(0);
		
		final JButton buttonBrowse = new JButton("Choose File");
		buttonBrowse.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				final JFileChooser chooser = new JFileChooser();
				if(chooser.showOpenDialog(ExampleApp.this) == JFileChooser.APPROVE_OPTION) {
					try {
						editFile.setText(chooser.getSelectedFile().getCanonicalPath());
						editHash.setText("");
					}
					catch (Throwable err) {
						err.printStackTrace();
						JOptionPane.showMessageDialog(ExampleApp.this, err.getMessage(), err.getClass().getName(), JOptionPane.WARNING_MESSAGE);
					}
				}
			}
		});
		
		final JButton buttonExecute = new JButton("Compute Hash");
		buttonExecute.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				try {
					final File inputFile = new File(editFile.getText().trim());
					if(!(inputFile.exists() && inputFile.isFile())) {
						JOptionPane.showMessageDialog(ExampleApp.this, "Input file could not be found!", "Not Found", JOptionPane.WARNING_MESSAGE);
						return;
					}
					final SwingWorker<String,Object> worker = new HashWorker(inputFile);
					buttonBrowse.setEnabled(false);
					buttonExecute.setEnabled(false);
					worker.addPropertyChangeListener(new PropertyChangeListener() {
						@Override
						public void propertyChange(PropertyChangeEvent evt) {
							try {
								if(evt.getPropertyName().equals("progress")) {
									progressBar.setValue((Integer)evt.getNewValue());
								}
								else if(evt.getPropertyName().equals("state")) {
									if(((SwingWorker.StateValue)evt.getNewValue()) == SwingWorker.StateValue.DONE) {
										editHash.setText(worker.get());
										buttonBrowse.setEnabled(true);
										buttonExecute.setEnabled(true);
									}
								}
							}
							catch (Throwable err) {
								err.printStackTrace();
								JOptionPane.showMessageDialog(ExampleApp.this, err.getMessage(), err.getClass().getName(), JOptionPane.WARNING_MESSAGE);
							}
						}
					});
					worker.execute();
				}
				catch (Throwable err) {
					err.printStackTrace();
					JOptionPane.showMessageDialog(ExampleApp.this, err.getMessage(), err.getClass().getName(), JOptionPane.WARNING_MESSAGE);
				}
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
	}

	private class HashWorker extends SwingWorker<String, Object> {
		private final File inputFile;
		
		public HashWorker(File inputFile) {
			this.inputFile = inputFile;
		}

		@Override
		protected String doInBackground() throws FileNotFoundException, IOException {
			try {
				final double totalSize = inputFile.length();
				try(BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(inputFile))) {
					try(final MHash384 digest = new MHash384()) {
						long processed = 0;
						final byte[] buffer = new byte[4096];
						int count;
						do {
							count = inputStream.read(buffer);
							if(count > 0) {
								digest.update(buffer, 0, count);
								processed += count;
								setProgress((int)Math.round((((double)processed) / totalSize) * 100.0));
							}
						}
						while(count == buffer.length);
						return bytesToHex(digest.result());
					}
				}
			}
			catch(Throwable err) {
				err.printStackTrace();
				JOptionPane.showMessageDialog(ExampleApp.this, err.getMessage(), err.getClass().getName(), JOptionPane.WARNING_MESSAGE);
				return null;
			}
		}
		
		private String bytesToHex(final byte[] bytes) {
			final StringBuilder sb = new StringBuilder();
			for (final byte b : bytes) {
				sb.append(String.format("%02X", b));
			}
			return sb.toString();
		}
	}
}
