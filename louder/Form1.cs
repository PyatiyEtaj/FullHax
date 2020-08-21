using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Management;
using System.Net;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace louder
{
    public partial class Form1 : Form
    {
        #region WINAPI
        public enum HookType : int
        {
            WH_JOURNALRECORD = 0,
            WH_JOURNALPLAYBACK = 1,
            WH_KEYBOARD = 2,
            WH_GETMESSAGE = 3,
            WH_CALLWNDPROC = 4,
            WH_CBT = 5,
            WH_SYSMSGFILTER = 6,
            WH_MOUSE = 7,
            WH_HARDWARE = 8,
            WH_DEBUG = 9,
            WH_SHELL = 10,
            WH_FOREGROUNDIDLE = 11,
            WH_CALLWNDPROCRET = 12,
            WH_KEYBOARD_LL = 13,
            WH_MOUSE_LL = 14
        }
        internal static class Kernel32
        {
            [DllImport("kernel32", CharSet = CharSet.Auto, ExactSpelling = false)]
            public static extern int GetDiskFreeSpaceEx(string lpDirectoryName, out ulong lpFreeBytesAvailable, out ulong lpTotalNumberOfBytes, out ulong lpTotalNumberOfFreeBytes);
            [DllImport("kernel32.dll", SetLastError = true)]
            public static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);
            [DllImport("kernel32.dll")]
            public static extern bool ReadProcessMemory(int hProcess, int lpBaseAddress, byte[] lpBuffer, int dwSize, ref int lpNumberOfBytesRead);
            [DllImport("kernel32.dll")]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool GetPhysicallyInstalledSystemMemory(out long TotalMemoryInKilobytes);
            public delegate IntPtr HOOKPROC(int nCode, IntPtr wParam, IntPtr lParam);
            [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            public static extern IntPtr GetModuleHandle(string lpModuleName);
            [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
            public static extern HOOKPROC GetProcAddress(IntPtr hModule, string procName);
            [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
            public static extern IntPtr LoadLibrary(string lpFileName);
        }
        internal static class User32
        {
            [DllImport("user32.dll")]
            public static extern IntPtr LoadCursorFromFile(string filename);
            [DllImport("user32.dll")]
            public static extern bool SetWindowText(IntPtr hWnd, string text);
            [DllImport("user32.dll", SetLastError = true)]
            public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
            [DllImport("user32.dll", SetLastError = true)]
            public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
            [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            public static extern IntPtr SetWindowsHookEx(HookType idHook, Kernel32.HOOKPROC lpfn, IntPtr hMod, uint dwThreadId);
            [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            public static extern bool UnhookWindowsHookEx(IntPtr hhk);
            [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            public static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode, IntPtr wParam, IntPtr lParam);
        }
        internal static class CoreDLL
        {
            [DllImport("coredll.dll")]
            public static extern bool PostThreadMessage(uint threadId, uint msg, ushort wParam, uint lParam);
        }
        public static IntPtr SetWHX(string dllName)
        {
            uint pid;
            IntPtr CFWindow;
            Console.WriteLine("waiting for crossfire...");
            while ((CFWindow = User32.FindWindow("CrossFire", "CROSSFIRE")) == IntPtr.Zero) Thread.Sleep(100);
            Thread.Sleep(5000);
            uint tid = User32.GetWindowThreadProcessId(CFWindow, out pid);
            IntPtr LibHandle = Kernel32.LoadLibrary(dllName);
            IntPtr Hook = User32.SetWindowsHookEx(HookType.WH_CALLWNDPROC, Kernel32.GetProcAddress(LibHandle, "HookProc"), LibHandle, tid);
            return Hook;
        }

        [DllImport("user32.dll")]
        private static extern IntPtr GetForegroundWindow();

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        public static extern IntPtr GetDesktopWindow();

        #endregion

        public Form1()
        {
            InitializeComponent();
            textBox1.Enabled = false;
            var response = Utilities.postRQ("echo");
            var encoding = ASCIIEncoding.ASCII;
            using (var reader = new System.IO.StreamReader(response.GetResponseStream(), encoding))
            {
                string responseText = reader.ReadToEnd();
            }
            if (response.StatusCode == HttpStatusCode.NotFound) textBox1.Text = $"Ваш ключ: {}";
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (Process.GetProcessesByName("crossfire.exe").Length > 0)
            {
                MessageBox.Show("Сначала запусти КФ!");
                return;
            }
            ensureHook();
        }

        private void removeFile(string filePath)
        {
            try
            {
                File.Delete(filePath);
            }
            catch (Exception) { }
        }
        private static IntPtr hHook;
        private static IntPtr setHook(string filePath) => hHook = SetWHX(filePath);
        private static bool removeHook() => User32.UnhookWindowsHookEx(hHook);
        private void ensureHook()
        {
            //var dllPath = $"{Path.GetTempFileName()}";
            //Utilities.saveFile(Utilities.postRQ("dll"), dllPath);
            var dllPath = "xxx.dll";
            if (File.Exists(dllPath)) setHook(dllPath);
            while (Process.GetProcessesByName("crossfire.exe").Length > 0) Thread.Sleep(1000);
            if (removeHook()) removeFile(dllPath);
        }
    }
    public class Utilities
    {
        private static string hostName = "https://safe-gorge-71800.herokuapp.com";
        private static NameValueCollection uniqueData = null;
        private static NameValueCollection initUniqueData()
        {
            if (uniqueData == null)
            {
                using (var curr = WindowsIdentity.GetCurrent())
                {
                    uniqueData = new NameValueCollection()
                    {
                        {
                            "X-UM", retrieveMOCField("SELECT * FROM Win32_BaseBoard", "SerialNumber")
                        },
                        {
                            "X-OS", retrieveMOCField("SELECT Caption FROM Win32_OperatingSystem", "Caption").Replace("Майкрософт", "")
                        },
                        {
                            "X-WU-N", curr.Name
                        },
                        {
                            "X-WU-S", curr.User.Value
                        }
                    };
                }
            }
            return uniqueData;
        }
        private static string retrieveMOCField(string query, string fieldName)
        {
            return (from x in new ManagementObjectSearcher(query)
                    .Get()
                    .Cast<ManagementObject>()
                    select x.GetPropertyValue(fieldName))
                    .FirstOrDefault()
                    .ToString();
        }
        public static HttpWebResponse postRQ(string methodName, string postData = "", string contentType = "")
        {
            var request = (HttpWebRequest)WebRequest.Create($"{hostName}/{methodName}");
            var postDataBytes = Encoding.ASCII.GetBytes(postData);
            request.Headers.Add(initUniqueData());
            request.UserAgent = "ld13";
            request.Method = "POST";
            request.ContentType = contentType;
            request.ContentLength = postDataBytes.Length;
            using (var stream = request.GetRequestStream())
            {
                stream.Write(postDataBytes, 0, postDataBytes.Length);
            }
            return (HttpWebResponse)request.GetResponse();
        }
        public static void saveFile(HttpWebResponse response, string filePath)
        {
            using (var stream = new FileStream(filePath, FileMode.Create, FileAccess.Write, FileShare.None))
            {
                response.GetResponseStream().CopyTo(stream);
            }
        }
    }
}
