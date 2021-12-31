using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace SharpSvn.MSBuild
{
    static class NativeMethods
    {
        [SuppressMessage("Microsoft.Usage", "CA2205:UseManagedEquivalentsOfWin32Api")] // We need the raw data instead of just the information
        [DllImport("version.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.U4)]
        static extern int GetFileVersionInfoSize(string pFilename, [MarshalAs(UnmanagedType.U4)] out int handle);

        public static int GetFileVersionInfoSize(string pFilename)
        {
            int tmpHandle; // Only set to 0
            return GetFileVersionInfoSize(pFilename, out tmpHandle);
        }

        [SuppressMessage("Microsoft.Usage", "CA2205:UseManagedEquivalentsOfWin32Api")] // We need the raw data instead of just the information
        [DllImport("version.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool GetFileVersionInfo(string pFilename, [MarshalAs(UnmanagedType.I4)]int handle, [MarshalAs(UnmanagedType.I4)]int len, [Out] byte[] data);

        public static bool GetFileVersionInfo(string pFilename, int len, out byte[] data)
        {
            data = new byte[len];
            return GetFileVersionInfo(pFilename, 0, len, data);
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern ResourceUpdateHandle BeginUpdateResource(string pFileName, [MarshalAs(UnmanagedType.Bool)]bool bDeleteExistingResources);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool UpdateResource(ResourceUpdateHandle hUpdate, IntPtr type, IntPtr name, ushort wLanguage, byte[] lpData, int cbData);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool EndUpdateResource(IntPtr hUpdate, [MarshalAs(UnmanagedType.Bool)] bool fDiscard);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool FreeLibrary(IntPtr hModule);

        [return: MarshalAs(UnmanagedType.Bool)]
        public delegate bool EnumResLangDelegate(IntPtr hModule, IntPtr lpszType, IntPtr lpszName, ushort wIDLanguage, IntPtr lParam);

    }

    class DllHandle : SafeHandle
    {
        public DllHandle()
            : base(IntPtr.Zero, true)
        {
        }

        protected override bool ReleaseHandle()
        {
            if (handle != IntPtr.Zero)
                return NativeMethods.FreeLibrary(handle);

            return true;
        }

        public override bool IsInvalid
        {
            get { return handle == IntPtr.Zero; }
        }
    }

    class ResourceUpdateHandle : SafeHandle
    {
        public ResourceUpdateHandle()
            : base(IntPtr.Zero, true)
        {
        }

        protected override bool ReleaseHandle()
        {
            if (handle != IntPtr.Zero)
                return NativeMethods.EndUpdateResource(handle, true);

            return true;
        }

        public override bool IsInvalid
        {
            get { return handle == IntPtr.Zero; }
        }

        public bool Commit()
        {
            IntPtr h = handle;
            if (h != IntPtr.Zero)
            {
                handle = IntPtr.Zero;
                if (!NativeMethods.EndUpdateResource(h, false))
                    handle = h;
                else
                    return true;
            }

            return false;
        }
    }
}
