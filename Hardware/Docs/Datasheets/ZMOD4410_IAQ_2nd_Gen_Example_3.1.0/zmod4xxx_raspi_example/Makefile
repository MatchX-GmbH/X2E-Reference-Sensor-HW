
LIB			:= iaq_2nd_gen
CC			:= gcc
SRC			:= src/
HAL			:= HAL/
LIB_DIR 	:= ../gas-algorithm-libraries/${LIB}/Raspberry\ Pi/ARMv8-A/arm-linux-gnueabihf-gcc/
GAS_LIB 	:= ${LIB_DIR}/lib_${LIB}.a
CLEAN_LIB	:= ${LIB_DIR}/lib_zmod4xxx_cleaning.a
EXEC		:= zmod4xxx_${LIB}_rpi
FLAGS		:= -lpthread -lrt -lm -lpigpio

zmod4xxx_rpi:
	${CC} -I$(SRC) -I$(HAL) -I$(LIB_DIR) -DRASPI -o ${EXEC} $(SRC)/*.c $(HAL)/*.c ${GAS_LIB} ${CLEAN_LIB} ${FLAGS}

clean:
	rm -f *rpi
