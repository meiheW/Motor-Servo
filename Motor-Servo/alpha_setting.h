// 文件描述: 电机的寄存器配置与底层控制
#ifndef ALPHA_SETTING_H
#define ALPHA_SETTING_H

#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

// 发送次数，用于发送失败时的重复发送
#define     OPLOOPS     (2)

// 寄存器地址, 用于寄存器参数设置
#define     PA1_01_ad    (0x4000)
#define     PA1_02_ad    (0x4001)
#define     PA1_03_ad    (0x4002)
#define     PA1_04_ad    (0x4003)
#define     PA1_05_ad    (0x4004)
#define     PA1_06_ad    (0x4005)
#define     PA1_07_ad    (0x4006)
#define     PA1_12_ad    (0x400B)
#define     PA1_33_ad    (0x4020)
#define     PA1_35_ad    (0x4022)

#define     PA2_01_ad    (0x4100)
#define     PA2_25_ad    (0x4118)
#define     PA2_26_ad    (0x4119)
#define     PA2_27_ad    (0x411A)
#define     PA2_28_ad    (0x411B)
#define     PA2_29_ad    (0x411C)
#define     PA2_40_ad    (0x4127)
#define     PA2_41_ad    (0x4128)
#define     PA2_72_ad    (0x4147)
#define     PA2_73_ad    (0x4148)
#define     PA2_93_ad    (0x415C)
#define     PA2_94_ad    (0x415D)
#define     PA2_95_ad    (0x415E)
#define     PA2_97_ad    (0x4160)
#define     PA2_99_ad    (0x4162)

#define     PA3_01_ad    (0x4200)
#define     PA3_02_ad    (0x4201)
#define     PA3_03_ad    (0x4202)
#define     PA3_04_ad    (0x4203)
#define     PA3_05_ad    (0x4204)
#define     PA3_09_ad    (0x4208)
#define     PA3_10_ad    (0x4209)
#define     PA3_11_ad    (0x420A)
#define     PA3_12_ad    (0x420B)
#define     PA3_13_ad    (0x420C)
#define     PA3_14_ad    (0x420D)
#define     PA3_15_ad    (0x420E)
#define     PA3_16_ad    (0x420F)
#define     PA3_17_ad    (0x4210)
#define     PA3_18_ad    (0x4211)
#define     PA3_19_ad    (0x4212)
#define     PA3_20_ad    (0x4213)
#define     PA3_21_ad    (0x4214)
#define     PA3_22_ad    (0x4215)
#define     PA3_23_ad    (0x4216)
#define     PA3_24_ad    (0x4217)
#define     PA3_36_ad    (0x4223)

#define     PA3_51_ad    (0x4232)
#define     PA3_52_ad    (0x4233)
#define     PA3_53_ad    (0x4234)
#define     PA3_56_ad    (0x4237)
#define     PA3_57_ad    (0x4238)
#define     PA3_58_ad    (0x4239)
#define     PA3_59_ad    (0x423A)
#define     PA3_60_ad    (0x423B)
#define     PA3_61_ad    (0x423C)
#define     PA3_62_ad    (0x423D)
#define     PA3_63_ad    (0x423E)
#define     PA3_64_ad    (0x423F)
#define     PA3_65_ad    (0x4240)
#define     PA3_66_ad    (0x4241)
#define     PA3_67_ad    (0x4242)
#define     PA3_68_ad    (0x4243)
#define     PA3_69_ad    (0x4244)
#define     PA3_70_ad    (0x4245)
#define     PA3_71_ad    (0x4246)

// 用于立即值控制的寄存器
#define     IMME_VLU_STATUS_ad              (0x5100)
#define     IMME_VLU_POSITION_ad            (0x5101)
#define     IMME_VLU_SPEED_ad               (0x5102)
#define     IMME_VLU_ACC_TIM_ad             (0x5103)
#define     IMME_VLU_DEC_TIM_ad             (0x5104)

