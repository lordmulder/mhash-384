/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Language bindings for Microsoft.NET                                                */
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
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace MHashDotNet.Example
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            uint major, minor, patch;
            MHashDotNet.MHash384.Version(out major, out minor, out patch);
            Title += String.Format(" v{0:D}.{1:D}.{2:D}", major, minor, patch);
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

        private void Button_ComputeClick(object sender, RoutedEventArgs e)
        {
            if(String.IsNullOrWhiteSpace(Edit_FileName.Text))
            {
                MessageBox.Show("Please select an input file first!", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
            try
            {
                Edit_HashValue.Text = String.Empty;
                ProgressIndicator.Value = 0.0;
                Label_Working.Visibility = Visibility.Visible;
                Button_Browse.IsEnabled = Button_Compute.IsEnabled = false;
                Mouse.OverrideCursor = Cursors.Wait; FileInfo info = new FileInfo(Edit_FileName.Text);
                using (FileStream fs = info.OpenRead())
                {
                    using (MHashDotNet.MHash384 digest = new MHashDotNet.MHash384())
                    {
                        short update = 0;
                        byte[] buffer = new byte[4096];
                        while (true)
                        {
                            if((update++ & 0x3FF) == 0)
                            {
                                ProgressIndicator.Value = (double)fs.Position / (double)info.Length;
                                DispatchPendingEvents();
                            }
                            int count = fs.Read(buffer, 0, buffer.Length);
                            if (count > 0)
                            {
                                digest.Update(buffer, 0, count);
                                continue;
                            }
                            break;
                        }
                        Edit_HashValue.Text = BitConverter.ToString(digest.GetResult()).Replace("-", "");
                    }
                }
            }
            catch(Exception err)
            {
                MessageBox.Show(err.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            finally
            {
                ProgressIndicator.Value = 1.0;
                Label_Working.Visibility = Visibility.Hidden;
                Button_Browse.IsEnabled = Button_Compute.IsEnabled = true;
                Mouse.OverrideCursor = null;
            }
        }

        private readonly Action dummyAction = new Action(() => { });

        private void DispatchPendingEvents()
        {
            Dispatcher.Invoke(dummyAction, DispatcherPriority.ContextIdle, null);
        }
    }
}
