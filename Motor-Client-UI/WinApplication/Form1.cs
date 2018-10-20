using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Net;
using System.Net.Sockets;
using System.Text.RegularExpressions;
using System.Threading;

namespace WinApplication
{
    public partial class APPMain : Form
    {
        // 网口监听线程
        bool connected = false;
        private BackgroundWorker mstatus_thread = null;
        private UdpClient mclient = null;
        private IPEndPoint mremote_ip_end_point = null;
        private IPEndPoint mlacol_ip_end_point = null;
        // UI信息
        private double msumangle = 0; //运动累计角度
        private string minfo = string.Empty;//硬件信息
        private double mangle = 360; // 当前角度
        private int mstatus = 1; // 运行状态
        private int mcmdstatus = -1; // 指令执行情况
        private int mfinish = -1; // 位置完成，finish信号
        private string merror_msg = string.Empty; //错误代码
        private string merror_backup = string.Empty;
        private string mspeed_msg = string.Empty; //速度信息
        private string mspeed_backup = string.Empty;
        // 端口数据
        private string mrecv_msg = string.Empty;

        private int cmdstatus = 0;

        private float left_degree;
        private float right_degree;
        private float zero_degree = 0;

        private int         have_ERRCODE = 0;
        private string str_ErrCode;

        public APPMain()
        {
            InitializeComponent();
        }

        private void APPMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (mstatus_thread != null)
            {
                connected = false;
                Thread.Sleep(1000);
                mstatus_thread.CancelAsync();
                mstatus_thread.Dispose();
                mstatus_thread = null;
                //Thread.Sleep(1000);

            }
            if (mclient != null)
            {
                mclient.Close();
            }

        }

        private void IPClose_Click(object sender, EventArgs e)
        {
            if (mclient != null)
            {
                connected = false;
                Thread.Sleep(1000);
                mstatus_thread.CancelAsync();
                mclient.Close();

                mstatus_thread.Dispose();
                mstatus_thread = null;
                mclient = null;
                IPconnect.Enabled = true;
                IPClose.Enabled = false;

                MessageShow("关闭连接");
            }
        }
        //IP连接
        private void IPconnect_Click(object sender, EventArgs e)
        {
            IPconnect.Enabled = false;

            if (string.IsNullOrEmpty(IPBox.Text) || string.IsNullOrEmpty(PortBox.Text))
            {
                MessageBox.Show("请输入IP地址与端口");
                IPconnect.Enabled = true;
                return;
            }
            int portNum = Convert.ToInt32(PortBox.Text);
            string hostName = IPBox.Text.ToString();
            string pattern = @"^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}$";
            bool isMatch = System.Text.RegularExpressions.Regex.IsMatch(hostName, pattern);
            if (!isMatch)
            {
                MessageBox.Show("请输入正确IP格式");
                IPconnect.Enabled = true;
                return;
            }

            string message = "Connect to " + hostName + " : " + portNum.ToString();
            MessageShow(message);

            // 远端IP设置
            mremote_ip_end_point = new IPEndPoint(IPAddress.Parse(hostName), portNum);
            mlacol_ip_end_point = new IPEndPoint(IPAddress.Any, portNum);
            // 创建终端
            mclient = new UdpClient(mlacol_ip_end_point);

            mclient.Client.SendTimeout = 5000;
            mclient.Client.ReceiveTimeout = 5000;

            // 
            CommandSend("$");
         
                      

            // 创建状态监听与更新线程
            
            mstatus_thread = new BackgroundWorker();
            mstatus_thread.WorkerSupportsCancellation = true;
            mstatus_thread.DoWork += new DoWorkEventHandler(ListenUdp); // 注册线程主体方法
            mstatus_thread.ProgressChanged += new ProgressChangedEventHandler(UpdateUI); // 注册UI更新方法
            mstatus_thread.WorkerReportsProgress = true;
            mstatus_thread.RunWorkerAsync();

           // CommandSend("$");

            IPconnect.Enabled = false;
            IPClose.Enabled = true;

        }

        //扫描模式
        private void Scan_Click(object sender, EventArgs e)
        {
            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }

            // 空闲状态判断
            if (1 != mstatus)
            {
                MessageBox.Show("电机非空闲状态");
                return;
            }


            //  Scan.Enabled = false;