// 线圈地址，用于信号控制 
// (注意: 后面的寄存器编号表示，设置指定的寄存器就可以使一种信号与线圈绑定)
#define     CONT1_ad        (0x0400)        //PA3_01
#define     CONT2_ad        (0x0401)        //PA3_02
#define     CONT3_ad        (0x0402)        //PA3_03
#define     CONT4_ad        (0x0403)        //PA3_04
#define     CONT5_ad        (0x0404)        //PA3_05

#define     CONT9_ad        (0x0208)        //PA3_09
#define     CONT10_ad       (0x0209)        //PA3_10
#define     CONT11_ad       (0x020A)        //PA3_11
#define     CONT12_ad       (0x020B)        //PA3_12
#define     CONT13_ad       (0x020C)        //PA3_13
#define     CONT14_ad       (0x020D)        //PA3_14
#define     CONT15_ad       (0x020E)        //PA3_15
#define     CONT16_ad       (0x020F)        //PA3_16
#define     CONT17_ad       (0x0210)        //PA3_17
#define     CONT18_ad       (0x0211)        //PA3_18
#define     CONT19_ad       (0x0212)        //PA3_19
#define     CONT20_ad       (0x0213)        //PA3_20
#define     CONT21_ad       (0x0214)        //PA3_21
#define     CONT22_ad       (0x0215)        //PA3_22
#define     CONT23_ad       (0x0216)        //PA3_23
#define     CONT24_ad       (0x0217)        //PA3_24

#define     OUT1_ad         (0x0500)        //PA3_51
#define     OUT2_ad         (0x0501)        //PA3_52
#define     OUT3_ad         (0x0502)        //PA3_53

#define     OUT6_ad         (0x0305)        //PA3_56
#define     OUT7_ad         (0x0306)        //PA3_57
#define     OUT8_ad         (0x0307)        //PA3_58
#define     OUT9_ad         (0x0308)        //PA3_59
#define     OUT10_ad        (0x0309)        //PA3_60
#define     OUT11_ad        (0x030A)        //PA3_61
#define     OUT12_ad        (0x030B)        //PA3_62
#define     OUT13_ad        (0x030C)        //PA3_63
#define     OUT14_ad        (0x030D)        //PA3_64
#define     OUT15_ad        (0x030E)        //PA3_65
#define     OUT16_ad        (0x030F)        //PA3_66
#define     OUT17_ad        (0x0310)        //PA3_67
#define     OUT18_ad        (0x0311)        //PA3_68
#define     OUT19_ad        (0x0312)        //PA3_69
#define     OUT20_ad        (0x0313)        //PA3_70
#define     OUT21_ad        (0x0314)        //PA3_71

// 输入信号的功能编码，用于对参数寄存器赋值，使相应信号与线圈绑定
#define     SERVO_ON_fc                     (1)
#define     FWD_fc                          (2)
#define     REV_fc                          (3)
#define     START_fc                        (4)
#define     ORG_fc                          (5)
#define     LS_fc                           (6)
#define     OT_PLUS_fc                      (7)
#define     OT_MINUS_fc                     (8)
#define     EMG_fc                          (10)
#define     RST_fc                          (11)
#define     ACCO_fc                         (14)
#define     PST_PRESET_fc                   (16)
#define     GAIN_SWITCH_fc                  (17)
#define     TRQ_LMT0_fc                     (19)
#define     TRQ_LMT1_fc                     (20)
#define     IMME_VLU_CTINU_fc               (22)
#define     IMME_VLU_CHG_fc                 (23)
#define     ELCT_GEAR_NUM_SEL0_fc           (24)
#define     ELCT_GEAR_NUM_SEL1_fc           (25)
#define     CMD_PULSE_INHIBIT_fc            (26)
#define     CMD_PULSE_RTO1_fc               (27)
#define     COM_PULSE_RTO2_fc               (28)
#define     PROPORTIONAL_CTRL_fc            (29)
#define     PAUSE_fc                        (31)
#define     PST_CANCEL_fc                   (32)
#define     EXTR_REGN_RESIST_OH_fc          (34)
#define     TEACHING_fc                     (35)
#define     CTRL_MOD_SLCT_fc                (36)
#define     PST_CTRL_fc                     (37)
#define     TRQ_CTRL_fc                     (38)
#define     OVERRIDE_EN_fc                  (43)
#define     OVERRIDE1_fc                    (44)
#define     OVERRIDE2_fc                    (45)
#define     OVERRIDE4_fc                    (46)
#define     OVERRIDE8_fc                    (47)
#define     INTRUPT_IN_EN_fc                (48)
#define     INTRUPT_IN_fc                   (49)
#define     DEVIATION_CLR_fc                (50)
#define     X1_fc                           (51)
#define     X2_fc                           (52)
#define     X3_fc                           (53)
#define     FREE_RUN_fc                     (54)
#define     EDT_PERMISSION_fc               (55)
#define     ANTI_FREQ_SLCT0_fc              (57)
#define     ANTI_FREQ_SLCT1_fc              (58)
#define     AD0_fc                          (60)
#define     AD1_fc                          (61)
#define     AD2_fc                          (62)
#define     AD3_fc                          (63)
#define     PST_DATA_SLCT_fc                (77)
#define     BRDCST_CANCEL_fc                (78)

