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
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace MHashDotNet.Example
{
    public partial class MainWindow : Window
    {
        delegate void ProgressHandler(double progress);

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
                    return ComputeHash(fileName, (p) => ProgressIndicator.Value = p);
                });
            }
            catch(Exception err)
            {
                MessageBox.Show(err.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
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

        private String ComputeHash(String fileName, ProgressHandler progressHandler)
        {
            FileInfo info = new FileInfo(fileName);
            Dispatcher dispatcher = Application.Current.Dispatcher;
            using (FileStream fs = info.OpenRead())
            {
                using (MHashDotNet.MHash384 digest = new MHashDotNet.MHash384())
                {
                    short update = 0;
                    byte[] buffer = new byte[4096];
                    while (true)
                    {
                        if ((update++ & 0x3FF) == 0)
                        {
                            double progress = ((double)fs.Position) / ((double)info.Length);
                            dispatcher.Invoke(() => progressHandler(progress));
                        }
                        int count = fs.Read(buffer, 0, buffer.Length);
                        if (count > 0)
                        {
                            digest.Update(buffer, 0, count);
                            continue;
                        }
                        break;
                    }
                    return BitConverter.ToString(digest.GetResult()).Replace("-", "");
                }
            }
        }
    }
}
