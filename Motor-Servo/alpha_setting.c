#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "elog.h"
#include "alpha_setting.h"


// 用于设置伺服OT，超程范围
static const int32_t plus_position = M_PULSE_PER_CIRCLE * TRANSMISSION_RATIO;
static const int32_t minus_position = (-M_PULSE_PER_CIRCLE) * TRANSMISSION_RATIO;
// modbus通信句柄
modbus_t *ctx = NULL;
uint16_t *tab_rq_registers = NULL;
uint8_t  *tab_rq_bits = NULL;
uint16_t *tab_rp_registers = NULL; 
uint8_t	 *tab_rp_bits = NULL;


// 基本寄存器设置，立即数控制模式（绝对控制系统需要电池，现在的控制器没有安装外部电池）
// 相对位置系统，电源被切断，则丢失当前位置，需要再次进行原点复归。
void parameter_register_setting();
// 进行线圈与信号的绑定
void io_signals_mapping();
// 参数设定
void init_parameters()
{
	parameter_register_setting();
	io_signals_mapping();
}
// 确认参数配置
int check_parameters();


// Init/Free bufffers for modbus-TRU
int init_buffers_for_modbus()
{
	tab_rq_registers = (uint16_t *) malloc(REGISTERS_BUFFER_SIZE * sizeof(uint16_t));
	memset(tab_rq_registers, 0, REGISTERS_BUFFER_SIZE * sizeof(uint16_t));
	tab_rp_registers = (uint16_t *) malloc(REGISTERS_BUFFER_SIZE * sizeof(uint16_t));
	memset(tab_rp_registers, 0, REGISTERS_BUFFER_SIZE  * sizeof(uint16_t));
	tab_rq_bits = (uint8_t *)malloc(BITS_BUFFER_SIZE * sizeof(uint8_t));
	memset(tab_rq_bits, 0, BITS_BUFFER_SIZE * sizeof(uint8_t));
	tab_rp_bits = (uint8_t *)malloc(BITS_BUFFER_SIZE * sizeof(uint8_t));
	memset(tab_rp_bits, 0, BITS_BUFFER_SIZE * sizeof(uint8_t));
	if(tab_rq_registers == NULL || tab_rp_registers == NULL || tab_rq_bits == NULL || tab_rp_bits == NULL) return -1;
	return 0;
}
void free_buffers_for_modbus()
{
	free(tab_rq_registers);
	free(tab_rp_registers);
	free(tab_rq_bits);
	free(tab_rp_bits);
	tab_rq_registers = NULL;
	tab_rp_registers = NULL;
	tab_rq_bits = NULL;
	tab_rp_bits = NULL;
}


// 打开 modbus-RTU 控制端
int open_modbus_rtu_master(const char *device, int baud, char parity, int data_bit, int stop_bit, int slave)
{
	int rc;
	ctx = modbus_new_rtu(device , baud, parity, data_bit, stop_bit);
	if(ctx == NULL){
		return -1;
	}
	// SLAVE_ID
	rc = modbus_set_slave(ctx,slave);
	if(-1 == rc){
		modbus_free(ctx);
		return -1;
	}
	// Debug mode
	//modbus_set_debug(ctx, TRUE);
	if (modbus_connect(ctx) == -1) {
		modbus_free(ctx);
		return -1;
	}
	return 0;
}
void close_modbus_rtu_master()
{
	modbus_close(ctx);
	modbus_free(ctx);
}


// 写寄存器
void write_register(int addr, uint16_t val0, uint16_t val1)
{
	tab_rq_registers[0] = val0;
	tab_rq_registers[1] = val1;
	modbus_write_registers(ctx,  addr,  2,  tab_rq_registers);
}