// 输出信号的功能编码，用于对参数寄存器赋值，使相应信号与线圈绑定
#define     RDY_fc                          (1)
#define     INP_fc                          (2)
#define     SPD_LMT_DETC_fc                 (11)
#define     OVER_WR_CMPL_fc                 (13)
#define     BRK_TIMING_fc                   (14)
#define     ALRM_DECT_A_fc                  (16)
#define     POINT_DETC_AREA1_fc             (17)
#define     POINT_DETC_AREA2_fc             (18)
#define     LMTER_DETC_fc                   (19)
#define     OT_DETC_fc                      (20)
#define     CYC_END_DETC_fc                 (21)
#define     HMING_CMPL_fc                   (22)
#define     ZRO_DEVI_fc                     (23)
#define     ZRO_SPEED_fc                    (24)
#define     SPD_COINCD_fc                   (25)
#define     TRQ_LMT_DETC_fc                 (26)
#define     ORLD_WRNING_fc                  (27)
#define     S_RDY_fc                        (28)
#define     ED_PRMSION_RSP_fc               (29)
#define     DATA_ERROR_fc                   (30)
#define     ADDR_ERROR_fc                   (31)
#define     ALRM_CODE0_fc                   (32)
#define     ALRM_CODE1_fc                   (33)
#define     ALRM_CODE2_fc                   (34)
#define     ALRM_CODE3_fc                   (35)
#define     ALRM_CODE4_fc                   (36)
#define     OT_PLUS_DETC_fc                 (38)
#define     OT_MINUS_DETC_fc                (39)
#define     HM_PST_LS_DETC_fc               (40)
#define     FRC_STP_DETC_fc                 (41)
#define     BATRY_WRNING_fc                 (45)
#define     LIFE_WRNING_fc                  (46)
#define     MD0_fc                          (60)
#define     MD1_fc                          (61)
#define     MD2_fc                          (62)
#define     MD3_fc                          (63)
#define     MD4_fc                          (64)
#define     MD5_fc                          (65)
#define     MD6_fc                          (66)
#define     MD7_fc                          (67)
#define     PST_PRST_CMPL_fc                (75)
#define     ALRM_DETC_B_fc                  (76)
#define     IMME_VLU_CNTIN_PERMS_fc         (79)
#define     IMME_VLU_CNTIN_CMPL_fc          (80)
#define     IMME_VLU_CHG_CMPL_fc            (81)
#define     CMD_PST_CMPL_fc                 (82)
#define     RANGE1_OF_PST_fc                (83)
#define     RANGE2_OF_PST_fc                (84)
#define     INTRUPT_PST_DECT_fc             (85)
#define     CONT_A_THROUGH_fc               (91)
#define     CONT_B_THROUGH_fc               (92)
#define     CONT_C_THROUGH_fc               (93)
#define     CONT_D_THROUGH_fc               (94)
#define     CONT_E_THROUGH_fc               (95)

