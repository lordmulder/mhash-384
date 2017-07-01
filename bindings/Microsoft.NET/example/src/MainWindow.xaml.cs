/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 for Microsoft.NET                                                                    */
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

using Microsoft.Win32;
using System;
using System.ComponentModel;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Globalization;

namespace MHashDotNet384.Example
{
    public partial class MainWindow : Window
    {
        const int BUFF_SIZE = 4096;

        delegate void ProgressHandler(double progress, double duration);

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            ushort major, minor, patch;
            MHashDotNet384.MHash384.GetVersion(out major, out minor, out patch);
            Title += String.Format(" v{0:D}.{1:D}.{2:D}", major, minor, patch);
            KeyDown += new KeyEventHandler(KeyDownHandler);
        }

        private async void KeyDownHandler(object sender, KeyEventArgs e)
        {
            if (Button_Browse.IsEnabled && Button_Compute.IsEnabled)
            {
                if(e.Key == Key.F12)
                {
                    SetBusy(true);
                    try
                    {
                        await Task.Run(() => MHash384.SelfTest());
                        MessageBox.Show("Self-test completed successfully", "Self-test", MessageBoxButton.OK, MessageBoxImage.Information);
                    }
                    catch (Exception err)
                    {
                        MessageBox.Show(String.IsNullOrEmpty(err.Message) ? err.GetType().FullName : err.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                    finally
                    {
                        SetBusy(false);
                    }
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
            OpenFileDialog dialog = new OpenFileDialog();
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
            catch(Exception err)
            {
                MessageBox.Show(String.IsNullOrEmpty(err.Message) ? err.GetType().FullName : err.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
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
                        awaitTaskSynchronously(updateTask);
                        progress = ((double)stream.Position) / ((double)fileInfo.Length);
                        updateTask = dispatcher.BeginInvoke(DispatcherPriority.Background, updateAction).Task;
                    }
                    if(!digest.Update(stream, BUFF_SIZE))
                    {
                        break;  /*EOF*/
                    }
                }
                stopWatch.Stop();
                awaitTaskSynchronously(updateTask);
                dispatcher.Invoke(() => progressHandler(1.0, stopWatch.ElapsedMilliseconds / 1000.0));
                return digest.ToString();
            }
        }

        private static void awaitTaskSynchronously(Task task)
        {
            if (!(Object.ReferenceEquals(task, null) || task.IsCompleted))
            {
                task.Wait();
            }
        }
    }
}
