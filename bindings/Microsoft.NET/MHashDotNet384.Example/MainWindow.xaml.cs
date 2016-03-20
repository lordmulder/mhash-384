using Microsoft.Win32;
using System;
using System.Collections.Generic;
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
            Edit_HashValue.Text = String.Empty;
            Label_Working.Visibility = Visibility.Visible;
            Button_Browse.IsEnabled = Button_Compute.IsEnabled = false;
            Mouse.OverrideCursor = Cursors.Wait;
            try
            {
                byte[] buffer = new byte[4096];
                using (FileStream fs = File.OpenRead(Edit_FileName.Text))
                {
                    using (MHashDotNet.MHash384 digest = new MHashDotNet.MHash384())
                    {
                        while (true)
                        {
                            Dispatcher.Invoke(new Action(() => {}), DispatcherPriority.ContextIdle, null);
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
                Label_Working.Visibility = Visibility.Hidden;
                Button_Browse.IsEnabled = Button_Compute.IsEnabled = true;
                Mouse.OverrideCursor = null;
            }
        }
    }
}