// 线圈与信号的绑定
void io_signals_mapping()
{
	// 输入信号设定
	// PA3_01 CONT1 (5)原点复位ORG_fc 
	write_register( PA3_01_ad, 0x0000, ORG_fc);
	// PA3_02 CONT2 (6)原点LS LS_fc     
	write_register( PA3_02_ad, 0x0000, LS_fc);
	// PA3_03 CONT3 
	write_register( PA3_03_ad, 0x0000, 0);
	// PA3_04 CONT4 
	write_register( PA3_04_ad, 0x0000, 0);
	// PA3_05 CONT5 
	write_register( PA3_05_ad, 0x0000, 0);
	// PA3_09 CONT9 (1)伺服    
	write_register( PA3_09_ad, 0x0000, SERVO_ON_fc);
	// PA3_10 CONT10 (11)报警复位   
	write_register( PA3_10_ad, 0x0000, RST_fc);
	// PA3_11 CONT11 (4) 自动启动
	write_register( PA3_11_ad, 0x0000, START_fc);
	// PA3_12 CONT12 (7)+方向超程+OT
	write_register( PA3_12_ad, 0x0000, OT_PLUS_fc );
	// PA3_13 CONT13 (8)-方向超程-OT
	write_register( PA3_13_ad, 0x0000, OT_MINUS_fc );
	// PA3_14 CONT14 (10)强制停止EMG
	write_register( PA3_14_ad, 0x0000, EMG_fc );
	// PA3_15 CONT15 (16)位置预置
	write_register( PA3_15_ad, 0x0000, PST_PRESET_fc );
	// PA3_16 CONT16 (22)立即值继续指令
	write_register( PA3_16_ad, 0x0000, IMME_VLU_CTINU_fc );
	// PA3_17 CONT17 (23)立即值变更指令
	write_register( PA3_17_ad, 0x0000, IMME_VLU_CHG_fc );
	// PA3_18 CONT18 (31)临时停止
	write_register( PA3_18_ad, 0x0000, PAUSE_fc );
	// PA3_19 CONT19 (32)定位取消
	write_register( PA3_19_ad, 0x0000, PST_CANCEL_fc );
	// PA3_20 CONT20 (35)示教
	write_register( PA3_20_ad, 0x0000, TEACHING_fc );
	// PA3_21 CONT21 (50)偏差清除，寄存器PA3_36设定清除时的输入形态
	write_register( PA3_21_ad, 0x0000, DEVIATION_CLR_fc );
	// PA3_22 CONT22 (54)自由运转BX
	write_register( PA3_22_ad, 0x0000, FREE_RUN_fc );
	// PA3_23 CONT23 (60)AD0
	write_register( PA3_23_ad, 0x0000, AD0_fc );

	// 输出信号设定
	// PA3_51 OUT1 (1)运行准备结束RDY
	write_register( PA3_51_ad, 0x0000, RDY_fc);
	// PA3_52 OUT2 (2)定位结束INP
	write_register( PA3_52_ad, 0x0000, INP_fc);
	// PA3_53 OUT3 (76)报警检测b接
	write_register( PA3_53_ad, 0x0000, ALRM_DETC_B_fc);
	// PA3_56 OUT6 (14)制动器时机
	write_register( PA3_56_ad, 0x0000, BRK_TIMING_fc);
	// PA3_57 OUT7 (20)OT检测，输入信号的OT处于OFF时，该信号ON
	write_register( PA3_57_ad, 0x0000, OT_DETC_fc);
	// PA3_58 OUT8 (22)原点复位结束
	write_register( PA3_58_ad, 0x0000, HMING_CMPL_fc);
	// PA3_59 OUT9 (23)偏差零
	write_register( PA3_59_ad, 0x0000, ZRO_DEVI_fc);
	// PA3_60 OUT10 (24)速度零
	write_register( PA3_60_ad, 0x0000, ZRO_SPEED_fc);
	// PA3_61 OUT11 (28)伺服准备就绪
	write_register( PA3_61_ad, 0x0000, S_RDY_fc);
	// PA3_62 OUT12 (30)数据错误，RS485不能正常读取
	write_register( PA3_62_ad, 0x0000, DATA_ERROR_fc);
	// PA3_63 OUT13 (32)报警代码0
	write_register( PA3_63_ad, 0x0000, ALRM_CODE0_fc);
	// PA3_64 OUT14 (33)报警代码1
	write_register( PA3_64_ad, 0x0000, ALRM_CODE1_fc);
	// PA3_65 OUT15 (34)报警代码2
	write_register( PA3_65_ad, 0x0000, ALRM_CODE2_fc);
	// PA3_66 OUT16 (35)报警代码3
	write_register( PA3_66_ad, 0x0000, ALRM_CODE3_fc);
	// PA3_67 OUT17 (36)报警代码4
	write_register( PA3_67_ad, 0x0000, ALRM_CODE4_fc);
	// PA3_68 OUT18 (46)使用寿命预报
	write_register( PA3_68_ad, 0x0000, LIFE_WRNING_fc);
	// PA3_69 OUT19 (75)位置预置结束
	write_register( PA3_69_ad, 0x0000, PST_PRST_CMPL_fc);
	// PA3_70 OUT20 (79)立即值继续许可
	write_register( PA3_70_ad, 0x0000, IMME_VLU_CNTIN_PERMS_fc);
	// PA3_71 OUT21 (82)指令定位结束
	write_register( PA3_71_ad, 0x0000, CMD_PST_CMPL_fc);

}


