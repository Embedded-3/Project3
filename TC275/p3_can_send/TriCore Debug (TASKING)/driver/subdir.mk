################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../driver/asclin.c" \
"../driver/can.c" \
"../driver/stm.c" 

COMPILED_SRCS += \
"driver/asclin.src" \
"driver/can.src" \
"driver/stm.src" 

C_DEPS += \
"./driver/asclin.d" \
"./driver/can.d" \
"./driver/stm.d" 

OBJS += \
"driver/asclin.o" \
"driver/can.o" \
"driver/stm.o" 


# Each subdirectory must supply rules for building sources it contributes
"driver/asclin.src":"../driver/asclin.c" "driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/haewoong/AURIX-v1.10.6-workspace/p3_can_send/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"driver/asclin.o":"driver/asclin.src" "driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"driver/can.src":"../driver/can.c" "driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/haewoong/AURIX-v1.10.6-workspace/p3_can_send/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"driver/can.o":"driver/can.src" "driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"driver/stm.src":"../driver/stm.c" "driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/haewoong/AURIX-v1.10.6-workspace/p3_can_send/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"driver/stm.o":"driver/stm.src" "driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-driver

clean-driver:
	-$(RM) ./driver/asclin.d ./driver/asclin.o ./driver/asclin.src ./driver/can.d ./driver/can.o ./driver/can.src ./driver/stm.d ./driver/stm.o ./driver/stm.src

.PHONY: clean-driver

