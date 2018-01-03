/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 for Microsoft.NET                                                                    */
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

using System;
using System.ComponentModel;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;
using System.Diagnostics;
using System.Globalization;
using System.Threading;

namespace MHashDotNet384.Example
{
    public partial class MainWindow : Window
    {
        private const int BUFF_SIZE = 4096;
        private readonly ManualResetEvent m_abortFlag = new ManualResetEvent(false);
        private delegate void ProgressHandler(double progress, double duration);

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            ushort major, minor, patch;
            MHash384.GetVersion(out major, out minor, out patch);
            Title += String.Format(" v{0:D}.{1:D}.{2:D}", major, minor, patch);
            KeyDown += new KeyEventHandler(KeyDownHandler);
        }

        private async void KeyDownHandler(object sender, KeyEventArgs e)
        {
            if (Button_Browse.IsEnabled && Button_Compute.IsEnabled)
            {
                if (e.Key == Key.F12)
                {
                    e.Handled = true;
                    await RunSelfTest();
                }
            }
            else
            {
                if (e.Key == Key.Escape)
                {
                    m_abortFlag.Set();
                }
            }
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            if(!(Button_Browse.IsEnabled && Button_Compute.IsEnabled))
            {
                e.Cancel = true;
            }
        }

        private void Button_BrowseClick(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dialog = new Microsoft.Win32.OpenFileDialog();
            bool? ok = dialog.ShowDialog();
            if(ok.HasValue && ok.Value)
            {
                Edit_FileName.Text = dialog.FileName;
                Edit_HashValue.Text = String.Empty;
            }
        }

        private async void Button_ComputeClick(object sender, RoutedEventArgs e)
        {
            if(String.IsNullOrWhiteSpace(Edit_FileName.Text))
            {
                MessageBox.Show("Please select an input file first!", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            SetBusy(true);
            try
            {
                String fileName = Edit_FileName.Text;
                Edit_HashValue.Text = String.Empty;
                Edit_HashValue.Text = await Task.Run<String>(() =>
                {
                    CultureInfo culture = CultureInfo.CreateSpecificCulture("en-US");
                    return ComputeHash(this.Dispatcher, fileName, (p,t) =>
                    {
                        ProgressIndicator.Value = p;
                        Label_Timer.Text = t.ToString("0.00", culture);
                    });
                });
            }
            catch (OperationCanceledException err)
            {
                String message = String.IsNullOrEmpty(err.Message) ? err.GetType().FullName : err.Message;
                Edit_HashValue.Text = message;
                MessageBox.Show(message, "Error", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
            catch (Exception err)
            {
                String message = String.IsNullOrEmpty(err.Message) ? err.GetType().FullName : err.Message;
                Edit_HashValue.Text = "Failure: " + message;
                MessageBox.Show(message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            finally
            {
                SetBusy(false);
            }
        }

        private void SetBusy(bool busy)
        {
            if(busy)
            {
                ProgressIndicator.Value = 0.0;
                Label_Working.Visibility = Visibility.Visible;
                Label_Timer.Text = String.Empty;
                Button_Browse.IsEnabled = Button_Compute.IsEnabled = false;
                Mouse.OverrideCursor = Cursors.Wait;
            }
            else
            {
                ProgressIndicator.Value = 1.0;
                Label_Working.Visibility = Visibility.Hidden;
                Button_Browse.IsEnabled = Button_Compute.IsEnabled = true;
                Mouse.OverrideCursor = null;
            }
        }

        private String ComputeHash(Dispatcher dispatcher, String fileName, ProgressHandler progressHandler)
        {
            MHash384 digest = new MHash384();
            FileInfo fileInfo = new FileInfo(fileName);
            Stopwatch stopWatch = new Stopwatch();
            m_abortFlag.Reset();
            using (BufferedStream stream = new BufferedStream(fileInfo.OpenRead(), BUFF_SIZE))
            {
                double progress = 0.0;
                ushort update = 0;
                Action updateAction = new Action(() => progressHandler(progress, stopWatch.ElapsedMilliseconds / 1000.0));
                Task updateTask = null;
                stopWatch.Start();
                for (;;)
                {
                    if ((++update & 0x1FF) == 0)
                    {
                        if(m_abortFlag.WaitOne(TimeSpan.Zero))
                        {
                            throw new OperationCanceledException("The operation was cancelled by the user!");
                        }
                        AwaitTaskSynchronously(updateTask);
                        progress = ((double)stream.Position) / ((double)fileInfo.Length);
                        updateTask = dispatcher.BeginInvoke(DispatcherPriority.Background, updateAction).Task;
                    }
                    if(!digest.Update(stream, BUFF_SIZE))
                    {
                        break;  /*EOF*/
                    }
                }
                stopWatch.Stop();
                AwaitTaskSynchronously(updateTask);
                dispatcher.Invoke(() => progressHandler(1.0, stopWatch.ElapsedMilliseconds / 1000.0));
                return digest.ToString();
            }
        }

        private async Task RunSelfTest()
        {
            SetBusy(true);
            m_abortFlag.Reset();
            try
            {
                await Task.Run(() => MHash384.SelfTest(m_abortFlag));
                String message = "Self-test completed successfully.";
                Edit_HashValue.Text = message;
                MessageBox.Show(message, "Self-test", MessageBoxButton.OK, MessageBoxImage.Information);
            }
            catch (OperationCanceledException err)
            {
                String message = String.IsNullOrEmpty(err.Message) ? err.GetType().FullName : err.Message;
                Edit_HashValue.Text = message;
                MessageBox.Show(message, "Error", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
            catch (Exception err)
            {
                String message = String.IsNullOrEmpty(err.Message) ? err.GetType().FullName : err.Message;
                Edit_HashValue.Text = "Failure: " + message;
            }
            finally
            {
                SetBusy(false);
            }
        }

        private static void AwaitTaskSynchronously(Task task)
        {
            if (!(Object.ReferenceEquals(task, null) || task.IsCompleted))
            {
                task.Wait();
            }
        }
    }
}
