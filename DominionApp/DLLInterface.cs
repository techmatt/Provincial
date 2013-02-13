using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace BaseCodeApp
{
    class DLLInterface
    {
        const string BaseCodeDLL = "BaseCode.dll";
        [DllImport(BaseCodeDLL)]
        private static extern IntPtr BCInit();
        [DllImport(BaseCodeDLL)]
        private static extern UInt32 BCProcessCommand(IntPtr context, [In, MarshalAs(UnmanagedType.LPStr)] String command);
        [DllImport(BaseCodeDLL)]
        private static extern IntPtr BCQueryBitmapByName(IntPtr context, [In, MarshalAs(UnmanagedType.LPStr)] String bitmapName);
        [DllImport(BaseCodeDLL)]
        private static extern IntPtr BCQueryStringByName(IntPtr context, [In, MarshalAs(UnmanagedType.LPStr)] String stringName);
        [DllImport(BaseCodeDLL)]
        private static extern double BCQueryDoubleByName(IntPtr context, [In, MarshalAs(UnmanagedType.LPStr)] String stringName);

        private static IntPtr baseCodeDLLContext = (IntPtr)0;

        public static void Init()
        {
            if (baseCodeDLLContext == (IntPtr)0)
            {
                baseCodeDLLContext = BCInit();
            }
        }

        public static String GetString(String stringName)
        {
            IntPtr stringPtr = BCQueryStringByName(baseCodeDLLContext, stringName);
            if (stringPtr == (IntPtr)0)
            {
                return null;
            }
            else
            {
                return Marshal.PtrToStringAnsi(stringPtr);
            }
        }

        public static double GetDouble(String stringName)
        {
            return BCQueryDoubleByName(baseCodeDLLContext, stringName);
        }

        public static void ProcessCommand(String command)
        {
            BCProcessCommand(baseCodeDLLContext, command);
        }
    }
}
