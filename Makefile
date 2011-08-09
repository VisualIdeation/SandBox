
FREEVR_INCLUDE_DIR = /opt/local/include/FreeVR
FREEVR_LIB_DIR = /opt/local/lib

BITS = -o32
BITS =
CFLAGS = -g $(BITS) -I$(FREEVR_INCLUDE_DIR) -IHershey/src/.
LFLAGS = -g $(BITS)


LIBS = -L$(FREEVR_LIB_DIR) -lfreevr_64pt Hershey/src/libhershey.a -lGL -lX11 -lXi -lm -lpthread

SNDLIB =

sandbox: SANDBOX_demo_ogl.o Hershey/src/libhershey.a
	cc $(LFLAGS) -o $@ SANDBOX_demo_ogl.o $(LIBS)

Hershey/src/libhershey.a:
	(cd Hershey/src; make libhershey.a)


SANDBOX_demo_ogl.o : SANDBOX_demo_ogl.c SANDBOX_objectcode_ogl.c SANDBOX_data_ogl.c SANDBOX_toolcode_ogl.c

.c.o:
	$(CC) $(CFLAGS) -c $<

dist:
	mkdir Sandbox
	cp -p SANDBOX_demo_ogl.c SANDBOX_objectcode_ogl.c SANDBOX_data_ogl.c SANDBOX_toolcode_ogl.c Sandbox/.
	cp -p Makefile TODO README README.sound Sandbox/.
	(cd Hershey/src; make clean)
	cp -pr Hershey times.* Sandbox/.
	cp -prL SANDBOX_tool.vals SANDBOX_param.vals SANDBOX_site.vals Sandbox/.
	cp -prL SANDBOX_aug SANDBOX_sat pics Sandbox/.
	tar -cf sandbox_ogl_yyyymmdd.tar Sandbox
	#rm -rf Sandbox

clean:
	rm -f *.o core*

