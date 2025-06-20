################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../driver/Flash_Programming.c" \
"../driver/asclin.c" \
"../driver/can.c" \
"../driver/start.c" \
"../driver/stm.c" 

COMPILED_SRCS += \
"driver/Flash_Programming.src" \
"driver/asclin.src" \
"driver/can.src" \
"driver/start.src" \
"driver/stm.src" 

C_DEPS += \
"./driver/Flash_Programming.d" \
"./driver/asclin.d" \
"./driver/can.d" \
"./driver/start.d" \
"./driver/stm.d" 

OBJS += \
"driver/Flash_Programming.o" \
"driver/asclin.o" \
"driver/can.o" \
"driver/start.o" \
"driver/stm.o" 


# Each subdirectory must supply rules for building sources it contributes
"driver/Flash_Programming.src":"../driver/Flash_Programming.c" "driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/haewoong/AURIX-v1.10.6-workspace/p3/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"driver/Flash_Programming.o":"driver/Flash_Programming.src" "driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"driver/asclin.src":"../driver/asclin.c" "driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/haewoong/AURIX-v1.10.6-workspace/p3/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"driver/asclin.o":"driver/asclin.src" "driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"driver/can.src":"../driver/can.c" "driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/haewoong/AURIX-v1.10.6-workspace/p3/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"driver/can.o":"driver/can.src" "driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"driver/start.src":"../driver/start.c" "driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/haewoong/AURIX-v1.10.6-workspace/p3/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"driver/start.o":"driver/start.src" "driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"driver/stm.src":"../driver/stm.c" "driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/haewoong/AURIX-v1.10.6-workspace/p3/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"driver/stm.o":"driver/stm.src" "driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-driver

clean-driver:
	-$(RM) ./driver/Flash_Programming.d ./driver/Flash_Programming.o ./driver/Flash_Programming.src ./driver/asclin.d ./driver/asclin.o ./driver/asclin.src ./driver/can.d ./driver/can.o ./driver/can.src ./driver/start.d ./driver/start.o ./driver/start.src ./driver/stm.d ./driver/stm.o ./driver/stm.src

.PHONY: clean-driver

