#define UART_BASE 0x10000000L
#define UART(offset) (*(volatile char*)((UART_BASE) + (offset)))
#define R_TX 0   // 发送寄存器
#define R_RX 0   // 接收寄存器
#define R_DLL 0  // 除数锁存寄存器低位
#define R_DLM 1  // 除数锁存寄存器高位
#define R_FCR 2  // FIFO控制寄存器
#define R_LCR 3  // 线控寄存器
#define R_LSR 5  // 线状态寄存器

void putstr(const char *s){
    for(int i=0;s[i];i++){
        while (!(UART(R_LSR) & (1 << 5)))
            ;
        UART(R_TX) = s[i];
    }
}
