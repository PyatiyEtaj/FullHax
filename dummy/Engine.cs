using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Management;
using System.Net;
using System.IO;
using System.Collections.Specialized;
using System.Security.Principal;
using dummy;
using System.Threading;
using System.Security.Cryptography;
using pj_ld13.Security;

namespace pj_ld13
{
    namespace Security
    {
        public class FingerPrint
        {
            private static string fingerPrint = string.Empty;
            public static string Value()
            {
                if (string.IsNullOrEmpty(fingerPrint))
                {
                    fingerPrint = GetHash("CPU >> " + cpuId() + "\nBIOS >> " +
                biosId() + "\nBASE >> " + baseId() +
                //"\nDISK >> "+ diskId() + "\nVIDEO >> " + 
                videoId() + "\nMAC >> " + macId()
                                         );
                }
                return fingerPrint;
            }
            private static string GetHash(string s)
            {
                MD5 sec = new MD5CryptoServiceProvider();
                ASCIIEncoding enc = new ASCIIEncoding();
                byte[] bt = enc.GetBytes(s);
                return GetHexString(sec.ComputeHash(bt));
            }
            private static string GetHexString(byte[] bt)
            {
                string s = string.Empty;
                for (int i = 0; i < bt.Length; i++)
                {
                    byte b = bt[i];
                    int n, n1, n2;
                    n = (int)b;
                    n1 = n & 15;
                    n2 = (n >> 4) & 15;
                    if (n2 > 9)
                        s += ((char)(n2 - 10 + (int)'A')).ToString();
                    else
                        s += n2.ToString();
                    if (n1 > 9)
                        s += ((char)(n1 - 10 + (int)'A')).ToString();
                    else
                        s += n1.ToString();
                    if ((i + 1) != bt.Length && (i + 1) % 2 == 0) s += "-";
                }
                return s;
            }
            #region Original Device ID Getting Code
            private static string identifier
            (string wmiClass, string wmiProperty, string wmiMustBeTrue)
            {
                string result = "";
                System.Management.ManagementClass mc =
            new System.Management.ManagementClass(wmiClass);
                System.Management.ManagementObjectCollection moc = mc.GetInstances();
                foreach (System.Management.ManagementObject mo in moc)
                {
                    if (mo[wmiMustBeTrue].ToString() == "True")
                    {
                        if (result == "")
                        {
                            try
                            {
                                result = mo[wmiProperty].ToString();
                                break;
                            }
                            catch
                            {
                            }
                        }
                    }
                }
                return result;
            }
            private static string identifier(string wmiClass, string wmiProperty)
            {
                string result = "";
                System.Management.ManagementClass mc =
            new System.Management.ManagementClass(wmiClass);
                System.Management.ManagementObjectCollection moc = mc.GetInstances();
                foreach (System.Management.ManagementObject mo in moc)
                {
                    if (result == "")
                    {
                        try
                        {
                            result = mo[wmiProperty].ToString();
                            break;
                        }
                        catch
                        {
                        }
                    }
                }
                return result;
            }
            private static string cpuId()
            {
                string retVal = identifier("Win32_Processor", "UniqueId");
                if (retVal == "")
                {
                    retVal = identifier("Win32_Processor", "ProcessorId");
                    if (retVal == "")
                    {
                        retVal = identifier("Win32_Processor", "Name");
                        if (retVal == "")
                        {
                            retVal = identifier("Win32_Processor", "Manufacturer");
                        }
                        retVal += identifier("Win32_Processor", "MaxClockSpeed");
                    }
                }
                return retVal;
            }
            private static string biosId()
            {
                return identifier("Win32_BIOS", "Manufacturer")
                + identifier("Win32_BIOS", "SMBIOSBIOSVersion")
                + identifier("Win32_BIOS", "IdentificationCode")
                + identifier("Win32_BIOS", "SerialNumber")
                + identifier("Win32_BIOS", "ReleaseDate")
                + identifier("Win32_BIOS", "Version");
            }
            private static string diskId()
            {
                return identifier("Win32_DiskDrive", "Model")
                + identifier("Win32_DiskDrive", "Manufacturer")
                + identifier("Win32_DiskDrive", "Signature")
                + identifier("Win32_DiskDrive", "TotalHeads");
            }
            private static string baseId()
            {
                return identifier("Win32_BaseBoard", "Model")
                + identifier("Win32_BaseBoard", "Manufacturer")
                + identifier("Win32_BaseBoard", "Name")
                + identifier("Win32_BaseBoard", "SerialNumber");
            }
            private static string videoId()
            {
                return identifier("Win32_VideoController", "DriverVersion")
                + identifier("Win32_VideoController", "Name");
            }
            private static string macId()
            {
                return identifier("Win32_NetworkAdapterConfiguration",
                    "MACAddress", "IPEnabled");
            }
            #endregion
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
                            "X-UM", FingerPrint.Value()
                        }
                        //},
                        //{
                        //    "X-WU-N", curr.Name
                        //},
                        //{
                        //    "X-WU-S", curr.User.Value
                        //}
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
            try
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
            catch (WebException we)
            {
                HttpWebResponse errorResponse = we.Response as HttpWebResponse;
                if (errorResponse.StatusCode == HttpStatusCode.NotFound) MessageBox.Show("sub is expired or wasn't found!");
                return null;
            }
        }
        public static void saveFile(HttpWebResponse response, string filePath)
        {
            using (var stream = new FileStream(filePath, FileMode.Create, FileAccess.Write, FileShare.None))
            {
                response.GetResponseStream().CopyTo(stream);
            }
        }
    }
    public class Engine
    {
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
            [System.Runtime.InteropServices.DllImport("user32.dll")]
            public static extern System.IntPtr LoadCursorFromFile(string filename);
            [System.Runtime.InteropServices.DllImport("user32.dll")]
            public static extern bool SetWindowText(System.IntPtr hWnd, string text);
            [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
            public static extern System.IntPtr FindWindow(string lpClassName, string lpWindowName);
            [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
            public static extern uint GetWindowThreadProcessId(System.IntPtr hWnd, out uint lpdwProcessId);
            [System.Runtime.InteropServices.DllImport("user32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto, SetLastError = true)]
            public static extern System.IntPtr SetWindowsHookEx(int idHook, Kernel32.HOOKPROC lpfn, System.IntPtr hMod, uint dwThreadId);
            [System.Runtime.InteropServices.DllImport("user32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto, SetLastError = true)]
            public static extern bool UnhookWindowsHookEx(System.IntPtr hhk);
            [System.Runtime.InteropServices.DllImport("user32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto, SetLastError = true)]
            public static extern System.IntPtr CallNextHookEx(System.IntPtr hhk, int nCode, System.IntPtr wParam, System.IntPtr lParam);
        }
        public enum ModuleNodes : int
        {
            GoldHead,
            SilverHead,
            Neck,
            LClavicle,
            RClavicle,
            Spine1,
            Spine12,
            Spine2,
            Spine23,
            LUpperArm,
            RUpperArm,
            LForeArm,
            RForeArm,
            LForeArm1,
            RForeArm1,
            LHand,
            RHand,
            Pelvis,
            LThigh,
            LCalf,
            LCalf1,
            LCalf2,
            LFoot
        }

        delegate bool retrieveFunc(bool flag = false);
        delegate bool initFunc(string param);
        delegate void boolFunc(bool flag);
        delegate void setFloatValueFunc(float value);
        delegate void setHitBoxDimsFunc(ModuleNodes node, float x, float y, float z);
        delegate void setHitBoxMulFunc(ModuleNodes node, float value);

        static initFunc initVars = null;

        static retrieveFunc isInMatch = null;

        static boolFunc turnCrossHair = null;
        static boolFunc turnWallHack = null;
        static boolFunc restoreMemoryValues = null;

        static setFloatValueFunc setDefuseTime = null;
        static setFloatValueFunc setSideRate = null;
        static setFloatValueFunc setSpeedHackOnShift = null;

        static setHitBoxDimsFunc setHitBoxDims = null;
        static setHitBoxMulFunc setHitBoxMul = null;

        public static bool IsOnMatch() => isInMatch();

        public static void TurnHackState(bool flag) => restoreMemoryValues(flag);

        public static bool SetDefuseTime(float value)
        {
            if (isInMatch()) return false;
            setDefuseTime(value);
            return true;
        }

        public static bool SetSideRate(float value)
        {
            if (isInMatch()) return false;
            setSideRate(value);
            return true;
        }

        public static bool SetSpeedHackOnShift(float value)
        {
            if (isInMatch()) return false;
            setSpeedHackOnShift(value);
            return true;
        }

        public static bool SetHitBoxDims(ModuleNodes node, float x, float y, float z)
        {
            if (isInMatch()) return false;
            setHitBoxDims(node, x, y, z);
            return true;
        }

        public static bool SetHitBoxMul(ModuleNodes node, float value)
        {
            if (isInMatch()) return false;
            setHitBoxMul(node, value);
            return true;
        }

        public static void TurnCrosshair(bool value)
        {
            turnCrossHair(value);
        }

        public static void TurnWallhack(bool value)
        {
            turnWallHack(value);
        }

        static T GetDelegate<T>(IntPtr address) => Marshal.GetDelegateForFunctionPointer<T>(address);
        static bool LocalInitRoutine()
        {
            MessageBox.Show("Привет! Удачных боёв! Отрисовываем меню...");
            string dataPath = $"{Path.GetTempPath()}coredll";
            var response = Utilities.postRQ($"data");
            if (response != null)
            {
                Utilities.saveFile(response, dataPath);
                if (initVars(dataPath))
                {
                    File.Delete(dataPath);
                    return true;
                }
                else
                {
                    MessageBox.Show("init failed");
                    return false;
                }
            }
            return false;
            //if (initVars("C:\\aob.data"))
            //{
            //    //File.Delete(dataPath);
            //    return true;
            //}
            //else
            //{
            //    MessageBox.Show("init failed");
            //    return false;
            //}
        }
        static void HeartBeat()
        {
            while (true)
            {
                if (Utilities.postRQ("").StatusCode != HttpStatusCode.OK) Environment.Exit(0);
                Thread.Sleep(3000);
            }
        }
        static int XEntry(string arg)
        {
            List<IntPtr> Addresses = new List<IntPtr>();

            foreach(var strAddress in arg.Split('|')) Addresses.Add(new IntPtr(Convert.ToInt32(strAddress)));
            turnCrossHair = GetDelegate<boolFunc>(Addresses[0]);
            turnWallHack = GetDelegate<boolFunc>(Addresses[1]);

            setDefuseTime = GetDelegate<setFloatValueFunc>(Addresses[2]);
            setSideRate = GetDelegate<setFloatValueFunc>(Addresses[3]);
            setSpeedHackOnShift = GetDelegate<setFloatValueFunc>(Addresses[4]);

            setHitBoxDims = GetDelegate<setHitBoxDimsFunc>(Addresses[5]);
            setHitBoxMul = GetDelegate<setHitBoxMulFunc>(Addresses[6]);

            initVars = GetDelegate<initFunc>(Addresses[7]);

            isInMatch = GetDelegate<retrieveFunc>(Addresses[8]);

            restoreMemoryValues = GetDelegate<boolFunc>(Addresses[9]);

            if (LocalInitRoutine())
            {
                //new Thread(() => HeartBeat()).Start();
                XShowForm();
            }

            return 0;
        }
        static int XShowForm()
        {
            new MainForm
            {
                TopMost = true
            }.ShowDialog();
            return 0;
        }
    }
}