// 对绑定后的线圈地址重命名，便于突出其信号的功能 [输入信号]
#define     SERVO_ON_ad                 (CONT9_ad)      //伺服
#define     FWD_ad                      
#define     REV_ad                      
#define     START_ad                    (CONT11_ad)     //自动启动
#define     ORG_ad                      (CONT1_ad)      //原点复位ORG
#define     LS_ad                       (CONT2_ad)      //原点LS
#define     OT_PLUS_ad                  (CONT12_ad)     //+方向超程
#define     OT_MINUS_ad                 (CONT13_ad)     //-方向超程
#define     EMG_ad                      (CONT14_ad)     //强制停止EMG
#define     RST_ad                      (CONT10_ad)     //报警复位RST
#define     ACCO_ad                     
#define     PST_PRESET_ad               (CONT15_ad)     //位置预置
#define     GAIN_SWITCH_ad              
#define     TRQ_LMT0_ad                 
#define     TRQ_LMT1_ad                 
#define     IMME_VLU_CTINU_ad           (CONT16_ad)     //立即值继续指令
#define     IMME_VLU_CHG_ad             (CONT17_ad)     //立即值变更指令
#define     ELCT_GEAR_NUM_SEL0_ad       
#define     ELCT_GEAR_NUM_SEL1_ad       
#define     CMD_PULSE_INHIBIT_ad        
#define     CMD_PULSE_RTO1_ad           
#define     COM_PULSE_RTO2_ad           
#define     PROPORTIONAL_CTRL_ad        
#define     PAUSE_ad                    (CONT18_ad)     //临时停止
#define     PST_CANCEL_ad               (CONT19_ad)     //定位取消
#define     EXTR_REGN_RESIST_OH_ad      
#define     TEACHING_ad                 (CONT20_ad)     //示教
#define     CTRL_MOD_SLCT_ad            
#define     PST_CTRL_ad                 
#define     TRQ_CTRL_ad                 
#define     OVERRIDE_EN_ad              
#define     OVERRIDE1_ad                
#define     OVERRIDE2_ad                
#define     OVERRIDE4_ad                
#define     OVERRIDE8_ad                
#define     INTRUPT_IN_EN_ad            
#define     INTRUPT_IN_ad               
#define     DEVIATION_CLR_ad            (CONT21_ad)     //偏差清除
#define     X1_ad                       
#define     X2_ad                       
#define     X3_ad                       
#define     FREE_RUN_ad                 (CONT22_ad)     //自由运转BX
#define     EDT_PERMISSION_ad           
#define     ANTI_FREQ_SLCT0_ad          
#define     ANTI_FREQ_SLCT1_ad          
#define     AD0_ad                      (CONT23_ad)     //AD0
#define     AD1_ad                                      //AD1
#define     AD2_ad                                      //AD2
#define     AD3_ad                                      //AD3
#define     PST_DATA_SLCT_ad            
#define     BRDCST_CANCEL_ad            

