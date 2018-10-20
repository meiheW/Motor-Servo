namespace WinApplication
{
    partial class APPMain
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.Cancel = new System.Windows.Forms.Button();
            this.Right = new System.Windows.Forms.Button();
            this.Left = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label6 = new System.Windows.Forms.Label();
            this.ZeroDegreeText = new System.Windows.Forms.TextBox();
            this.LeftDegreeText = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.Scan = new System.Windows.Forms.Button();
            this.RightDegreeText = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.MotionCheck = new System.Windows.Forms.Button();
            this.ErrorCheck = new System.Windows.Forms.Button();
            this.IPconnect = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.IPBox = new System.Windows.Forms.ComboBox();
            this.PortBox = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.IPClose = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label4 = new System.Windows.Forms.Label();
            this.GotoDestinaton = new System.Windows.Forms.Button();
            this.DestinationText = new System.Windows.Forms.TextBox();
            this.MessageText = new System.Windows.Forms.TextBox();
            this.ClearText = new System.Windows.Forms.Button();
            this.MotorStop = new System.Windows.Forms.Button();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.button1 = new System.Windows.Forms.Button();
            this.GetSpeed = new System.Windows.Forms.Button();
            this.SpeedSetButton = new System.Windows.Forms.Button();
            this.DeceLabel = new System.Windows.Forms.Label();
            this.AcceLabel = new System.Windows.Forms.Label();
            this.SpeedLabel = new System.Windows.Forms.Label();
            this.DeceText = new System.Windows.Forms.TextBox();
            this.AcceText = new System.Windows.Forms.TextBox();
            this.SpeedText = new System.Windows.Forms.TextBox();
            this.CurrentStatus = new System.Windows.Forms.Label();
            this.RST = new System.Windows.Forms.Button();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.Cancel);
            this.groupBox1.Controls.Add(this.Right);
            this.groupBox1.Controls.Add(this.Left);
            this.groupBox1.Location = new System.Drawing.Point(462, 349);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox1.Size = new System.Drawing.Size(381, 75);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "手动模式";
            // 
            // Cancel
            // 
            this.Cancel.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.Cancel.Location = new System.Drawing.Point(269, 28);
            this.Cancel.Name = "Cancel";
            this.Cancel.Size = new System.Drawing.Size(60, 28);
            this.Cancel.TabIndex = 2;
            this.Cancel.Text = "停";
            this.Cancel.UseVisualStyleBackColor = false;
            this.Cancel.Click += new System.EventHandler(this.Stop_Click);
            // 
            // Right
            // 
            this.Right.Location = new System.Drawing.Point(159, 28);
            this.Right.Name = "Right";
            this.Right.Size = new System.Drawing.Size(60, 28);
            this.Right.TabIndex = 1;
            this.Right.Text = "右";
            this.Right.UseVisualStyleBackColor = true;
            this.Right.Click += new System.EventHandler(this.Right_Click);
            // 
            // Left
            // 
            this.Left.Location = new System.Drawing.Point(52, 28);
            this.Left.Name = "Left";
            this.Left.Size = new System.Drawing.Size(60, 28);
            this.Left.TabIndex = 0;
            this.Left.Text = "左";
            this.Left.UseVisualStyleBackColor = true;
            this.Left.Click += new System.EventHandler(this.Left_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.ZeroDegreeText);
            this.groupBox2.Controls.Add(this.LeftDegreeText);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.Scan);
            this.groupBox2.Controls.Add(this.RightDegreeText);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Location = new System.Drawing.Point(462, 132);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(381, 105);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "扫描模式";
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(37, 78);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(30, 20);
            this.label6.TabIndex = 5;
            this.label6.Text = "零点";
            // 
            // ZeroDegreeText
            // 
            this.ZeroDegreeText.Location = new System.Drawing.Point(74, 74);
            this.ZeroDegreeText.Name = "ZeroDegreeText";
            this.ZeroDegreeText.Size = new System.Drawing.Size(60, 24);
            this.ZeroDegreeText.TabIndex = 1;
            this.ZeroDegreeText.Text = "0";
            // 
            // LeftDegreeText
            // 
            this.LeftDegreeText.Location = new System.Drawing.Point(73, 44);
            this.LeftDegreeText.Name = "LeftDegreeText";
            this.LeftDegreeText.Size = new System.Drawing.Size(60, 24);
            this.LeftDegreeText.TabIndex = 1;
            this.LeftDegreeText.Text = "-70.0";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(37, 48);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(30, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "起：";
            // 
            // Scan
            // 
            this.Scan.Location = new System.Drawing.Point(285, 45);
            this.Scan.Name = "Scan";
            this.Scan.Size = new System.Drawing.Size(60, 28);
            this.Scan.TabIndex = 4;
            this.Scan.Text = "开始";
            this.Scan.UseVisualStyleBackColor = true;
            this.Scan.Click += new System.EventHandler(this.Scan_Click);
            // 
            // RightDegreeText
            // 
            this.RightDegreeText.Location = new System.Drawing.Point(191, 45);
            this.RightDegreeText.Name = "RightDegreeText";
            this.RightDegreeText.Size = new System.Drawing.Size(60, 24);
            this.RightDegreeText.TabIndex = 3;
            this.RightDegreeText.Text = "70.0";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(155, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(30, 20);
            this.label2.TabIndex = 2;
            this.label2.Text = "终：";
            // 
            // MotionCheck
            // 
            this.MotionCheck.Location = new System.Drawing.Point(462, 452);
            this.MotionCheck.Name = "MotionCheck";
            this.MotionCheck.Size = new System.Drawing.Size(100, 50);
            this.MotionCheck.TabIndex = 3;
            this.MotionCheck.Text = "运动自检";
            this.MotionCheck.UseVisualStyleBackColor = true;
            this.MotionCheck.Click += new System.EventHandler(this.Check_Click);
            // 
            // ErrorCheck
            // 
            this.ErrorCheck.Location = new System.Drawing.Point(568, 451);
            this.ErrorCheck.Name = "ErrorCheck";
            this.ErrorCheck.Size = new System.Drawing.Size(100, 50);
            this.ErrorCheck.TabIndex = 4;
            this.ErrorCheck.Text = "故障查询";
            this.ErrorCheck.UseVisualStyleBackColor = true;
            this.ErrorCheck.Click += new System.EventHandler(this.InquireStatus_Click);
            // 
            // IPconnect
            // 
            this.IPconnect.Location = new System.Drawing.Point(274, 36);
            this.IPconnect.Name = "IPconnect";
            this.IPconnect.Size = new System.Drawing.Size(80, 28);
            this.IPconnect.TabIndex = 5;
            this.IPconnect.Text = "连接";
            this.IPconnect.UseVisualStyleBackColor = true;
            this.IPconnect.Click += new System.EventHandler(this.IPconnect_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.IPBox);
            this.groupBox3.Controls.Add(this.PortBox);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Controls.Add(this.IPClose);
            this.groupBox3.Controls.Add(this.IPconnect);
            this.groupBox3.Location = new System.Drawing.Point(462, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(373, 108);
            this.groupBox3.TabIndex = 7;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "IP连接";
            // 
            // IPBox
            // 
            this.IPBox.FormattingEnabled = true;
            this.IPBox.Items.AddRange(new object[] {
            "192.168.0.15",
            "192.168.1.15"});
            this.IPBox.Location = new System.Drawing.Point(80, 30);
            this.IPBox.Name = "IPBox";
            this.IPBox.Size = new System.Drawing.Size(175, 22);
            this.IPBox.TabIndex = 10;
            this.IPBox.Text = "192.168.0.15";
            // 
            // PortBox
            // 
            this.PortBox.FormattingEnabled = true;
            this.PortBox.Items.AddRange(new object[] {
            "12345"});
            this.PortBox.Location = new System.Drawing.Point(80, 70);
            this.PortBox.Name = "PortBox";
            this.PortBox.Size = new System.Drawing.Size(175, 22);
            this.PortBox.TabIndex = 9;
            this.PortBox.Text = "12345";
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(6, 73);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(68, 19);
            this.label5.TabIndex = 8;
            this.label5.Text = "Port:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(18, 33);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(31, 15);
            this.label3.TabIndex = 7;
            this.label3.Text = "IP:";
            // 
            // IPClose
            // 
            this.IPClose.Location = new System.Drawing.Point(274, 69);
            this.IPClose.Name = "IPClose";
            this.IPClose.Size = new System.Drawing.Size(80, 28);
            this.IPClose.TabIndex = 5;
            this.IPClose.Text = "关闭";
            this.IPClose.UseVisualStyleBackColor = true;
            this.IPClose.Click += new System.EventHandler(this.IPClose_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label4);
            this.groupBox4.Controls.Add(this.GotoDestinaton);
            this.groupBox4.Controls.Add(this.DestinationText);
            this.groupBox4.Location = new System.Drawing.Point(462, 253);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(381, 80);
            this.groupBox4.TabIndex = 8;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "指定目标点";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(81, 34);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(82, 15);
            this.label4.TabIndex = 2;
            this.label4.Text = "目标点位置";
            // 
            // GotoDestinaton
            // 
            this.GotoDestinaton.Location = new System.Drawing.Point(283, 28);
            this.GotoDestinaton.Name = "GotoDestinaton";
            this.GotoDestinaton.Size = new System.Drawing.Size(60, 28);
            this.GotoDestinaton.TabIndex = 1;
            this.GotoDestinaton.Text = "GO";
            this.GotoDestinaton.UseVisualStyleBackColor = true;
            this.GotoDestinaton.Click += new System.EventHandler(this.GotoDestinaton_Click);
            // 
            // DestinationText
            // 
            this.DestinationText.Location = new System.Drawing.Point(191, 28);
            this.DestinationText.Name = "DestinationText";
            this.DestinationText.Size = new System.Drawing.Size(60, 24);
            this.DestinationText.TabIndex = 0;
            this.DestinationText.Text = "0.0";
            // 
            // MessageText
            // 
            this.MessageText.Location = new System.Drawing.Point(39, 24);
            this.MessageText.Multiline = true;
            this.MessageText.Name = "MessageText";
            this.MessageText.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.MessageText.Size = new System.Drawing.Size(383, 309);
            this.MessageText.TabIndex = 9;
            // 
            // ClearText
            // 
            this.ClearText.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.ClearText.Location = new System.Drawing.Point(347, 336);
            this.ClearText.Name = "ClearText";
            this.ClearText.Size = new System.Drawing.Size(75, 30);
            this.ClearText.TabIndex = 10;
            this.ClearText.Text = "清空对话";
            this.ClearText.UseVisualStyleBackColor = false;
            this.ClearText.Click += new System.EventHandler(this.ClearText_Click);
            // 
            // MotorStop
            // 
            this.MotorStop.BackColor = System.Drawing.Color.Red;
            this.MotorStop.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.MotorStop.Location = new System.Drawing.Point(780, 454);
            this.MotorStop.Name = "MotorStop";
            this.MotorStop.Size = new System.Drawing.Size(70, 47);
            this.MotorStop.TabIndex = 11;
            this.MotorStop.Text = "释放伺服";
            this.MotorStop.UseVisualStyleBackColor = false;
            this.MotorStop.Click += new System.EventHandler(this.MotorStop_Click);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.button1);
            this.groupBox5.Controls.Add(this.GetSpeed);
            this.groupBox5.Controls.Add(this.SpeedSetButton);
            this.groupBox5.Controls.Add(this.DeceLabel);
            this.groupBox5.Controls.Add(this.AcceLabel);
            this.groupBox5.Controls.Add(this.SpeedLabel);
            this.groupBox5.Controls.Add(this.DeceText);
            this.groupBox5.Controls.Add(this.AcceText);
            this.groupBox5.Controls.Add(this.SpeedText);
            this.groupBox5.Location = new System.Drawing.Point(39, 377);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(385, 135);
            this.groupBox5.TabIndex = 12;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "运行速度设置";
            // 
            // button1
            // 
            this.button1.Font = new System.Drawing.Font("宋体", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.button1.Location = new System.Drawing.Point(308, 86);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(71, 43);
            this.button1.TabIndex = 8;
            this.button1.Text = "累计角度";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.sum_click);
            // 
            // GetSpeed
            // 
            this.GetSpeed.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.GetSpeed.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.GetSpeed.Location = new System.Drawing.Point(229, 87);
            this.GetSpeed.Name = "GetSpeed";
            this.GetSpeed.Size = new System.Drawing.Size(76, 42);
            this.GetSpeed.TabIndex = 7;
            this.GetSpeed.Text = "获取速度";
            this.GetSpeed.UseVisualStyleBackColor = false;
            this.GetSpeed.Click += new System.EventHandler(this.GetSpeed_Click);
            // 
            // SpeedSetButton
            // 
            this.SpeedSetButton.BackColor = System.Drawing.SystemColors.MenuHighlight;
            this.SpeedSetButton.Font = new System.Drawing.Font("宋体", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.SpeedSetButton.Location = new System.Drawing.Point(264, 16);
            this.SpeedSetButton.Name = "SpeedSetButton";
            this.SpeedSetButton.Size = new System.Drawing.Size(73, 66);
            this.SpeedSetButton.TabIndex = 6;
            this.SpeedSetButton.Text = "速度设定";
            this.SpeedSetButton.UseVisualStyleBackColor = false;
            this.SpeedSetButton.Click += new System.EventHandler(this.SpeedSetButton_Click);
            // 
            // DeceLabel
            // 
            this.DeceLabel.Location = new System.Drawing.Point(31, 83);
            this.DeceLabel.Name = "DeceLabel";
            this.DeceLabel.Size = new System.Drawing.Size(130, 24);
            this.DeceLabel.TabIndex = 5;
            this.DeceLabel.Text = "减速时间(0.1ms)";
            // 
            // AcceLabel
            // 
            this.AcceLabel.Location = new System.Drawing.Point(31, 58);
            this.AcceLabel.Name = "AcceLabel";
            this.AcceLabel.Size = new System.Drawing.Size(130, 24);
            this.AcceLabel.TabIndex = 4;
            this.AcceLabel.Text = "加速时间(0.1ms)";
            // 
            // SpeedLabel
            // 
            this.SpeedLabel.Location = new System.Drawing.Point(31, 28);
            this.SpeedLabel.Name = "SpeedLabel";
            this.SpeedLabel.Size = new System.Drawing.Size(130, 24);
            this.SpeedLabel.TabIndex = 3;
            this.SpeedLabel.Text = "速度(度/s）";
            // 
            // DeceText
            // 
            this.DeceText.Location = new System.Drawing.Point(164, 87);
            this.DeceText.Name = "DeceText";
            this.DeceText.Size = new System.Drawing.Size(60, 24);
            this.DeceText.TabIndex = 2;
            this.DeceText.Text = "100";
            // 
            // AcceText
            // 
            this.AcceText.Location = new System.Drawing.Point(163, 58);
            this.AcceText.Name = "AcceText";
            this.AcceText.Size = new System.Drawing.Size(60, 24);
            this.AcceText.TabIndex = 1;
            this.AcceText.Text = "100";
            // 
            // SpeedText
            // 
            this.SpeedText.Location = new System.Drawing.Point(163, 28);
            this.SpeedText.Name = "SpeedText";
            this.SpeedText.Size = new System.Drawing.Size(60, 24);
            this.SpeedText.TabIndex = 0;
            // 
            // CurrentStatus
            // 
            this.CurrentStatus.BackColor = System.Drawing.Color.White;
            this.CurrentStatus.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.CurrentStatus.Location = new System.Drawing.Point(39, 336);
            this.CurrentStatus.Name = "CurrentStatus";
            this.CurrentStatus.Size = new System.Drawing.Size(305, 30);
            this.CurrentStatus.TabIndex = 13;
            this.CurrentStatus.Text = "当前位置：          当前状态：";
            this.CurrentStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // RST
            // 
            this.RST.Location = new System.Drawing.Point(674, 452);
            this.RST.Name = "RST";
            this.RST.Size = new System.Drawing.Size(100, 50);
            this.RST.TabIndex = 14;
            this.RST.Text = "报警复位";
            this.RST.UseVisualStyleBackColor = true;
            this.RST.Click += new System.EventHandler(this.RST_Click);
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(471, 519);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(56, 19);
            this.checkBox1.TabIndex = 15;
            this.checkBox1.Text = "连续";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // APPMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 14F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(868, 590);
            this.Controls.Add(this.checkBox1);
            this.Controls.Add(this.RST);
            this.Controls.Add(this.CurrentStatus);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.MotorStop);
            this.Controls.Add(this.ClearText);
            this.Controls.Add(this.MessageText);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.ErrorCheck);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.MotionCheck);
            this.Font = new System.Drawing.Font("宋体", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "APPMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "电机控制";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.APPMain_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button Cancel;
        private System.Windows.Forms.Button Right;
        private System.Windows.Forms.Button Left;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button Scan;
        private System.Windows.Forms.TextBox RightDegreeText;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button MotionCheck;
        private System.Windows.Forms.Button ErrorCheck;
        private System.Windows.Forms.Button IPconnect;
        private System.Windows.Forms.TextBox LeftDegreeText;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button GotoDestinaton;
        private System.Windows.Forms.TextBox DestinationText;
        private System.Windows.Forms.TextBox MessageText;
        private System.Windows.Forms.Button ClearText;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox PortBox;
        private System.Windows.Forms.ComboBox IPBox;
        private System.Windows.Forms.Button MotorStop;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Label SpeedLabel;
        private System.Windows.Forms.TextBox DeceText;
        private System.Windows.Forms.TextBox AcceText;
        private System.Windows.Forms.TextBox SpeedText;
        private System.Windows.Forms.Button SpeedSetButton;
        private System.Windows.Forms.Label DeceLabel;
        private System.Windows.Forms.Label AcceLabel;
        private System.Windows.Forms.Label CurrentStatus;
        private System.Windows.Forms.Button RST;
        private System.Windows.Forms.Button GetSpeed;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button IPClose;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox ZeroDegreeText;

    }
}

