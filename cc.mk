.c.o:
	$(CC) $(my_cflags) $(ADD_INCLUDE) $(COMMON_CC_FLAGS) $(LAST_INCLUDE) -o $@ $<
.cpp.o:
	$(CC) $(my_cflags) $(ADD_INCLUDE) $(COMMON_CC_FLAGS) $(LAST_INCLUDE) -o $@ $<
.S.o:
	$(CC) $(my_cflags) $(ADD_INCLUDE) -D__ASSEMBLY__ $(COMMON_CC_FLAGS) $(LAST_INCLUDE) -o $@ $<
clean:
	@rm -f *.o *.a *.ad
	@for i in $(PARTS); do $(MAKE) -C $$i clean; done

subs_parts:
	@for i in $(PARTS); do $(MAKE) -C $$i; done
	
to_image:
	c:\\image\\winimage.exe d:\\q64\\hd0.img -I $(MY_BUILD_BASE)\\release