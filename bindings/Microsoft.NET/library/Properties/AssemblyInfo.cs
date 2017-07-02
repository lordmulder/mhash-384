/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 for Microsoft.NET                                                                    */
/* Copyright(c) 2016-2017 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

using System.Reflection;
using System.Runtime.InteropServices;

// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle("MHashDotNet384 Library")]
[assembly: AssemblyDescription("MHash-384 for Microsoft.NET")]
[assembly: AssemblyConfiguration(VERSION_INFO.CONFIG)]
[assembly: AssemblyCompany("Muldersoft.com")]
[assembly: AssemblyProduct("MHash-384")]
[assembly: AssemblyCopyright("Copyright © 2016-2017 LoRd_MuldeR <mulder2@gmx.de>")]
[assembly: AssemblyTrademark("MHash-384")]
[assembly: AssemblyCulture("")]

// Setting ComVisible to false makes the types in this assembly not visible 
// to COM components.  If you need to access a type in this assembly from 
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("84c78e66-08da-4356-9ad3-c090e9f4d3a7")]

// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version 
//      Build Number
//      Revision
//
// You can specify all the values or you can default the Build and Revision Numbers 
// by using the '*' as shown below:
// [assembly: AssemblyVersion("1.0.*")]
[assembly: AssemblyVersion(VERSION_INFO.VERSION_MAJOR + "." + VERSION_INFO.VERSION_MINOR + ".*")]
[assembly: AssemblyFileVersion(VERSION_INFO.VERSION_MAJOR + "." + VERSION_INFO.VERSION_MINOR + "." + VERSION_INFO.VERSION_PATCH + ".0")]