// 对绑定后的线圈地址重命名，便于突出其信号的功能 [输出信号]
#define     RDY_ad                          (OUT1_ad)       //运行准备结束RDY
#define     INP_ad                          (OUT2_ad)       //定位结束INP
#define     SPD_LMT_DETC_ad                 
#define     OVER_WR_CMPL_ad                 
#define     BRK_TIMING_ad                   (OUT6_ad)       //制动器时机
#define     ALRM_DECT_A_ad                
#define     POINT_DETC_AREA1_ad             
#define     POINT_DETC_AREA2_ad             
#define     LMTER_DETC_ad                   
#define     OT_DETC_ad                      (OUT7_ad)       //OT检测，输入信号的OT处于OFF时，该信号ON
#define     CYC_END_DETC_ad                 
#define     HMING_CMPL_ad                   (OUT8_ad)       //原点复位结束
#define     ZRO_DEVI_ad                     (OUT9_ad)       //偏差零
#define     ZRO_SPEED_ad                    (OUT10_ad)      //速度零
#define     SPD_COINCD_ad                   
#define     TRQ_LMT_DETC_ad                 
#define     ORLD_WRNING_ad                  
#define     S_RDY_ad                        (OUT11_ad)      //伺服准备就绪
#define     ED_PRMSION_RSP_ad               
#define     DATA_ERROR_ad                   (OUT12_ad)      //数据错误，RS485不能正常读写
#define     ADDR_ERROR_ad                   
#define     ALRM_CODE0_ad                   (OUT13_ad)      //报警代码0
#define     ALRM_CODE1_ad                   (OUT14_ad)      //报警代码1
#define     ALRM_CODE2_ad                   (OUT15_ad)      //报警代码2
#define     ALRM_CODE3_ad                   (OUT16_ad)      //报警代码3
#define     ALRM_CODE4_ad                   (OUT17_ad)      //报警代码4
#define     OT_PLUS_DETC_ad                 
#define     OT_MINUS_DETC_ad                
#define     HM_PST_LS_DETC_ad               
#define     FRC_STP_DETC_ad                 
#define     BATRY_WRNING_ad                 
#define     LIFE_WRNING_ad                  (OUT18_ad)      //使用寿命预报
#define     MD0_ad                          
#define     MD1_ad                          
#define     MD2_ad                          
#define     MD3_ad                          
#define     MD4_ad                          
#define     MD5_ad                          
#define     MD6_ad                          
#define     MD7_ad                          
#define     PST_PRST_CMPL_ad                (OUT19_ad)      //位置预置结束
#define     ALRM_DETC_B_ad                  (OUT3_ad)       //报警检测b接，OFF为报警
#define     IMME_VLU_CNTIN_PERMS_ad         (OUT20_ad)      //立即值继续许可
#define     IMME_VLU_CNTIN_CMPL_ad          
#define     IMME_VLU_CHG_CMPL_ad            
#define     CMD_PST_CMPL_ad                 (OUT21_ad)      //指令定位结束
#define     RANGE1_OF_PST_ad                
#define     RANGE2_OF_PST_ad                
#define     INTRUPT_PST_DECT_ad             
#define     CONT_A_THROUGH_ad               
#define     CONT_B_THROUGH_ad               
#define     CONT_C_THROUGH_ad               
#define     CONT_D_THROUGH_ad               
#define     CONT_E_THROUGH_ad               

// 伺服电机的每圈脉冲数量、传动减速比
#define     M_PULSE_PER_CIRCLE          (36000)
#define     TRANSMISSION_RATIO          (202*10)
// 为modbus控制准备的缓冲区内存
#define		REGISTERS_BUFFER_SIZE       (12)
#define		BITS_BUFFER_SIZE	        (24)


// modbus句柄
extern modbus_t *ctx;
extern uint16_t *tab_rq_registers;
extern uint8_t  *tab_rq_bits;
extern uint16_t *tab_rp_registers; 
extern uint8_t	*tab_rp_bits;

// modbus参数对象格式
struct rtu_master_t;
typedef struct rtu_master_t rtu_master_t;
struct rtu_master_t{
        char device[32];
        int baud;
        char parity;
        int data_bit;
        int stop_bit;
        int slave;
        int reset_parameter; // 使用指定参数重置控制器寄存器
};


// 初始化modbus控制使用的内存缓冲区
// 返回值: 0 表示初始化成功，-1 表示失败
int init_buffers_for_modbus();
// 释放内存空间
void free_buffers_for_modbus();

// 打开modbus控制终端
// 返回值: 0 表示打开成功，-1 表示失败
int open_modbus_rtu_master(const char *device, int baud, char parity, int data_bit, int stop_bit, int slave);
// 关闭终端
void close_modbus_rtu_master();

// 初始化寄存器，reset_parameter设置为1时调用
void init_parameters();
// 确认寄存器的设置正确
// 返回值: 0 表示校验通过， -1 表示寄存器的设置有误
int check_parameters();

#ifdef __cplusplus
}
#endif
#endif	//ALPHA_SETTING_H