// 基本寄存器设置，立即数控制模式
// 相对位置系统，电源被切断，则丢失当前位置，需要再次进行原点复归。
void parameter_register_setting()
{
	//Part1. 基本设定参数()
	// PA1_01=7 定位运行
	write_register( PA1_01_ad, 0x0000, 0x0007);
	// PA1_02=0 INC 控制系统
	write_register( PA1_02_ad, 0x0000, 0x0000);
	// PA1_03=1,指令脉冲形式，默认值
	write_register( PA1_03_ad, 0x0000, 0x0001);
	// PA1_04=0,旋转方向切换，默认值
	write_register( PA1_04_ad, 0x0000, 0x0000);
	// PA1_05 每转脉冲数设置
	write_register( PA1_05_ad, (M_PULSE_PER_CIRCLE >> 16) & 0xFFFF, M_PULSE_PER_CIRCLE & 0xFFFF);
	// PA1_06,07 默认值 16,1，注意 用于将脉冲转化为单位量[unit] = 度/pulse
	write_register( PA1_06_ad, 0x0000, 16);
	write_register( PA1_07_ad, 0x0000, 1);
	// PA1_12=0,Z相偏置默认值
	write_register( PA1_12_ad, 0x0000, 0x0000);

	// PA1_13=10 自整定模式，默认值
	// PA1_14=1.0 负载惯性力矩比，默认值
	// PA1_15 自整定增益1
	// PA1_16 自整定增益2
	// PA1_25 最大转速
	// PA1_27=300% 正转转矩限制值，默认值
	// PA1_28=300% 反转转矩限制值，默认值
	// TODO PA1_29=50r/min 速度一致范围
	// TODO PA1_30=50r/min 零速度范围
	// TODO PA1_31=0 偏差单位选择，0为单位量，1为脉冲量
	// TODO PA1_32=100 偏差零范围/定位结束范围 (受偏差单位选择的影响)
	
	// PA1_33~35 定位结束的信号配置INP
	// PA1_33=0 等级形态
	write_register( PA1_33_ad, 0x0000, 0x0000);
	// PA1_35=0 判断时间，默认值
	write_register( PA1_35_ad, 0x0000, 0x0000);

	// PA1-36~40 加速时间和减速时间的设定
	// TODO 注意！ 加减速时间设定是到达0(零)~2000(r/min)的时间设定
	
	//Part2. 控制增益、滤波器设定参数()
	// PA1_54 位置指令响应时间常数
	// PA1_58=0.000 前馈增益1,默认值
	// PA1_61~67 第2增益设定
	// PA1_68=0% 加速度增益，默认值
	
	//Part3. 自动运行设定参数()
	// PA2_01=0 位置数据小数点位置，默认值
	write_register( PA2_01_ad, 0x0000, 0x0000);

	// TODO PA2_06~18、24 原点复归设定 (重要！！！) 由于当前时INC控制系统，每次需要原点复归
	// PA2_06=500r/min 原点复归速度，默认值
	// PA2_07=50r/min 原点复归爬行速度，默认值
	// PA2_08=0 正转，原点复归起动方向，默认值
	// PA2_09=0 单位量，原点复归反转偏移量，默认值
	// PA2_10=0 正转，原点复归方向
	// PA2_11=1,编码器Z相，原点位移量剧准信号，默认值
	// PA2_12=0,原点基准信号，原点LS,默认值
	// PA2_13=0,原点LS时机选择，ON边缘时机，默认值
	// PA2_14=1000,单位量，原点偏移量，默认值
	// PA2_15=0,爬行速度减速动作，反转无效，默认值
	// PA2_16=0,单位量，浮动原点位置，默认值
	// PA2_17=0,原点检测范围，结束之后常时ON，默认值
	// PA2_18=100ms,原点复位OT时减速时间，默认值
	// PA2_22=0ms,挡块检测时间，默认值
	// PA2_23=0,挡块转矩限制值，默认值
	// PA2_24=0,原点复归OT测出时运行选择，反转，默认值
	// PA1_37~40 设定原点复归动作过程中的加减速度
	
	// PA2_60=300%,第三转矩限制值，默认值
	// PA2_19=0, 单位量，预置位置，默认值
	
	// PA2_25~27 软件OT设置，OT检测
	// PA1_01=7, PA2_25=0; 0 为 PTP mode, 1 为 INC mode and OT invalid
	write_register( PA2_25_ad, 0x0000, 0x0000);
	// +OT position > -OT position
	write_register( PA2_26_ad, (plus_position >> 16) & 0xFFFF, plus_position & 0xFFFF);
	write_register( PA2_27_ad, (minus_position >> 16) & 0xFFFF, minus_position & 0xFFFF);
	// PA2_28~29 限制器检测位置
	write_register( PA2_28_ad, (plus_position >> 16) & 0xFFFF, plus_position & 0xFFFF);
	write_register( PA2_29_ad, (minus_position >> 16) & 0xFFFF, minus_position & 0xFFFF);
	// PA2_40=0 RS485立即值数据运行
	write_register( PA2_40_ad, 0x0000, 0x0000);
	// PA2_41=3 顺次起动有效/无效，默认为0; 0无效，1有效，2原点复归，3立即值数据运行
	write_register( PA2_41_ad, 0x0000, 0x0003);

	// PA2_42 停止定时小数点位置
	// PA2_43 M代码OFF时输出选择
	// PA2_44=0，定位扩展功能，默认值
	
	//Part4. 扩展功能设定参数
	// PA2_51~53 电子齿轮分子1、2、3
	// PA2_57~60 转矩限制设定
	// 默认 PA2_57=0, PA2_58=300%, PA2_59=0, PA2_60=300%
	// PA2_61~63 动作指令序列设定
	// 默认 PA2_61=5, PA2_62=5, PA2_63=5
	// PA2_64=0s, 制动器动作时间，伺服由ON置于OFF后，到自由运转状态的时间（功能码14,提示释放伺服完成）
	// PA2_65=1，选择再生电阻，默认值
	// PA2_67=1，电压不足时报警检测，默认为检测
	// PA2_69=15[rev]，偏差超出检测值，默认值
	// PA2_70=50%，过载预报值，默认值
	
	// PA2_72=1，局号，默认为1
	write_register( PA2_72_ad, 0x0000, 0x0001);
	// PA2_73=0，默认波特率38400
	write_register( PA2_73_ad, 0x0000, 0x0000);
	// PA2_93=0，默认值，偶校验，1个停止位
	write_register( PA2_93_ad, 0x0000, 0x0000);
	// PA2_94=0，响应时间0ms，默认值
	write_register( PA2_94_ad, 0x0000, 0x0000);
	// PA2_95=0，通信超时时间，默认值0,不检测
	write_register( PA2_95_ad, 0x0000, 0x0000);
	// PA2_97=1，通信协议选择，1为modbus
	write_register( PA2_97_ad, 0x0000, 0x0001);
	// PA2_99=0，选择编码器，默认值0为20bit编码器
	write_register( PA2_99_ad, 0x0000, 0x0000);

	//
	// PA3_36=0，设定偏差清零信号的有效形态，0为边缘ON边缘清除，1为等级，至少保持2ms以上才有效
	write_register( PA3_36_ad, 0x0000, 0x0000);

}