            string left_detree_str = LeftDegreeText.Text.ToString();
            string right_degree_str = RightDegreeText.Text.ToString();
            string zero_degree_str = ZeroDegreeText.Text.ToString();
            string float_pattern = @"^-?([1-9]\d*\.\d*|0\.\d*[1-9]\d*|0?\.0+|0)$";
            string int_pattern = @"^-?[1-9]\d*$";
            bool isMatchL = System.Text.RegularExpressions.Regex.IsMatch(left_detree_str, float_pattern) || System.Text.RegularExpressions.Regex.IsMatch(left_detree_str, int_pattern);
            bool isMatchR = System.Text.RegularExpressions.Regex.IsMatch(right_degree_str, float_pattern) || System.Text.RegularExpressions.Regex.IsMatch(right_degree_str, int_pattern);
            bool isMatchZ = System.Text.RegularExpressions.Regex.IsMatch(zero_degree_str, float_pattern) || System.Text.RegularExpressions.Regex.IsMatch(right_degree_str, int_pattern);
            if (!(isMatchL && isMatchR))
            {
                MessageBox.Show("请输入正确角度格式");
                Scan.Enabled = true;
                return;
            }


            
            zero_degree = (float)Convert.ToDouble(zero_degree_str);

            left_degree = (float)Convert.ToDouble(left_detree_str) + zero_degree;
            right_degree = (float)Convert.ToDouble(right_degree_str) + zero_degree;
            //Goto Point 1
            string cmd_left = "point " + left_degree.ToString();
            MessageShow("LEFT:"+cmd_left);
            CommandSend(cmd_left);

            cmdstatus = 2;

            timer1.Enabled = true;
            timer1.Interval = 1000;
            timer1.Start();

            //Thread.Sleep(200);
            //while (1 != mstatus)
            //{
            //    //等待起点到达
            //    RefreshStatus();
            //    Thread.Sleep(10);
            //}

            ////Goto Point 2
            //string cmd_right = "point " + right_degree.ToString();
            //MessageShow(cmd_right);
            //CommandSend(cmd_right);

