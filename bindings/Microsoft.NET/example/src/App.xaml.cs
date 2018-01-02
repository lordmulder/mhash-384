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
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Windows;

namespace MHashDotNet384.Example
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
    }

    internal static class Startup
    {
        [STAThread()]
        [System.Diagnostics.DebuggerNonUserCode()]
        public static void Main()
        {
#if DEBUG
            LaunchApplication();
#else
            try
            {
                AppDomain.CurrentDomain.UnhandledException += UnhandeledExceptionHandler;
                LaunchApplication();
            }
            catch (Exception e)
            {
                UnhandeledExceptionHandler(e);
            }
#endif
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static void LaunchApplication()
        {
            App app = new App();
            app.InitializeComponent();
            app.Run();
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static void UnhandeledExceptionHandler(object sender, UnhandledExceptionEventArgs e)
        {
            UnhandeledExceptionHandler(e.ExceptionObject as Exception);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static void UnhandeledExceptionHandler(Exception e)
        {
            try
            {
                const String PREFIX = "A critical error has occurred, the application needs to terminate";
                String text = String.IsNullOrWhiteSpace(e.Message) ? String.Concat(PREFIX, '!') : String.Concat(PREFIX, ":\n\n", e.Message);
                MessageBox.Show(text, AppDomain.CurrentDomain.FriendlyName, MessageBoxButton.OK, MessageBoxImage.Error);
            }
            finally
            {
                Environment.Exit(666);
            }
        }
    }
}
