
<!DOCTYPE html>
<head>
    <meta charset="UTF-8">
</head>
<h1>make_note.html</h1>
<p style="white-space:pre-wrap">


## passing parameters

make CFLAG=-DEBUG	

## template

kbd.o command.o files.o : command.h	

## close print

@	close the echo	
$(obj)%.s:    %.S	
$(CPP) $(AFLAGS) -o $@ $<	
$(obj)%.o:    %.S	
$(CC) $(AFLAGS) -c -o $@ $<	
$(obj)%.o:    %.c	
$(CC) $(CFLAGS) -c -o $@ $<	

## examples

ifeq ($(CONFIG_IEC_COMMON),y)	
filename		:= common	
current_dir		:= $(current_dir)$(filename)/	
sub_inc_path	+= $(current_dir)inc/	
obj-$(CONFIG_LINKED_LIST_IEC)			+= linked_list.o	
current_dir		:=$(patsubst %/,%,$(dir $(patsubst %/,%,$(current_dir))))/	
endif #HAL	

## Kbuild

menuconfig IEC_COMMON	
tristate "iec_common"	
if IEC_COMMON	
config LINKED_LIST_IEC	
tristate "linked_list"	
endif #IEC_COMMON	

## make Kbuild

#auto make Kconfig depend on *.c	
srcs = $(basename $(wildcard *.c))	
show: 	
$(foreach src,$(srcs),$(call write_Kconfig, $(src)))	
define write_Kconfig	
echo "config $(shell echo $(1) | tr [a-z] [A-Z])" >> Kconfig	
echo "	tristate "$(1)"" >> Kconfig	
endef	

## 

defconfig:	
./scripts/conf Kconfig	
./scripts/conf -s --syncconfig Kconfig	
menuconfig:	
./scripts/mconf Kconfig	
./scripts/conf -s --syncconfig Kconfig	

## 

#ts
ifeq ($(CONFIG_TS),y)
sub	:= ts
sub_inc_path 	:=
sub_src_path 	:=
include $(sub)/Makefile
inc_path		+= $(sub_inc_path)
src_path		+= $(sub_src_path)
current_dir		:=
endif #CONFIG_TS
#net
ifeq ($(CONFIG_EXAMPLES),y)
sub	:= examples
sub_inc_path 	:=
sub_src_path 	:=
include $(sub)/Makefile
inc_path		+= $(sub_inc_path)
src_path		+= $(sub_src_path)
current_dir		:=
endif #CONFIG_NET
$(obj)/hello: $(targets)
	$(CC) $^ $(ld_flags) -o $@
$(obj)/%.o: %.s config
	$(CC) -c $(c_flags) -o $@ $<
$(obj)/%.s: %.i config
	$(CC) -S $(c_flags) -o $@ $<
$(obj)/%.i: %.c config
	$(CC) -E $(c_flags) -o $@ $<
$(obj)/%.a: $(obj)/%.o config FORCE
	$(LD) $(KBUILD_LDFLAGS) -r -o $(@D)/.tmp_$(@F) $@
$(obj)/%.o: $(src)/%.S $(objtool_dep) config FORCE
	$(CC) $(c_flags) -c -o $@ $<
#----------------------end-----------------------------
//---------------------end------------------------------
</p>
