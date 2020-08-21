using MetroFramework.Controls;
using pj_ld13;
using System;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace dummy
{
    public partial class MainForm : MetroFramework.Forms.MetroForm
    {
        private Point initialCoord = new Point(0, 0);
        static readonly string cfgFolder = "d0cfgs";
        static readonly DirectoryInfo di = new DirectoryInfo(cfgFolder);
        public MainForm()
        {
            InitializeComponent();
            StartPosition = FormStartPosition.Manual;
            Location = initialCoord;

            metroTabControl1.SelectedIndex = 0;

            if (!Directory.Exists(cfgFolder)) Directory.CreateDirectory(cfgFolder);

            initEventHandlers();
            initByDefaultValues();

            updateConfigList();
        }

        private void updateConfigList()
        {
            metroListView1.Items.Clear();
            var cfgList = di.EnumerateFiles().Select(x => x.Name.Split('.')[0]).ToArray();
            foreach (var c in cfgList) metroListView1.Items.Add(c);
            if (cfgList.Length < 1)
            {
                initByDefaultValues();
                updateConfigList();
            }
            var def = metroListView1.FindItemWithText("default");
            if (def == null) initByDefaultValues();
            else def.EnsureVisible();
        }

        private void initEventHandlers()
        {
            metroTrackBar4.ValueChanged += valueChangedGoldX;
            metroTrackBar5.ValueChanged += valueChangedGoldY;
            metroTrackBar6.ValueChanged += valueChangedGoldZ;
            metroTrackBar7.ValueChanged += valueChangedSilverZ;
            metroTrackBar8.ValueChanged += valueChangedSilverY;
            metroTrackBar9.ValueChanged += valueChangedSilverX;
            metroTrackBar10.ValueChanged += valueChangedBodyZ;
            metroTrackBar11.ValueChanged += valueChangedBodyY;
            metroTrackBar12.ValueChanged += valueChangedBodyX;
            metroTrackBar13.ValueChanged += valueChangedPelvisZ;
            metroTrackBar14.ValueChanged += valueChangedPelvisY;
            metroTrackBar15.ValueChanged += valueChangedPelvisX;
        }

        private void createDefaultConfig()
        {
            metroTrackBar4.Value = 8;
            metroTrackBar5.Value = 8;
            metroTrackBar6.Value = 8;
            metroTrackBar7.Value = 17;
            metroTrackBar8.Value = 17;
            metroTrackBar9.Value = 17;
            metroTrackBar10.Value = 20;
            metroTrackBar11.Value = 20;
            metroTrackBar12.Value = 20;
            metroTrackBar13.Value = 23;
            metroTrackBar14.Value = 23;
            metroTrackBar15.Value = 23;
            saveConfig("default");
        }

        private void initByDefaultValues()
        {
            if (metroListView1.FindItemWithText("default") == null) createDefaultConfig();
            loadConfig("default");
        }

        private void valueChangedGoldX(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel14.Text = $"x: {mtb.Value}";
        }
        private void valueChangedGoldY(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel15.Text = $"y: {mtb.Value}";
        }
        private void valueChangedGoldZ(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel16.Text = $"z: {mtb.Value}";
        }

        private void valueChangedSilverX(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel11.Text = $"x: {mtb.Value}";
        }
        private void valueChangedSilverY(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel17.Text = $"y: {mtb.Value}";
        }
        private void valueChangedSilverZ(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel21.Text = $"z: {mtb.Value}";
        }

        private void valueChangedBodyX(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel12.Text = $"x: {mtb.Value}";
        }
        private void valueChangedBodyY(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel18.Text = $"y: {mtb.Value}";
        }
        private void valueChangedBodyZ(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel22.Text = $"z: {mtb.Value}";
        }

        private void valueChangedPelvisX(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel13.Text = $"x: {mtb.Value}";
        }
        private void valueChangedPelvisY(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel19.Text = $"y: {mtb.Value}";
        }
        private void valueChangedPelvisZ(object sender, EventArgs e)
        {
            var mtb = (MetroTrackBar)sender;
            metroLabel23.Text = $"z: {mtb.Value}";
        }

        private void improveParams(bool showWarn)
        {
            Engine.TurnWallhack(metroToggle1.Checked);
            Engine.TurnCrosshair(metroToggle3.Checked);

            bool isInMatch = Engine.IsOnMatch();

            if (!isInMatch)
            {
                Engine.SetDefuseTime(metroToggle4.Checked ? 3 : 6);

                Engine.SetHitBoxDims(Engine.ModuleNodes.GoldHead, (float)metroTrackBar4.Value, (float)metroTrackBar5.Value, (float)metroTrackBar6.Value);

                Engine.SetHitBoxDims(Engine.ModuleNodes.SilverHead, (float)metroTrackBar9.Value, (float)metroTrackBar8.Value, (float)metroTrackBar7.Value);

                Engine.SetHitBoxDims(Engine.ModuleNodes.Spine1, (float)metroTrackBar12.Value, (float)metroTrackBar11.Value, (float)metroTrackBar10.Value);
                Engine.SetHitBoxDims(Engine.ModuleNodes.Spine12, (float)metroTrackBar12.Value, (float)metroTrackBar11.Value, (float)metroTrackBar10.Value);
                Engine.SetHitBoxDims(Engine.ModuleNodes.Spine2, (float)metroTrackBar12.Value, (float)metroTrackBar11.Value, (float)metroTrackBar10.Value);
                Engine.SetHitBoxDims(Engine.ModuleNodes.Spine23, (float)metroTrackBar12.Value, (float)metroTrackBar11.Value, (float)metroTrackBar10.Value);

                Engine.SetHitBoxDims(Engine.ModuleNodes.Pelvis, (float)metroTrackBar15.Value, (float)metroTrackBar14.Value, (float)metroTrackBar13.Value);
            }
            else if (showWarn)
            {
                MetroFramework.MetroMessageBox.Show(
                    this,
                    $"Изменения применены исключительно к визуалам, поскольку вы находитесь в бою!\n" +
                    $"Для изменения остальных значений необходимо выйти из боя и применить заново!",
                    "IMPROVE",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Information
                );
            }
        }

        private void metroButton1_Click(object sender, EventArgs e)
        {
            improveParams(true);
        }

        private void saveConfig(string cfgName)
        {
            var whState = $"wallhack: {(metroToggle1.Checked ? "on" : "off")}";
            var crossHairState = $"crosshair: {(metroToggle3.Checked ? "on" : "off")}";
            var fastDefuseState = $"fast defuse: {(metroToggle4.Checked ? "on" : "off")}";

            var goldHitBoxValue = $"gold hitbox value: {(float)metroTrackBar4.Value} {(float)metroTrackBar5.Value} {(float)metroTrackBar6.Value}";
            var silverHitBoxValue = $"silver hitbox value: {(float)metroTrackBar9.Value} {(float)metroTrackBar8.Value} {(float)metroTrackBar7.Value}";
            var bodyHitBoxValue = $"body hitbox value: {(float)metroTrackBar12.Value} {(float)metroTrackBar11.Value} {(float)metroTrackBar10.Value}";
            var pelvisHitBoxValue = $"pelvis hitbox value: {(float)metroTrackBar15.Value} {(float)metroTrackBar14.Value} {(float)metroTrackBar13.Value}";

            var cfgContent = $"{whState}\r\n" +
                             $"{crossHairState}\r\n" +
                             $"{fastDefuseState}\r\n" +
                             $"{goldHitBoxValue}\r\n" +
                             $"{silverHitBoxValue}\r\n" +
                             $"{bodyHitBoxValue}\r\n" +
                             $"{pelvisHitBoxValue}";

            File.WriteAllText($"{cfgFolder}/{cfgName}.d0cfg", cfgContent);
        }

        private void loadConfig(string cfgName)
        {
            var filePath = $"{cfgFolder}/{cfgName}.d0cfg";
            using (var fileStream = File.OpenRead(filePath))
            using (StreamReader reader = new StreamReader(fileStream))
            {
                string fileContent = reader.ReadToEnd();
                var split = fileContent.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

                metroToggle1.Checked = split[0].Split(null)[1] == "on";
                metroToggle3.Checked = split[1].Split(null)[1] == "on";
                metroToggle4.Checked = split[2].Split(null)[2] == "on";

                var goldHitBoxValue = split[3].Split(null);
                metroTrackBar4.Value = (int)float.Parse(goldHitBoxValue[3]);
                metroTrackBar5.Value = (int)float.Parse(goldHitBoxValue[4]);
                metroTrackBar6.Value = (int)float.Parse(goldHitBoxValue[5]);

                var silverHitBoxValue = split[4].Split(null);
                metroTrackBar9.Value = (int)float.Parse(silverHitBoxValue[3]);
                metroTrackBar8.Value = (int)float.Parse(silverHitBoxValue[4]);
                metroTrackBar7.Value = (int)float.Parse(silverHitBoxValue[5]);

                var bodyHitBoxValue = split[5].Split(null);
                metroTrackBar12.Value = (int)float.Parse(bodyHitBoxValue[3]);
                metroTrackBar11.Value = (int)float.Parse(bodyHitBoxValue[4]);
                metroTrackBar10.Value = (int)float.Parse(bodyHitBoxValue[5]);

                var pelvisHitBoxValue = split[6].Split(null);
                metroTrackBar15.Value = (int)float.Parse(pelvisHitBoxValue[3]);
                metroTrackBar14.Value = (int)float.Parse(pelvisHitBoxValue[4]);
                metroTrackBar13.Value = (int)float.Parse(pelvisHitBoxValue[5]);
            }
            improveParams(false);
        }

        private void metroButton2_Click(object sender, EventArgs e)
        {
            if (metroTextBox1.Text == "")
            {
                MetroFramework.MetroMessageBox.Show(
                    this,
                    "Название конфига не указано!",
                    "CFG SAVE",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error
                );
                return;
            }
            saveConfig(metroTextBox1.Text);
            updateConfigList();
            MetroFramework.MetroMessageBox.Show(
                this,
                "Конфиг успешно сохранён!",
                "CFG SAVE",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information
            );
        }

        private void metroButton3_Click(object sender, EventArgs e)
        {
            var sItems = metroListView1.SelectedItems;
            if (sItems.Count <= 0)
            {
                MetroFramework.MetroMessageBox.Show(
                    this,
                    "Конфиг не выбран!",
                    "CFG SAVE",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error
                );
                return;
            }
            loadConfig(sItems[0].Text);
            MetroFramework.MetroMessageBox.Show(
                this,
                "Конфиг успешно загружен!",
                "CFG LOAD",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information
            );
        }

        private void metroButton4_Click(object sender, EventArgs e)
        {
            updateConfigList();
        }
    }
}
