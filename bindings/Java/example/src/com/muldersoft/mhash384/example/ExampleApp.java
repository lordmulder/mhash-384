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
import java.util.concurrent.Semaphore;
import java.util.function.Consumer;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JTextField;
import javax.swing.SwingWorker;
import javax.swing.WindowConstants;
import javax.swing.border.EmptyBorder;

import com.muldersoft.mhash384.MHash384;
import com.muldersoft.mhash384.MHash384.ByteString;

public class ExampleApp extends JFrame {

    private static final long serialVersionUID = -3576679013210278556L;

    public ExampleApp() {
        final JButton[] buttons = initUI();
        
        addWindowListener(new WindowListener() {
            @Override
            public void windowOpened(final WindowEvent e) {
                try {
                    final List<Integer> version = MHash384.getVersion();
                    setTitle(String.format("MHashJava384 - Example App v%d.%d.%d", version.get(0), version.get(1), version.get(2)));
                }
                catch (Throwable err) {
                    err.printStackTrace();
                    JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);
                    dispose();
                }
            }

            @Override
            public void windowClosing(final WindowEvent e) {
                for(final JButton button : buttons) {
                    if(button.isEnabled()) {
                        ExampleApp.this.dispose();
                        break;
                    }
                }
            }

            @Override
            public void windowClosed(final WindowEvent e) { }

            @Override
            public void windowIconified(final WindowEvent e) { }

            @Override
            public void windowDeiconified(final WindowEvent e) { }

            @Override
            public void windowActivated(final WindowEvent e) { }

            @Override
            public void windowDeactivated(final WindowEvent e) { }
        });
    }

    private JButton[] initUI() {
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setTitle("MHashJava384 - Example App [Launching]");
        setSize(800, 384);
        setMinimumSize(getSize());
        setLocationRelativeTo(null);
        setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
        
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
        final Semaphore abortFlag = new Semaphore(0);
        buttonExecute.setEnabled(false);
        buttonBrowse.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (browseForFile(editFile, editHash, progressBar)) {
                    buttonExecute.setEnabled(true);
                    buttonExecute.requestFocus();
                }
            }
        });
        buttonExecute.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                computeHashAsync(editFile.getText().trim(), editHash, progressBar, new JButton[] { buttonBrowse, buttonExecute }, abortFlag);
            }
        });

        final JPanel body = new JPanel();
        body.setLayout(new GridLayout(8, 1));
        body.setBorder(new EmptyBorder(new Insets(12, 12, 12, 12)));
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
                final boolean busy =  (!(buttonExecute.isEnabled() || buttonBrowse.isEnabled()));
                if ((e.getKeyCode() == KeyEvent.VK_F12) && (e.getID() == KeyEvent.KEY_PRESSED) && (!busy)) {
                    runSelfTest(editHash, progressBar, new JButton[] { buttonBrowse, buttonExecute }, abortFlag);
                    return true;
                }
                if ((e.getKeyCode() == KeyEvent.VK_ESCAPE) && (e.getID() == KeyEvent.KEY_PRESSED) && busy) {
                    abortFlag.release();
                    return true;
                }
                return false;
            }
        });
        
        return new JButton[] { buttonBrowse, buttonExecute };
    }

    private boolean browseForFile(final JTextField editFile, final JTextField editHash, final JProgressBar progressBar) {
        final JFileChooser chooser = new JFileChooser();
        boolean result = false;
        if (chooser.showOpenDialog(ExampleApp.this) == JFileChooser.APPROVE_OPTION) {
            try {
                editFile.setText(chooser.getSelectedFile().getCanonicalPath());
                editHash.setText("Please click \"Compute Hash\" to begin computation!");
                progressBar.setValue(progressBar.getMinimum());
                result = true;
            }
            catch (Throwable err) {
                err.printStackTrace();
                editHash.setText(getMessage(err = unwrapException(err)));
                JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);

            }
        }
        return result;
    }

    private void computeHashAsync(final String path, final JTextField editHash, final JProgressBar progress, final JButton[] buttons, final Semaphore abortFlag) {
        try {
            final File inputFile = new File(path);
            if (!(inputFile.exists() && inputFile.isFile())) {
                JOptionPane.showMessageDialog(ExampleApp.this, "Input file could not be found!", "Not Found", JOptionPane.WARNING_MESSAGE);
                return;
            }
            final HashWorker worker = createHashWorker(editHash, progress, buttons, inputFile, abortFlag);
            for (final JButton button : buttons) {
                button.setEnabled(false);
            }
            abortFlag.drainPermits();
            editHash.setText("Working on it, please be patient...");
            worker.execute();
        }
        catch (Throwable err) {
            err.printStackTrace();
            editHash.setText(getMessage(err = unwrapException(err)));
            JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);

        }
    }

    private HashWorker createHashWorker(final JTextField editHash, final JProgressBar progress, final JButton[] buttons, final File inputFile, final Semaphore abortFlag) {
        return new HashWorker(this, inputFile, abortFlag) {
            @Override
            protected void done(){
                try {
                    final String result = this.get();
                    if((result != null) && (result.length() > 0)) {
                        editHash.setText(result);
                    }
                }
                catch (Throwable err) {
                    err.printStackTrace();
                    editHash.setText(getMessage(err = unwrapException(err)));
                    JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);
                }
                finally {
                    for (final JButton button : buttons) {
                        button.setEnabled(true);
                    }
                }
            }

            @Override
            protected void process(final List<Integer> chunks) {
                progress.setValue(maxValue(chunks));
            }
        };
    }

    private void runSelfTest(final JTextField editHash, final JProgressBar progress, final JButton[] buttons, final Semaphore abortFlag) {
        System.out.println("[SELF-TEST]");
        for (final JButton button : buttons) {
            button.setEnabled(false);
        }
        final SwingWorker<Boolean, Integer[]> worker = new SwingWorker<Boolean, Integer[]>() {
            private final Integer[] lastProgress = new Integer[] { -1, -1 }; 
            
            @Override
            protected Boolean doInBackground() throws Exception {
                editHash.setText("Self-test is running, please be patient...");
                MHash384.selfTest(abortFlag, new Consumer<Integer[]>() {
                    @Override
                    public void accept(final Integer[] progress) {
                        publish(progress);
                    }
                });
                return true;
            }

            @Override
            protected void done() {
                try {
                    if (get()) { /* check result */
                        final String successMessage = "Self-test completed successfully.";
                        editHash.setText(successMessage);
                        JOptionPane.showMessageDialog(ExampleApp.this, successMessage, "Self-test", JOptionPane.INFORMATION_MESSAGE);
                    }
                    else {
                        throw new AssertionError("Self-test has failed. See log for details!");
                    }
                }
                catch (Throwable err) {
                    err.printStackTrace();
                    editHash.setText(getMessage(err = unwrapException(err)));
                    JOptionPane.showMessageDialog(ExampleApp.this, getMessage(err), err.getClass().getName(), JOptionPane.ERROR_MESSAGE);

                }
                finally {
                    for (final JButton button : buttons) {
                        button.setEnabled(true);
                    }
                }
            }
            
            @Override
            protected void process(final List<Integer[]> chunks) {
                final Integer[] value = maxValueEx(chunks);
                if(value.length >= 3) {
                    if((lastProgress[0] != value[0]) || (lastProgress[1] != value[1])) {
                        editHash.setText(String.format("Self-test is running (step %d of %d), please be patient...", value[1] + 1, value[0]));
                        lastProgress[0] = value[0];
                        lastProgress[1] = value[1];
                    }
                    progress.setValue(value[2]);
                }
            }
        };
        worker.execute();
    }

    private static class HashWorker extends SwingWorker<String, Integer> {
        private final File inputFile;
        private final Semaphore abortFlag;
        
        public HashWorker(final Component parent, final File inputFile, final Semaphore abortFlag) {
            this.inputFile = inputFile;
            this.abortFlag = abortFlag;
        }

        @Override
        protected String doInBackground() throws FileNotFoundException, IOException, InterruptedException {
            final double totalSize = inputFile.length();
            final long timeBegin = System.currentTimeMillis();
            final MHash384 mhash384 = new MHash384();
            try (BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(inputFile))) {
                long processed = 0;
                final byte[] buffer = new byte[4096];
                int count;
                do {
                    count = inputStream.read(buffer);
                    if (count > 0) {
                        mhash384.update(buffer, 0, count);
                        processed += count;
                        publish((int) Math.round((((double) processed) / totalSize) * 100.0));
                    }
                    if(abortFlag.tryAcquire()) {
                        throw new InterruptedException("Operation aborted by user!");
                    }
                }
                while (count == buffer.length);
            }
            final ByteString digest = mhash384.digest();
            final long totaltime = System.currentTimeMillis() - timeBegin;
            System.out.printf("Operation completed after %f seconds!\n", (double) totaltime / 1000.0);
            return digest.toString();
        }
    }

    private static int maxValue(final List<Integer> chunks) {
        int result = Integer.MIN_VALUE;
        for (final Integer value : chunks) {
            result = Math.max(result, value.intValue());
        }
        return result;
    }

    private static Integer[] maxValueEx(final List<Integer[]> chunks) {
        if(!chunks.isEmpty()) {
            Integer[] result = chunks.get(0);
            for (final Integer[] chunk : chunks) {
                for(int i = 0; i < Math.min(result.length, chunk.length); ++i) {
                    if(chunk[i] > result[i]) {
                        result = chunk;
                        break;
                    }
                    else if(chunk[i] < result[i]) {
                        break;
                    }
                }
            }
            return result;
        }
        else {
            return new Integer[0];
        }
    }
    
    private Throwable unwrapException(final Throwable err) {
        final Throwable next = err.getCause();
        if(next != null) {
            return unwrapException(next);
        }
        return err;
    }

    private static String getMessage(final Throwable err) {
        final String message = err.getMessage();
        return ((message != null) && (!message.isEmpty())) ? message : err.getClass().getName();
    }
}
