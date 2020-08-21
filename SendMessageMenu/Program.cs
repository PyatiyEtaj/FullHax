using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SendMessageMenu
{
    class Program
    {
        #region WinApi
        internal static class Kernel32
        {
            [System.Runtime.InteropServices.DllImport("kernel32", CharSet = System.Runtime.InteropServices.CharSet.Auto, ExactSpelling = false)]
            public static extern int GetDiskFreeSpaceEx(string lpDirectoryName, out ulong lpFreeBytesAvailable, out ulong lpTotalNumberOfBytes, out ulong lpTotalNumberOfFreeBytes);
            [System.Runtime.InteropServices.DllImport("kernel32.dll", SetLastError = true)]
            public static extern System.IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);
            [System.Runtime.InteropServices.DllImport("kernel32.dll")]
            public static extern bool ReadProcessMemory(int hProcess, int lpBaseAddress, byte[] lpBuffer, int dwSize, ref int lpNumberOfBytesRead);
            [System.Runtime.InteropServices.DllImport("kernel32.dll")]
            [return: System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.Bool)]
            public static extern bool GetPhysicallyInstalledSystemMemory(out long TotalMemoryInKilobytes);
            public delegate System.IntPtr HOOKPROC(int nCode, System.IntPtr wParam, System.IntPtr lParam);
            [System.Runtime.InteropServices.DllImport("kernel32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto, SetLastError = true)]
            public static extern System.IntPtr GetModuleHandle(string lpModuleName);
            [System.Runtime.InteropServices.DllImport("kernel32.dll", CharSet = System.Runtime.InteropServices.CharSet.Ansi, SetLastError = true)]
            public static extern HOOKPROC GetProcAddress(System.IntPtr hModule, string procName);
            [System.Runtime.InteropServices.DllImport("kernel32.dll", CharSet = System.Runtime.InteropServices.CharSet.Ansi, SetLastError = true)]
            public static extern System.IntPtr LoadLibrary(string lpFileName);
        }
        internal static class User32
        {
            [DllImport("user32.dll")]
            public static extern System.IntPtr LoadCursorFromFile(string filename);
            [DllImport("user32.dll")]
            public static extern bool SetWindowText(System.IntPtr hWnd, string text);
            [DllImport("user32.dll", SetLastError = true)]
            public static extern System.IntPtr FindWindow(string lpClassName, string lpWindowName);
            [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
            public static extern uint GetWindowThreadProcessId(System.IntPtr hWnd, out uint lpdwProcessId);
            [System.Runtime.InteropServices.DllImport("user32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto, SetLastError = true)]
            public static extern System.IntPtr SetWindowsHookEx(int idHook, Kernel32.HOOKPROC lpfn, System.IntPtr hMod, uint dwThreadId);
            [System.Runtime.InteropServices.DllImport("user32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto, SetLastError = true)]
            public static extern bool UnhookWindowsHookEx(System.IntPtr hhk);
            [System.Runtime.InteropServices.DllImport("user32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto, SetLastError = true)]
            public static extern System.IntPtr CallNextHookEx(System.IntPtr hhk, int nCode, System.IntPtr wParam, System.IntPtr lParam);
            [DllImport("user32.dll", SetLastError = true)]
            public static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);
        }
        #endregion

        #region Utils
        private static readonly Random rand = new Random();
        private static uint rnd32(uint u)
        {
            uint x;
            if (u < int.MaxValue) return (uint)rand.Next((int)u + 1);
            do
            {
                do x = (uint)rand.Next(1 << 30) << 2;
                while (x > u);
                x |= (uint)rand.Next(1 << 2);
            }
            while (x > u);
            return x;
        }
        private static uint rnd32(uint u0, uint u1)
        {
            return u0 < u1 ? u0 + rnd32(u1 - u0) : u1 + rnd32(u0 - u1);
        }
        #endregion

        #region Declarations
        static IntPtr CF_WINDOW_HANDLE = IntPtr.Zero;
        class HMsg
        {
            public uint msg;
            public IntPtr key;
            public IntPtr value;
        }
        #endregion

        #region Core
        static void BroadcastGameMsg(HMsg h)
        {
            if (CF_WINDOW_HANDLE == IntPtr.Zero) CF_WINDOW_HANDLE = User32.FindWindow("CrossFire", "CROSSFIRE");
            User32.SendMessage(CF_WINDOW_HANDLE, h.msg, h.key, h.value);
        }

        static HMsg createHmsg(uint code, IntPtr key, IntPtr value) => new HMsg
        {
            msg = code,
            key = key,
            value = value
        };
        #endregion

        class Engine
        {
            public static void TurnHack(bool flag) => BroadcastGameMsg(createHmsg(1337, kc[0], flag ? kc[1] : kc[0]));
        }

        class Visuals
        {
            public static void TurnWallHack(bool flag) => BroadcastGameMsg(createHmsg(1338, kc[0], flag ? kc[1] : kc[0]));

            public static void TurnCrossHair(bool flag) => BroadcastGameMsg(createHmsg(1338, kc[1], flag ? kc[1] : kc[0]));
        }

        class HitBoxEditor
        {
            public static void SetHitBoxMul(Node node, int val) => BroadcastGameMsg(createHmsg(1339, kc[(int)node], new IntPtr(val)));
            public static void SetHitBoxDimsX(Node node, int valX) => BroadcastGameMsg(createHmsg(1340, kc[(int)node], new IntPtr(valX)));
            public static void SetHitBoxDimsY(Node node, int valY) => BroadcastGameMsg(createHmsg(1341, kc[(int)node], new IntPtr(valY)));
            public static void SetHitBoxDimsZ(Node node, int valZ) => BroadcastGameMsg(createHmsg(1342, kc[(int)node], new IntPtr(valZ)));
        }

        static List<IntPtr> kc = new List<IntPtr>();

        enum Node : int
        {
            GoldHead,
            SilverHead,
            Body,
            Pelvis
        }

        static void Main(string[] args)
        {
            for (int i = 0; i < 10; i++) kc.Add(new IntPtr(i));

            Visuals.TurnWallHack(true);

            HitBoxEditor.SetHitBoxMul(Node.Body, 3);

            HitBoxEditor.SetHitBoxDimsX(Node.GoldHead, 40);
            HitBoxEditor.SetHitBoxDimsY(Node.GoldHead, 40);
            HitBoxEditor.SetHitBoxDimsZ(Node.GoldHead, 40);
        }
    }
}