int check_io(int addr, int code)
{
	if(2 != modbus_read_registers(ctx,  addr,  2,  tab_rp_registers)) {
		printf("Modbus failed!!!!!!!!!!!!!!\n");
		return -1;
	}
	if(tab_rp_registers[0] != 0 || tab_rp_registers[1] != code) return -1;
	return 0;
}
int check_register(int addr, uint16_t val0, uint16_t val1)
{
	if(2 != modbus_read_registers(ctx,  addr,  2,  tab_rp_registers)) return -1;
	if(tab_rp_registers[0] != val0 || tab_rp_registers[1] != val1) return -1;
	return 0;
}



// Debug函数，查看register读取结果
void debug_resiter_result(int addr)
{
	if(2 != modbus_read_registers(ctx,  addr,  2,  tab_rp_registers)) return;
	printf("addr: 0x%4x val0: 0x%4x val1: 0x%4x\n", addr, tab_rp_registers[0], tab_rp_registers[1]);
}
// Debug函数，从地址addr开始，查看后面那个寄存器的值
void debug_func_code(int addr, int len)
{
	for(int i=0; i<len; ++i)
	{
		if(2 != modbus_read_registers(ctx,  addr+i,  2,  tab_rp_registers)) return;
		printf("addr: 0x%4x val0: 0x%4d val1: 0x%4d\n", addr, tab_rp_registers[0], tab_rp_registers[1]);
	}
}