            Scan.Enabled = true;
        }


        //指定目标点
        private void GotoDestinaton_Click(object sender, EventArgs e)
        {

            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }

            // 空闲状态判断
            if (1 != mstatus)
            {
                MessageBox.Show("电机非空闲状态");
                return;
            }
            
            cmdstatus = 0;
            GotoDestinaton.Enabled = false;
            string dst_str = DestinationText.Text.ToString();
            string float_pattern = @"^-?([1-9]\d*\.\d*|0\.\d*[1-9]\d*|0?\.0+|0)$";
            string int_pattern = @"^-?[1-9]\d*$";
            bool isMatch = System.Text.RegularExpressions.Regex.IsMatch(dst_str, float_pattern) || System.Text.RegularExpressions.Regex.IsMatch(dst_str, int_pattern);
            
            if (!isMatch)
            {
                MessageBox.Show("请输入正确角度格式");
                GotoDestinaton.Enabled = true;
                return;
            }
            // 空闲状态判断
            if (1 != mstatus)
            {
                MessageBox.Show("电机非空闲状态");
                GotoDestinaton.Enabled = true;
                return;
            }

            double dst_angle = Convert.ToDouble(dst_str);
            //Goto Point
            string cmd = "point " + dst_angle.ToString();
            MessageShow(cmd);
            CommandSend(cmd);

            GotoDestinaton.Enabled = true;
        }


        //左转
        private void Left_Click(object sender, EventArgs e)
        {
            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }

            // 空闲状态判断
            if (1 != mstatus)
            {
                MessageBox.Show("电机非空闲状态");
                return;
            }
            //左转指令
            string cmd = "runleft";
            MessageShow(cmd);
            CommandSend(cmd);
            cmdstatus = 0;
        }


        //右转
        private void Right_Click(object sender, EventArgs e)
        {
            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }

            // 空闲状态判断
            if (1 != mstatus)
            {
                MessageBox.Show("电机非空闲状态");
                return;
            }
            //右转指令
            string cmd = "runright";
            MessageShow(cmd);
            CommandSend(cmd);
            cmdstatus = 0;
        }

        //取消当前运动,进入空闲
        private void Stop_Click(object sender, EventArgs e)
        {
            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }

           

            //当前运动，停止并进入空闲
            string cmd = "cancel";
            MessageShow(cmd);
            CommandSend(cmd);
            cmdstatus = 0;
        }

        //运动自检操作
        private void Check_Click(object sender, EventArgs e)
        {
            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }

            // 空闲状态判断
            if (1 != mstatus)
            {
                MessageBox.Show("电机非空闲状态");
                return;
            }

            // 运动自检
            string cmd = "check";
            MessageShow(cmd);
            CommandSend(cmd);
            Thread.Sleep(2000);
            cmdstatus = 1;
            //如果在check，而且连续，并且状况空闲
            timer1.Enabled = true;
            timer1.Interval = 1000;
            timer1.Start();

        }

        //故障查询
        private void InquireStatus_Click(object sender, EventArgs e)
        {
            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }

          
            // 请求返回故障码
            string cmd = "errormsg";
            MessageShow(cmd);
            CommandSend(cmd);
            cmdstatus = 0;
        }

        //释放伺服,即serve-off,并退出终端控制程序
        private void MotorStop_Click(object sender, EventArgs e)
        {

            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }
            
            string cmd = "stop";
            MessageShow(cmd);
            CommandSend(cmd);

            connected = false;
            if (mclient != null) {
                mclient.Close();
            }
            IPconnect.Enabled = true;
            cmdstatus = 0;
        }

        //清空对话文本框
        private void ClearText_Click(object sender, EventArgs e)
        {
            MessageText.Clear();
        }

        //速度设定
        private void SpeedSetButton_Click(object sender, EventArgs e)
        {


            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }
            SpeedSetButton.Enabled = false;

            string speed = SpeedText.Text.ToString();
            string accetime = AcceText.Text.ToString();
            string decetime = DeceText.Text.ToString();

            string float_pattern = @"^([1-9]\d*\.\d*|0\.\d*[1-9]\d*|0?\.0+|0)$";
            string int_pattern = @"^[1-9]\d*$";
            bool speedIsMatch = System.Text.RegularExpressions.Regex.IsMatch(speed, float_pattern) || System.Text.RegularExpressions.Regex.IsMatch(speed, int_pattern);
            bool accetimeIsMatch = System.Text.RegularExpressions.Regex.IsMatch(accetime, int_pattern);
            bool decetimeIsMatch = System.Text.RegularExpressions.Regex.IsMatch(decetime, int_pattern);
            if (!speedIsMatch || !accetimeIsMatch || !decetimeIsMatch)
            {
                MessageBox.Show("请输入正确参数格式");
                SpeedSetButton.Enabled = true;
                return;
            }
            // 空闲状态判断
            if (1 != mstatus) 
            {
                MessageBox.Show("电机非空闲状态");
                SpeedSetButton.Enabled = true;
                return;
            }

            //速度设定
            string cmd = "speed " + speed;
            MessageShow(cmd);
            CommandSend(cmd);
            // 500ms超时判断
            if (-1 == WaitTimeOut(1 == mstatus))
            {
                MessageBox.Show("响应超时");
                SpeedSetButton.Enabled = true;
                return;
            }

            cmd = "acce " + accetime;
            MessageShow(cmd);
            CommandSend(cmd);
            // 500ms超时判断
            if (-1 == WaitTimeOut(1 == mstatus))
            {
                MessageBox.Show("响应超时");
                SpeedSetButton.Enabled = true;
                return;
            }

            cmd = "dece " + decetime;
            MessageShow(cmd);
            CommandSend(cmd);
            // 500ms超时判断
            if (-1 == WaitTimeOut(1 == mstatus))
            {
                MessageBox.Show("响应超时");
                SpeedSetButton.Enabled = true;
                return;
            }

            SpeedSetButton.Enabled = true;
        }

        //获取电机速度
        private void GetSpeed_Click(object sender, EventArgs e)
        {

            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }
            string cmd = "spdmsg";
            MessageShow(cmd);
            CommandSend(cmd);
        }

        //报警复位
        private void RST_Click(object sender, EventArgs e)
        {

            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }
            // 报警复位命令
            string cmd = "alarmrst";
            MessageShow(cmd);
            CommandSend(cmd);
            cmdstatus = 0;
        }


        //函数说明： 发送指令到服务器
        //输入参数： 字符串指令
        private void CommandSend(string cmd)
        {
            byte[] buffer = Encoding.ASCII.GetBytes(cmd);
            try
            {
                lock (mclient)
                {
                    mclient.Send(buffer, buffer.Length, mremote_ip_end_point);
                }
            }
            catch (SystemException)
            {
                MessageBox.Show("发送出错");
                return;
            }
            Thread.Sleep(1); // ms

            //// 测试数据接收
            //string test_receive = string.Empty;
            //byte[] bytes_rec;
            //try
            //{
            //    lock (mclient)
            //    {
            //        bytes_rec = mclient.Receive(ref mremote_ip_end_point);
            //    }
            //    if (bytes_rec.Length > 0)
            //    {
            //        test_receive = Encoding.Default.GetString(bytes_rec, 0, bytes_rec.Length);
            //        MessageBox.Show("发现接收数据" + "\r\n" + test_receive);
            //    }
            //}
            //catch (SystemException)
            //{
            //    MessageBox.Show("接收出错");
            //    return;
            //}
            //Thread.Sleep(1); // ms

            //// 测试接收数据解析
            //MessageParse(test_receive);
            
        }

        //函数说明： 输出信息到交互文本框
        //输入参数： message - 待输出信息
        private void MessageShow(string message)
        {
            // 清空交互框
            if (MessageText.Text.Count() > 200) MessageText.Text = string.Empty;
            // 信息显示
            MessageText.Text += message + "\r\n";
            //MessageText.Update();
        }

        /// <summary>
        /// TCP监听
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ListenUdp(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker worker = sender as BackgroundWorker;
            connected = true;
            while (connected)
            {
                //MessageBox.Show("ListenTcp");

                //消息读取
                byte[] bytes_rec;
                string recv_message = string.Empty;

                try
                {
                    lock (mclient)
                    {
                        bytes_rec = mclient.Receive(ref mremote_ip_end_point);
                    }
                    if (bytes_rec.Length > 0)
                    {
                        recv_message = Encoding.Default.GetString(bytes_rec, 0, bytes_rec.Length);
                        //MessageBox.Show("发现接收数据");
                        mrecv_msg = recv_message;
                        MessageParse(recv_message);
                    }
                }
                catch (SocketException ex)  // (System.Exception ex)
                {
                    //ex.SocketErrorCode
                   // MessageBox.Show("接收出错");
                   // MessageBox.Show("错误提示: \r\n" + ex.SocketErrorCode.ToString());           
                    //ex.ErrorCode
                    //connected = false;
                    //关闭TCP终端
                   // MessageShow("SOCKET错误:" + ex.SocketErrorCode.ToString());

                    have_ERRCODE = 1;
                    str_ErrCode = "SOCKET错误:" + ex.SocketErrorCode.ToString();

                   
                   
                }

                worker.ReportProgress(0);
                //Thread.Sleep(1); // ms
            }

        }

        /// <summary>
        /// 显示刷新
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UpdateUI(object sender, ProgressChangedEventArgs e)
        {
            //MessageBox.Show("UpdateUI");

            //状态显示更新
            RefreshStatus();
        }

        // 状态刷新
        private void RefreshStatus()
        {
            ////测试，刷新接收到的信息
            //MessageShow(mrecv_msg);

            if (have_ERRCODE == 1)
            {
                 MessageShow(str_ErrCode);
                // CommandSend("$");                 
                 have_ERRCODE = 0;
            }

            //电机运行状态
            CurrentStatus.Text = "当前位置：" + mangle.ToString() + "  当前状态：" + mstatus.ToString();
            CurrentStatus.Update();

            //任务完成信号，finish信号
            if (1 == mfinish)
            {
                MessageShow("到位完成");
                mfinish = -1;
            }

            //指令执行情况（成功执行1 / 未被执行0）
            if (1 == mcmdstatus) 
            {
                MessageShow("命令成功执行");
                mcmdstatus = -1;
            }
            else if (0 == mcmdstatus)
            {
                MessageShow("命令未被执行");
                mcmdstatus = -1;
            }

            //故障码
            if (!string.IsNullOrEmpty(merror_msg))
            {
                MessageShow("故障查询返回: "+merror_msg);
                merror_backup = merror_msg;
                merror_msg = string.Empty;
            }
            //速度信息
            if (!string.IsNullOrEmpty(mspeed_msg))
            {
                MessageShow("速度信息返回: "+mspeed_msg);
                mspeed_backup = mspeed_msg;
                mspeed_msg = string.Empty;
            }
        }

        //函数说明: 解析服务端发回的数据
        //信息格式说明： 注意，角度固定为小数点后有三位；状态为一位正数
        // $A角度，状态\r\n
        // $B\r\n 表示运动完成
        // $C\r\n 命令受理执行成功
        // $D\r\n 命令未受理
        // $E故障码,其他故障码\r\n
        // 格式：alm43210,寿命预警RS485故障OT超程
        // $F\r\n 编码器报警
        // $G速度，加时间，减速时间\r\n 注意，速度为浮点数，三位小数
        private void MessageParse(string msg)
        {
            // 信息格式
            string staus_pattern = @"\$A(-?\d+\.\d+),(\d)\r\n";
            string finish_pattern = @"\$B\r\n";
            string ack_pattern = @"\$C\r\n";
            string miss_pattern = @"\$D\r\n";
            string error_code_pattern = @"\$E\d+,\d+\r\n";
            string encoder_alarm_pattern = @"\$F\r\n";
            string speed_pattern = @"\$G\d+\.\d+,\d+,\d+\r\n";
            string sum_angle_pattern = @"\$H(-?\d+\.\d+),(.+)\r\n";

            // A响应: 角度，状态
            MatchCollection status_mc = Regex.Matches(msg, staus_pattern);
            if (status_mc.Count > 0)
            {
                mangle = Convert.ToDouble(status_mc[status_mc.Count - 1].Groups[1].Value);
                mstatus = Convert.ToInt32(status_mc[status_mc.Count - 1].Groups[2].Value);
            }
            // B响应: finish响应
            MatchCollection finish_mc = Regex.Matches(msg, finish_pattern);
            if (finish_mc.Count > 0)
            {
                //MessageBox.Show("Finish响应");
                mfinish = 1;
            }
            // C响应: 指令执行成功反馈（目前没有反馈）
            MatchCollection ack_mc = Regex.Matches(msg, ack_pattern);
            if (ack_mc.Count > 0)
            {
                mcmdstatus = 1;
            }
            // D响应: 指令未被执行反馈
            MatchCollection miss_mc = Regex.Matches(msg, miss_pattern);
            if (miss_mc.Count > 0)
            {
                mcmdstatus = 0;
            }
            // E响应: 故障码
            MatchCollection error_mc = Regex.Matches(msg, error_code_pattern);
            if (error_mc.Count > 0)
            {
                //MessageBox.Show("收到故障结果");
                merror_msg = Convert.ToString(error_mc[error_mc.Count-1].Value);
            }
            // F响应:编码器报警
            MatchCollection encoder_mc = Regex.Matches(msg, encoder_alarm_pattern);
            if (encoder_mc.Count >0)
            {
                MessageBox.Show("收到编码器报警");
            }
            // G响应: 速度信息返回
            MatchCollection speed_mc = Regex.Matches(msg, speed_pattern);
            if (speed_mc.Count > 0) 
            {
                //MessageBox.Show("收到速度信息");
                mspeed_msg = Convert.ToString(speed_mc[speed_mc.Count-1].Value);
            }
            // H响应: 总角度与信息返回
            MatchCollection sum_angle_mc = Regex.Matches(msg, sum_angle_pattern);
            if (sum_angle_mc.Count > 0)
            {
                msumangle = Convert.ToDouble(sum_angle_mc[sum_angle_mc.Count - 1].Groups[1].Value);
                minfo = Convert.ToString(sum_angle_mc[sum_angle_mc.Count - 1].Groups[2].Value);
            }
           

        }


        //// 函数说明: 超时判断,在指定时间内signal没有被置1, 返回-1.
        //// 说明: 超时时间 50*10ms
        private int WaitTimeOut(bool signal)
        {
            int waittime = 50; //50*10ms
            for (int i = 0; i < waittime; ++i)
            {
                // 超时判断
                if (i == waittime - 1)
                {
                    return -1;
                }
                if (signal) {
                    break;
                }else {
                    Thread.Sleep(10); // 10ms
                }
            }
            return 0;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (mclient != null) {
                mclient.Close();
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {

            timer1.Enabled = false;

            if (mstatus == 1 )
            {
                //连续运行 
                if (cmdstatus == 1 && checkBox1.Checked )
                {

                    string cmd = "check";
                    MessageShow(cmd);
                    CommandSend(cmd);
                    Thread.Sleep(2000);

                }
                else if (cmdstatus == 2 )  //扫描模式 
                {
                    if (Math.Abs(right_degree - mangle) < 0.3 )
                    {
                        if (checkBox1.Checked)
                        {
                            string cmd_left = "point " + left_degree.ToString();
                            MessageShow("LOOP LEFT:" + cmd_left);
                            CommandSend(cmd_left);
                            Thread.Sleep(2000);

                        }
                        else
                        {

                            cmdstatus = 0;
                        }
                       
                       
                    }
                    else if (Math.Abs(left_degree - mangle) < 0.3)
                    {
                        string cmd_right = "point " + right_degree.ToString();
                        MessageShow("RIGHT:" + cmd_right);
                        CommandSend(cmd_right);
                        Thread.Sleep(2000);
                    }

                }

            }

            timer1.Enabled = true;
            timer1.Start();
           
        }

        private void sum_click(object sender, EventArgs e)
        {

            if (mclient == null)
            {
                MessageBox.Show("未连接！");
                return;
            }
            string cmd = "sum";
            MessageShow(cmd);
            CommandSend(cmd);
            Thread.Sleep(500);

            String mstr = "当前运动累计角度为：" + msumangle + "\r\n";
            mstr = mstr + "硬件信息：" + minfo + "\r\n";
            MessageShow(mstr);
        }

        

       

    }
}
