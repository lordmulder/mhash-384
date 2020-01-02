/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Simple fast portable secure hashing library                                        */
/* Copyright(c) 2016-2020 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;

namespace com.muldersoft.mhash384.test
{
    class TestDriver
    {
        static void Main(string[] args)
        {
            Trace.Listeners.Add(new ConsoleTraceListener());
            RunAllTests<MHash384Test>();
        }

        private static void RunAllTests<T>()
        {
            T instance = (T) Activator.CreateInstance(typeof(T));
            foreach (var method in GetMethodsByAttribute<ClassInitializeAttribute>(typeof(T)))
            {
                method.Invoke(null, new object[] { null });
            }
            foreach (var method in GetMethodsByAttribute<TestMethodAttribute>(typeof(T)))
            {
                try
                {
                    method.Invoke(instance, null);
                }
                catch (TargetInvocationException targetInvocationException)
                {
                    if (typeof(AssertFailedException).IsAssignableFrom(targetInvocationException.GetType()))
                    {
                        throw targetInvocationException;
                    }
                }
            }
            foreach (var method in GetMethodsByAttribute<ClassCleanupAttribute>(typeof(T)))
            {
                method.Invoke(null, null);
            }
        }

        private static IEnumerable<MethodInfo> GetMethodsByAttribute<T>(Type type)
        {
            try
            {
                return type.GetMethods().Where((method) => 
                    method.GetCustomAttributes(false).Select((attr) => attr.GetType()).Contains(typeof(T)));
            }
            catch
            {
                return Enumerable.Empty<MethodInfo>();
            }
        }
    }
}
