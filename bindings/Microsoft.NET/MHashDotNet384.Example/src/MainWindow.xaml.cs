using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace MHashDotNet.Example
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
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
