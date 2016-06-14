ASM := C:\Keil\ARM\BIN40\armasm.exe
CC := C:\Keil\ARM\BIN40\armcc.exe
LINK := C:\Keil\ARM\BIN40\armlink.exe
OBJCOPY := C:\Keil\ARM\BIN40\fromelf.exe
RM := del /q
AR := C:\Keil\ARM\BIN40\armar.exe
#PRT_LIB := prtlib.a

A_SRC := $(sort $(wildcard *.s))
C_SRC := $(sort $(wildcard *.c))
A_OBJ := $(patsubst %.s,%.o,$(A_SRC))
C_OBJ := $(patsubst %.c,%.o,$(C_SRC))


TARGET := M3soft

.PHONY : all clean

all : $(TARGET).bin $(TARGET).dfu $(TARGET).hex
#all : $(TARGET).hex

$(TARGET).bin : $(TARGET).axf
	$(OBJCOPY) --bin $< --output $@

$(TARGET).dfu : $(TARGET).bin
	stm32_crc32 $< $@


$(TARGET).hex : $(TARGET).axf
	$(OBJCOPY) --i32 $< --output $@

$(TARGET).axf : $(A_OBJ) $(C_OBJ) STM32_Flash.scat
	-$(LINK) --device=DARMSTM --feedback "fb.txt" --strict --scatter "STM32_Flash.scat" --list "$(TARGET).map" --output "$@" $(A_OBJ) $(C_OBJ)
	$(OBJCOPY) -z $@
	$(OBJCOPY) -z $(C_OBJ)

$(A_OBJ) : %.o : %.s Makefile
	-$(ASM) --device=DARMSTM --thumb --apcs=interwork --depend "$(basename $<).d" -o "$@" $<

$(C_OBJ) : %.o : %.c Makefile
	-$(CC) --device=DARMSTM --thumb --apcs=interwork --depend "$(basename $<).d" -O3 -Ospace --multibyte_chars -c --gnu -I. --feedback "fb.txt" -o "$@" -c $<

clean:
	-$(RM) *.d > nul
	-$(RM) *.o > nul
	-$(RM) fb.txt > nul
	-$(RM) $(TARGET).map > nul
	-$(RM) $(TARGET).axf > nul
	-$(RM) $(TARGET).hex > nul
	-$(RM) $(TARGET).bin > nul
	-$(RM) $(TARGET).dfu > nul

genlib:
	-$(RM) $(PRT_LIB)
	-$(AR) -r $(PRT_LIB) *.o
#	-$(AR) -d $(PRT_LIB) main.o event.o uart.o NormalMode.o MenuMode.o config.o printf.o esc_p.o fsmc_sram.o


-include $(wildcard *.d)