// 确认参数配置
int check_parameters()
{
	int ret;

	// // Debug语句，获取引脚配置功能码
	// debug_func_code(PA3_01_ad, 5);
	// debug_func_code(PA3_09_ad, 16);
	// debug_func_code(PA3_51_ad, 3);
	// debug_func_code(PA3_56_ad, 16);

	// 输入信号设定
	// PA3_01 CONT1 (1)伺服
	ret = check_io( PA3_01_ad, ORG_fc ); if(-1==ret) return -1;
	// PA3_02 CONT2 (11)报警复位
	ret = check_io( PA3_02_ad, LS_fc ); if(-1==ret) return -1;
	// PA3_03 CONT3 (4)自动启动
	ret = check_io( PA3_03_ad, 0 ); if(-1==ret) return -1;
	// PA3_04 CONT4 (5)原点复位ORG
	ret = check_io( PA3_04_ad, 0 ); if(-1==ret) return -1;
	// PA3_05 CONT5 (6)原点LS
	ret = check_io( PA3_05_ad, 0 ); if(-1==ret) return -1;
	// PA3_09 CONT9 (7)+方向超程+OT
	ret = check_io( PA3_09_ad, SERVO_ON_fc ); if(-1==ret) return -1;
	// PA3_10 CONT10 (8)-方向超程-OT
	ret = check_io( PA3_10_ad, RST_fc ); if(-1==ret) return -1;
	// PA3_11 CONT11 (10)强制停止EMG
	ret = check_io( PA3_11_ad, START_fc ); if(-1==ret) return -1;
	// PA3_12 CONT12 (16)位置预置
	ret = check_io( PA3_12_ad, OT_PLUS_fc ); if(-1==ret) return -1;
	// PA3_13 CONT13 (22)立即值继续指令
	ret = check_io( PA3_13_ad, OT_MINUS_fc ); if(-1==ret) return -1;
	// PA3_14 CONT14 (23)立即值变更指令
	ret = check_io( PA3_14_ad, EMG_fc ); if(-1==ret) return -1;
	// PA3_15 CONT15 (31)临时停止
	ret = check_io( PA3_15_ad, PST_PRESET_fc ); if(-1==ret) return -1;
	// PA3_16 CONT16 (32)定位取消
	ret = check_io( PA3_16_ad, IMME_VLU_CTINU_fc ); if(-1==ret) return -1;
	// PA3_17 CONT17 (35)示教
	ret = check_io( PA3_17_ad, IMME_VLU_CHG_fc ); if(-1==ret) return -1;
	// PA3_18 CONT18 (50)偏差清除
	ret = check_io( PA3_18_ad, PAUSE_fc ); if(-1==ret) return -1;
	// PA3_19 CONT19 (54)自由运转BX
	ret = check_io( PA3_19_ad, PST_CANCEL_fc ); if(-1==ret) return -1;
	// PA3_20 CONT20 (60)AD0
	ret = check_io( PA3_20_ad, TEACHING_fc ); if(-1==ret) return -1;
	// PA3_21 CONT21 (61)AD1
	ret = check_io( PA3_21_ad, DEVIATION_CLR_fc ); if(-1==ret) return -1;
	// PA3_22 CONT22 (62)AD2
	ret = check_io( PA3_22_ad, FREE_RUN_fc ); if(-1==ret) return -1;
	// PA3_23 CONT23 (63)AD3
	ret = check_io( PA3_23_ad, AD0_fc ); if(-1==ret) return -1;

	// 输出信号设定
	// PA3_51 OUT1 (1)运行准备结束RDY
	ret = check_io( PA3_51_ad, RDY_fc ); if(-1==ret) return -1;
	// PA3_52 OUT2 (2)定位结束INP
	ret = check_io( PA3_52_ad, INP_fc ); if(-1==ret) return -1;
	// PA3_53 OUT3 (76)报警检测b接
	ret = check_io( PA3_53_ad, ALRM_DETC_B_fc); if(-1==ret) return -1;
	// PA3_56 OUT6 (14)制动器时机
	ret = check_io( PA3_56_ad, BRK_TIMING_fc ); if(-1==ret) return -1;
	// PA3_57 OUT7 (20)OT检测，输入信号的OT处于OFF时，该信号ON
	ret = check_io( PA3_57_ad, OT_DETC_fc ); if(-1==ret) return -1;
	// PA3_58 OUT8 (22)原点复位结束
	ret = check_io( PA3_58_ad, HMING_CMPL_fc ); if(-1==ret) return -1;
	// PA3_59 OUT9 (23)偏差零
	ret = check_io( PA3_59_ad, ZRO_DEVI_fc ); if(-1==ret) return -1;
	// PA3_60 OUT10 (24)速度零
	ret = check_io( PA3_60_ad, ZRO_SPEED_fc ); if(-1==ret) return -1;
	// PA3_61 OUT11 (28)伺服准备就绪
	ret = check_io( PA3_61_ad, S_RDY_fc ); if(-1==ret) return -1;
	// PA3_62 OUT12 (30)数据错误，RS485不能正常读取
	ret = check_io( PA3_62_ad, DATA_ERROR_fc ); if(-1==ret) return -1;
	// PA3_63 OUT13 (32)报警代码0
	ret = check_io( PA3_63_ad, ALRM_CODE0_fc ); if(-1==ret) return -1;
	// PA3_64 OUT14 (33)报警代码1
	ret = check_io( PA3_64_ad, ALRM_CODE1_fc ); if(-1==ret) return -1;
	// PA3_65 OUT15 (34)报警代码2
	ret = check_io( PA3_65_ad, ALRM_CODE2_fc ); if(-1==ret) return -1;
	// PA3_66 OUT16 (35)报警代码3
	ret = check_io( PA3_66_ad, ALRM_CODE3_fc ); if(-1==ret) return -1;
	// PA3_67 OUT17 (36)报警代码4
	ret = check_io( PA3_67_ad, ALRM_CODE4_fc ); if(-1==ret) return -1;
	// PA3_68 OUT18 (46)使用寿命预报
	ret = check_io( PA3_68_ad, LIFE_WRNING_fc ); if(-1==ret) return -1;
	// PA3_69 OUT19 (75)位置预置结束
	ret = check_io( PA3_69_ad, PST_PRST_CMPL_fc ); if(-1==ret) return -1;
	// PA3_70 OUT20 (79)立即值继续许可
	ret = check_io( PA3_70_ad, IMME_VLU_CNTIN_PERMS_fc ); if(-1==ret) return -1;
	// PA3_71 OUT21 (82)指令定位结束
	ret = check_io( PA3_71_ad, CMD_PST_CMPL_fc ); if(-1==ret) return -1;


	//Part1. 基本设定参数()
	// PA1_01=7 定位运行
	ret = check_register( PA1_01_ad, 0x0000, 0x0007); if(-1==ret) return -1;
	// PA1_02=0 INC 控制系统
	ret = check_register( PA1_02_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA1_03=1,指令脉冲形式，默认值
	ret = check_register( PA1_03_ad, 0x0000, 0x0001); if(-1==ret) return -1;
	// PA1_04=0,旋转方向切换，默认值
	ret = check_register( PA1_04_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA1_05 每转脉冲数设置
	ret = check_register( PA1_05_ad, (M_PULSE_PER_CIRCLE >> 16) & 0xFFFF, M_PULSE_PER_CIRCLE & 0xFFFF); if(-1==ret) return -1;
	// PA1_06,07 默认值 16,1，注意 用于将脉冲转化为单位量[unit] = 度/pulse
	ret = check_register( PA1_06_ad, 0x0000, 16); if(-1==ret) return -1;
	ret = check_register( PA1_07_ad, 0x0000, 1); if(-1==ret) return -1;
	// PA1_12=0,Z相偏置默认值
	ret = check_register( PA1_12_ad, 0x0000, 0x0000); if(-1==ret) return -1;

	// PA1_13=10 自整定模式，默认值
	// PA1_14=1.0 负载惯性力矩比，默认值
	// PA1_15 自整定增益1
	// PA1_16 自整定增益2
	// PA1_25 最大转速
	// PA1_27=300% 正转转矩限制值，默认值
	// PA1_28=300% 反转转矩限制值，默认值
	// TODO PA1_29=50r/min 速度一致范围
	// TODO PA1_30=50r/min 零速度范围
	// TODO PA1_31=0 偏差单位选择，0为单位量，1为脉冲量
	// TODO PA1_32=100 偏差零范围/定位结束范围 (受偏差单位选择的影响)
	
	// PA1_33~35 定位结束的信号配置INP
	// PA1_33=0 等级形态
	ret = check_register( PA1_33_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA1_35=0 判断时间，默认值
	ret = check_register( PA1_35_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA1-36~40 加速时间和减速时间的设定
	// TODO 注意！ 加减速时间设定是到达0(零)~2000(r/min)的时间设定
	
	//Part2. 控制增益、滤波器设定参数()
	// PA1_54 位置指令响应时间常数
	// PA1_58=0.000 前馈增益1,默认值
	// PA1_61~67 第2增益设定
	// PA1_68=0% 加速度增益，默认值
	
	//Part3. 自动运行设定参数()
	// PA2_01=0 位置数据小数点位置，默认值
	ret = check_register( PA2_01_ad, 0x0000, 0x0000); if(-1==ret) return -1;

	// TODO PA2_06~18、24 原点复归设定 (重要！！！) 由于当前时INC控制系统，每次需要原点复归
	// PA2_06=500r/min 原点复归速度，默认值
	// PA2_07=50r/min 原点复归爬行速度，默认值
	// PA2_08=0 正转，原点复归起动方向，默认值
	// PA2_09=0 单位量，原点复归反转偏移量，默认值
	// PA2_10=0 正转，原点复归方向
	// PA2_11=1,编码器Z相，原点位移量剧准信号，默认值
	// PA2_12=0,原点基准信号，原点LS,默认值
	// PA2_13=0,原点LS时机选择，ON边缘时机，默认值
	// PA2_14=1000,单位量，原点偏移量，默认值
	// PA2_15=0,爬行速度减速动作，反转无效，默认值
	// PA2_16=0,单位量，浮动原点位置，默认值
	// PA2_17=0,原点检测范围，结束之后常时ON，默认值
	// PA2_18=100ms,原点复位OT时减速时间，默认值
	// PA2_22=0ms,挡块检测时间，默认值
	// PA2_23=0,挡块转矩限制值，默认值
	// PA2_24=0,原点复归OT测出时运行选择，反转，默认值
	// PA1_37~40 设定原点复归动作过程中的加减速度
	
	// PA2_60=300%,第三转矩限制值，默认值
	// PA2_19=0, 单位量，预置位置，默认值
	
	// PA2_25~27 软件OT设置，OT检测
	// PA1_01=7, PA2_25=0; 0 为 PTP mode, 1 为 INC mode and OT invalid
	ret = check_register( PA2_25_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// +OT position > -OT position
	ret = check_register( PA2_26_ad, (plus_position >> 16) & 0xFFFF, plus_position & 0xFFFF); if(-1==ret) return -1;
	ret = check_register( PA2_27_ad, (minus_position >> 16) & 0xFFFF, minus_position & 0xFFFF); if(-1==ret) return -1;
	// PA2_28~29 限制器检测位置
	ret = check_register( PA2_28_ad, (plus_position >> 16) & 0xFFFF, plus_position & 0xFFFF); if(-1==ret) return -1;
	ret = check_register( PA2_29_ad, (minus_position >> 16) & 0xFFFF, minus_position & 0xFFFF); if(-1==ret) return -1;
	// PA2_40=0 RS485立即值数据运行
	ret = check_register( PA2_40_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA2_41=3 顺次起动有效/无效，默认为0; 0无效，1有效，2原点复归，3立即值数据运行
	ret = check_register( PA2_41_ad, 0x0000, 0x0003); if(-1==ret) return -1;

	// PA2_42 停止定时小数点位置
	// PA2_43 M代码OFF时输出选择
	// PA2_44=0，定位扩展功能，默认值
	
	//Part4. 扩展功能设定参数
	// PA2_51~53 电子齿轮分子1、2、3
	// PA2_57~60 转矩限制设定
	// 默认 PA2_57=0, PA2_58=300%, PA2_59=0, PA2_60=300%
	// PA2_61~63 动作指令序列设定
	// 默认 PA2_61=5, PA2_62=5, PA2_63=5
	// PA2_64=0s, 制动器动作时间，伺服由ON置于OFF后，到自由运转状态的时间（功能码14,提示释放伺服完成）
	// PA2_65=1，选择再生电阻，默认值
	// PA2_67=1，电压不足时报警检测，默认为检测
	// PA2_69=15[rev]，偏差超出检测值，默认值
	// PA2_70=50%，过载预报值，默认值
	
	// PA2_72=1，局号，默认为1
	ret = check_register( PA2_72_ad, 0x0000, 0x0001); if(-1==ret) return -1;
	// PA2_73=0，默认波特率38400
	ret = check_register( PA2_73_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA2_93=0，默认值，偶校验，1个停止位
	ret = check_register( PA2_93_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA2_94=0，响应时间0.00ms，默认值
	ret = check_register( PA2_94_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA2_95=0，通信超时时间，默认值0,不检测
	ret = check_register( PA2_95_ad, 0x0000, 0x0000); if(-1==ret) return -1;
	// PA2_97=1，通信协议选择，1为modbus
	ret = check_register( PA2_97_ad, 0x0000, 0x0001); if(-1==ret) return -1;
	// PA2_99=0，选择编码器，默认值0为20bit编码器
	ret = check_register( PA2_99_ad, 0x0000, 0x0000); if(-1==ret) return -1;

	//
	// PA3_36=0，设定偏差清零信号的有效形态，0为边缘ON边缘清除，1为等级，至少保持2ms以上才有效
	ret = check_register( PA3_36_ad, 0x0000, 0x0000); if(-1==ret) return -1;

	return 0;
}

