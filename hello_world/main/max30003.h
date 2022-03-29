#ifndef MAX30003_H_
#define MAX30003_H_

#define WREG 0x00
#define RREG 0x01

#define   STATUS          0x01
#define   EN_INT          0x02
#define   EN_INT2         0x03
#define   MNGR_INT        0x04
#define   MNGR_DYN        0x05
#define   SW_RST          0x08
#define   SYNCH           0x09
#define   FIFO_RST        0x0A
#define   INFO            0x0F
#define   CNFG_GEN        0x10
#define   CNFG_CAL        0x12
#define   CNFG_EMUX       0x14
#define   CNFG_ECG        0x15
#define   CNFG_RTOR1      0x1D
#define   CNFG_RTOR2      0x1E
#define   ECG_FIFO_BURST  0x20
#define   ECG_FIFO        0x21
#define   RTOR            0x25
#define   NO_OP           0x7F

//Please check you pin ESP32 setup
#define PIN_SPI_CS      5
#define PIN_SPI_MOSI    23
#define PIN_SPI_MISO    19
#define PIN_SPI_SCK     18

#define PIN_NUM_FCLK      13

#define SAMPLES_PER_PACKET 8

void max30003_initchip(int pin_miso, int pin_mosi, int pin_sck, int pin_cs );
void max30003_sw_reset(void);
void max30003_synch(void);
void MAX30003_Reg_Write (unsigned char WRITE_ADDRESS, unsigned long data);
long max30003_read_ecg_data(void);

#endif
